/*
    Facundo García - 328757
    Juan Martín Lasserre - 289139
    Universidad ORT Uruguay - M4A - 2025
*/

#include <cassert>
#include <string>
#include <iostream>
#include <limits>
#include "tads/ListImp.cpp"

using namespace std;

class NodoRecurso
{
public:
    string dominio, path, titulo;
    int tiempo;

    NodoRecurso() : dominio(""), path(""), titulo(""), tiempo(0) {}
    NodoRecurso(string dominio, string path, string titulo, int tiempo) : dominio(dominio), path(path), titulo(titulo), tiempo(tiempo) {}
    bool operator==(const NodoRecurso &otroNodo)
    {
        return (this->dominio == otroNodo.dominio && this->path == otroNodo.path);
    }
};

class NodoDominio
{
public:
    string dominio;
    ListImp<int> indices;

    NodoDominio(string dom) : dominio(dom), indices(ListImp<int>()) {}
    bool operator==(const NodoDominio &otroNodo)
    {
        return this->dominio == otroNodo.dominio;
    }
};

class NodoPosicion
{
public:
    int arrayPosicion[2];

    NodoPosicion(int ubicacionEnHash, int index)
    {
        arrayPosicion[0] = ubicacionEnHash;
        arrayPosicion[1] = index;
    }

    // necesito overload del == para poder usar ListImp<NodoPosicion>
    bool operator==(const NodoPosicion &otroNodo)
    {
        return (this->arrayPosicion[0] == otroNodo.arrayPosicion[0] && this->arrayPosicion[1] == otroNodo.arrayPosicion[1]); // O(1)
    }
};

class NodoHash
{
public:
    NodoHash()
    {
        dominio = path = titulo = "";
        tiempo = 0;
        libre = true;
        bucket = ListImp<NodoRecurso *>();
    }

    NodoHash(string dominio, string path, string titulo, int tiempo) : dominio(dominio), path(path), titulo(titulo), tiempo(tiempo), libre(false), indices(ListImp<NodoDominio *>()), bucket(ListImp<NodoRecurso *>()) {}
    ListImp<NodoRecurso *> bucket;  // solo para dominio_path
    ListImp<NodoDominio *> indices; // solo para dominios
    string dominio;
    string path;
    string titulo;
    int tiempo;
    bool libre;
};

// Forward declaration to allow pointer members
class TablaHash
{
private:
    NodoHash **tabla;
    int cantidad;
    int largo;

    // Adaptado de: https://cp-algorithms.com/string/string-hashing.html (polynomial rolling hash function)
    unsigned int miHash1(string key)
    {
        int p = 31;
        unsigned int hash_value = 0;
        unsigned int p_pow = 1;
        int m = 1e9 + 9;
        for (char c : key)
        {
            hash_value = (hash_value + (c - 'a' + 1) * p_pow) % m;
            p_pow = (p_pow * p) % m;
        }
        return hash_value;
    }

public:
    TablaHash(int largo)
    {
        this->largo = largo;
        cantidad = 0;
        tabla = new NodoHash *[largo];
        for (int i = 0; i < largo; i++)
            tabla[i] = nullptr;
    }

    int cantidadElementos()
    {
        return cantidad;
    }

    int insertarEnPaths(string dominio, string path, string titulo, int tiempo)
    {
        float fc = (float)(cantidad + 1) / (float)largo;
        if (fc > 0.7)
            return -1;
        int pos = miHash1(dominio + path) % largo;
        if (!tabla[pos]) // nullptr
        {
            NodoRecurso *aAgregar = new NodoRecurso(dominio, path, titulo, tiempo);
            tabla[pos] = new NodoHash(dominio, path, titulo, tiempo);
            tabla[pos]->bucket.insert(aAgregar);
            cantidad++;
            return pos;
        }
        else if (tabla[pos]->libre) // nodo libre
        {
            tabla[pos]->dominio = dominio;
            tabla[pos]->path = path;
            tabla[pos]->titulo = titulo;
            tabla[pos]->tiempo = tiempo;
            tabla[pos]->libre = false;
            NodoRecurso *aAgregar = new NodoRecurso(dominio, path, titulo, tiempo);
            tabla[pos]->bucket.insert(aAgregar);
            cantidad++;
            return pos;
        }
        else
        { // nodo ocupado, puede ser update
            int largo = tabla[pos]->bucket.getSize();
            for (int i = 0; i < largo; i++)
            {
                NodoRecurso *rec = tabla[pos]->bucket.get(0);
                if (rec->dominio == dominio && rec->path == path)
                {
                    rec->tiempo = tiempo;
                    rec->titulo = titulo;
                    return pos; // no aumento cantidad porque actualicé un nodo
                }
                tabla[pos]->bucket.removeAt(0); // O(1)
                tabla[pos]->bucket.insert(rec); // O(1)
            }
            NodoRecurso *aAgregar = new NodoRecurso(dominio, path, titulo, tiempo);
            tabla[pos]->bucket.insert(aAgregar);
            cantidad++;
            return pos;
        }
    }

    int cantDominio(string dominio)
    {
        int pos = miHash1(dominio) % largo;
        if (!tabla[pos] || tabla[pos]->libre)
            return 0;
        int largo = tabla[pos]->indices.getSize();
        for (int i = 0; i < largo; i++)
        {
            NodoDominio *dom = tabla[pos]->indices.get(0);
            if (dom->dominio == dominio)
                return dom->indices.getSize();
            tabla[pos]->indices.removeAt(0);
            tabla[pos]->indices.insert(dom);
        }
        return 0;
    }

    bool contiene(string dominio, string path)
    {
        int pos = miHash1(dominio + path) % largo;
        if (!tabla[pos] || tabla[pos]->libre)
            return false;
        int largo = tabla[pos]->bucket.getSize();
        for (int i = 0; i < largo; i++)
        {
            NodoRecurso *rec = tabla[pos]->bucket.get(0);
            if (rec->dominio == dominio && rec->path == path)
                return true;
            tabla[pos]->bucket.removeAt(0);
            tabla[pos]->bucket.insert(rec);
        }
        return false;
    }

    void insertarDominio(string dominio, int index)
    {
        float fc = (float)(cantidad + 1) / (float)largo;
        if (index == -1)
            return;
        int pos = miHash1(dominio) % largo;
        if (!tabla[pos] && fc <= 0.7)
        {
            tabla[pos] = new NodoHash(dominio, "", "", 0);
            NodoDominio *dom = new NodoDominio(dominio);
            dom->indices.insert(index);
            tabla[pos]->indices.insert(dom);
            cantidad++;
        }
        else if (tabla[pos]->libre && fc <= 0.7)
        {
            tabla[pos]->dominio = dominio;
            tabla[pos]->libre = false;
            NodoDominio *dom = new NodoDominio(dominio);
            dom->indices.insert(index);
            tabla[pos]->indices.insert(dom);
            cantidad++;
        }
        else
        {
            int largo = tabla[pos]->indices.getSize(); // esta es la cantidad de dominios en el bucket
            bool actualizo = false;
            for (int i = 0; i < largo && !actualizo; i++)
            {
                NodoDominio *nodoAct = tabla[pos]->indices.get(0);
                if (nodoAct->dominio == dominio)
                {
                    int largoDom = nodoAct->indices.getSize();
                    for (int i = 0; i < largoDom; i++)
                    {
                        int currentIndex = nodoAct->indices.get(0);
                        if (currentIndex == index)
                        {
                            actualizo = true;
                            break;
                        }
                        nodoAct->indices.removeAt(0);
                        nodoAct->indices.insert(currentIndex);
                    }
                    // recorrí todos los índices del dominio y no encontré el que quiero insertar; debo insertarlo
                    nodoAct->indices.insertAt(0, index);
                    return;
                }
                tabla[pos]->indices.removeAt(0);
                tabla[pos]->indices.insert(nodoAct);
            }
            // recorrí todos los dominios de esa ubicación y ninguno era el que buscaba: debo agregarlo
            if (actualizo || fc > 0.7)
                return;
            NodoDominio *dom = new NodoDominio(dominio);
            dom->indices.insert(index);
            tabla[pos]->indices.insert(dom);
            cantidad++;
        }
    }

    void buscar(string dominio, string path)
    {
        int pos = miHash1(dominio + path) % largo;
        if (!tabla[pos] || tabla[pos]->libre)
        {
            cout << "recurso_no_encontrado" << endl;
            return;
        }
        int largo = tabla[pos]->bucket.getSize();
        for (int i = 0; i < largo; i++)
        {
            NodoRecurso *rec = tabla[pos]->bucket.get(0);
            if (rec->dominio == dominio && rec->path == path)
            {
                cout << rec->titulo << " " << rec->tiempo << endl;
                return;
            }
            tabla[pos]->bucket.removeAt(0);
            tabla[pos]->bucket.insert(rec);
        }
        cout << "recurso_no_encontrado" << endl;
    }

    ListImp<int>* getRecursosByDominio(string dominio){
        int pos = miHash1(dominio)%largo;
        if (!tabla[pos] || tabla[pos]->libre) return nullptr;
        int largo = tabla[pos]->indices.getSize();
        for (int i = 0; i < largo; i++){
            NodoDominio* dom = tabla[pos]->indices.get(0);
            if (dom->dominio == dominio) return &dom->indices;
            tabla[pos]->indices.removeAt(0);
            tabla[pos]->indices.insert(dom);
        }
        return nullptr;
    }

    void listarPaths (ListImp<int>*& lista, string dominio){
        int largoRecs = lista->getSize();
        for (int i = 0; i < largoRecs; i++){
            int index = lista->get(0);
            ListImp<NodoRecurso*> listaRecs = tabla[index]->bucket;
            int largoListaRecs = listaRecs.getSize();
            for (int j = 0; j < largoListaRecs; j++){
                NodoRecurso* currentIndex = listaRecs.get(0);
            }
            lista->removeAt(0);
            lista->insert(index);
        }
    }
};

// Declaración de la clase Cache (mismos atributos, métodos vacíos)
class Cache
{
public:
    TablaHash *dominio_path;
    TablaHash *dominios;

    unsigned int nextPrime(unsigned int n)
    {
        if (n <= 1)
            return 2;
        while (true)
        {
            bool isPrime = true;
            for (int i = 2; i < n && isPrime && i * i <= n; i++)
            {
                if (n % i == 0)
                    isPrime = false;
            }
            if (isPrime)
                return n;
            n++;
        }
    }
    Cache(int n)
    {
        int largo = nextPrime(n * 2);
        dominio_path = new TablaHash(largo);
        dominios = new TablaHash(largo);
    }

    void PUT(string dominio, string path, string titulo, int tiempo)
    {
        int index = dominio_path->insertarEnPaths(dominio, path, titulo, tiempo);
        dominios->insertarDominio(dominio, index);
    }
    void GET(string dominio, string path)
    {
        dominio_path->buscar(dominio, path);
    }
    void REMOVE(string dominio, string path)
    {
    }
    void CONTAINS(string dominio, string path)
    {
        if (dominio_path->contiene(dominio, path))
            cout << "true" << endl;
        else
            cout << "false" << endl;
    }
    void COUNT_DOMAIN(string dominio)
    {
        cout << dominios->cantDominio(dominio) << endl;
    }
    void LIST_DOMAIN(string dominio)
    {
    }
    void CLEAR_DOMAIN(string dominio)
    {
    }
    void SIZE()
    {
        cout << dominio_path->cantidadElementos() << endl;
    }
    void CLEAR()
    {
    }
};

int main()
{
    int N;
    string command;
    cin >> N;
    Cache *cache = new Cache(N);
    for (int i = 0; i < N; i++)
    {
        cin >> command;
        // cout << "Ejecutando comando " << command << ", num " << i + 2 << ": ";
        if (command == "PUT")
        {
            string dominio, path, titulo;
            int tiempo;
            cin >> dominio >> path >> titulo >> tiempo;
            cache->PUT(dominio, path, titulo, tiempo);
        }
        else if (command == "GET")
        {
            string dominio, path;
            cin >> dominio >> path;
            cache->GET(dominio, path);
        }
        else if (command == "REMOVE")
        {
            string dominio, path;
            cin >> dominio >> path;
            cache->REMOVE(dominio, path);
        }
        else if (command == "CONTAINS")
        {
            string dominio, path;
            cin >> dominio >> path;
            cache->CONTAINS(dominio, path);
        }
        else if (command == "COUNT_DOMAIN")
        {
            string dominio;
            cin >> dominio;
            cache->COUNT_DOMAIN(dominio);
        }
        else if (command == "LIST_DOMAIN")
        {
            string dominio;
            cin >> dominio;
            cache->LIST_DOMAIN(dominio);
        }
        else if (command == "CLEAR_DOMAIN")
        {
            string dominio;
            cin >> dominio;
            cache->CLEAR_DOMAIN(dominio);
        }
        else if (command == "SIZE")
        {
            cache->SIZE();
        }
        else if (command == "CLEAR")
        {
            cache->CLEAR();
        }
        else
        {
            cout << "Comando_inválido" << endl;
        }
        // cout << endl;
    }
    return 0;
}