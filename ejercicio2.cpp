/*
    -----------------------------------
    Facundo García - 328757
    Juan Martín Lasserre - 289139
    -----------------------------------
    Universidad ORT Uruguay - M4A - 2025
    -----------------------------------
*/

#include <cassert>
#include <string>
#include <iostream>
#include <limits>
#include "tads/ListImp.cpp"
using namespace std;
class NodoLista
{
public:
    NodoLista *sig;
    NodoLista *ant;
    int index;

    NodoLista(int i) : sig(NULL), ant(NULL), index(i) {}
};

class Lista
{
public:
    NodoLista *head;
    NodoLista *tail;
    int largo;

    Lista() : head(NULL), tail(NULL), largo(0) {}

    // PRE:
    // POS: inserta el elemento al comienzo de la lista
    void insertar(int index)
    {
        NodoLista *nuevo = new NodoLista(index);
        if (estaVacia())
        {
            head = tail = nuevo;
        }
        else
        {
            head->ant = nuevo;
            nuevo->sig = head;
            head = nuevo;
        }
        largo++;
    }

    // PRE:
    // POS: elimina index de la lista, retorna true si lo eliminó, false si no
    bool eliminar(int index)
    {
        NodoLista *aux = head;
        while (aux)
        {
            if (aux->index == index)
            {
                if (aux == head)
                {
                    head = aux->sig;
                    if (head != NULL)
                    {
                        head->ant = NULL;
                    }
                }
                else if (aux == tail)
                {
                    tail = aux->ant;
                    if (tail != NULL)
                    {
                        tail->sig = NULL;
                    }
                }
                else
                {
                    aux->ant->sig = aux->sig;
                    aux->sig->ant = aux->ant;
                }
                delete aux;
                largo--;
                return true;
            }
            aux = aux->sig;
        }
        return false;
    }

    // PRE:
    // POS: "actualiza" la posición de index en la lista; lo elimina y lo agrega al principio
    void actualizar(int index)
    {
        eliminar(index);
        insertar(index);
    }

    bool estaVacia()
    {
        return largo == 0;
    }

    int getLargo()
    {
        return this->largo;
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
    }

    NodoHash(string dominio, string path, string titulo, int tiempo) : dominio(dominio), path(path), titulo(titulo), tiempo(tiempo), libre(true), l(Lista()) {}
    Lista l;
    string dominio;
    string path;
    string titulo;
    int tiempo;
    bool libre;
};

class TablaHash
{
public:
    TablaHash(int largo)
    {
        this->largo = largo;
        cantidad = 0;
        fc = 0;
        tabla = new NodoHash *[largo];
        for (int i = 0; i < largo; i++)
            tabla[i] = NULL;
    }

    void clear()
    {
        for (int i = 0; i < largo; i++)
        {
            delete tabla[i];
            tabla[i] = NULL;
        }
    }

    int cantElementos()
    {
        return cantidad;
    }

    // si no funciona esto toca corchazo
    void recorrerDominio(string dominio)
    {
        int pos = miHash1(dominio) % largo;
        NodoLista *headDominio = tabla[pos]->l.head;
        while (headDominio)
        {
            string path = tabla[headDominio->index]->path;
            cout << path << " ";
            headDominio = headDominio->sig;
        }
    }

    void insertarDominio(string dominio, string path, string titulo, int tiempo)
    {
        fc = (float)cantidad / (float)largo;
        if (fc > 0.7)
            return;
        int intentos = 1;
        int pos = miHash1(dominio) % largo;
        NodoHash *nodo = tabla[pos];
        if (!nodo)
        {
            NodoHash *agrego = new NodoHash(dominio, path, titulo, tiempo);
            tabla[pos] = agrego;
            tabla[pos]->l.insertar(pos);
            cantidad++;
        }
        else
        {
            if (nodo->path == path)
            {
                nodo->titulo = titulo;
                nodo->tiempo = tiempo;
                nodo->l.actualizar(pos);
            }
            else
            {
                NodoLista *aux = nodo->l.head;
                bool encontrado = false;
                while (aux)
                {
                    if (tabla[aux->index]->path == path)
                    {
                        tabla[aux->index]->titulo = titulo;
                        tabla[aux->index]->tiempo = tiempo;
                        nodo->l.actualizar(aux->index);
                        encontrado = true;
                        break;
                    }
                    aux = aux->sig;
                }
                if (encontrado) return;
                // o sea, aquí recorrí toda la lista del dominio y no estaba así que debe ser nuevo
                pos = (miHash1(dominio) + intentos * miHash2(dominio)) % largo;
                while (tabla[pos])
                {
                    pos = (miHash1(dominio) + (++intentos) * miHash2(dominio)) % largo;
                }
                NodoHash *aAgregar = new NodoHash(dominio, path, titulo, tiempo);
                nodo->l.insertar(pos);
                tabla[pos] = aAgregar;
                cantidad++;
            }
        }
    }

    void insertarCompuesto(string dominio, string path, string titulo, int tiempo)
    {
        fc = (float)cantidad / (float)largo;
        if (fc > 0.7)
            return;
        int intentos = 1;
        int pos = miHash1(dominio + path) % largo;
        while (tabla[pos])
        {
            if (tabla[pos]->dominio == dominio && tabla[pos]->path == path)
            {
                tabla[pos]->tiempo = tiempo;
                tabla[pos]->titulo = titulo;
                return;
            }
            pos = (miHash1(dominio + path) + (++intentos) * miHash2(dominio + path)) % largo;
        }
        NodoHash *aAgregar = new NodoHash(dominio, path, titulo, tiempo);
        tabla[pos] = aAgregar;
        cantidad++;
    }

    NodoHash *buscar(string dominio, string path)
    {
        int intentos = 0;
        int pos = (miHash1(dominio + path)) % largo;
        while (tabla[pos] && intentos <= largo)
        {
            if (tabla[pos]->dominio == dominio && tabla[pos]->path == path)
                return tabla[pos];
            pos = (miHash1(dominio + path) + (++intentos) * miHash2(dominio + path)) % largo;
        }
        return nullptr;
    }

    int getLargoDominio(string dominio)
    {
        int intentos = 1;
        int pos = miHash1(dominio) % largo;
        while (tabla[pos] && intentos <= largo)
        {
            if (tabla[pos]->dominio == dominio) return tabla[pos]->l.getLargo();
            pos = (miHash1(dominio) + (++intentos) * miHash2(dominio)) % largo;
        }
        return 0;
    }

    void eliminar(string dominio, string path)
    {
    }

private:
    int cantidad;
    int largo;
    float fc;
    NodoHash **tabla;

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
    // Adaptado de: https://helloacm.com/the-simplest-string-hash-function-djb2-algorithm-and-implementations/ (djb2 algorithm por Dan Bernstein)
    unsigned int miHash2(string key)
    {
        unsigned int hash = 5381;
        for (char c : key)
        {
            hash = ((hash << 5) + hash) + c;
        }
        return hash;
    }
};

class Cache
{
public:
    int largo;
    TablaHash *dominio_path;
    TablaHash *dominio_hash;

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
        largo = n;
        dominio_path = new TablaHash(nextPrime(largo * 2));
        dominio_hash = new TablaHash(nextPrime(largo * 2));
    }
    void PUT(string dominio, string path, string titulo, int tiempo)
    {
        dominio_hash->insertarDominio(dominio, path, titulo, tiempo);
        dominio_path->insertarCompuesto(dominio, path, titulo, tiempo);
    }
    void GET(string dominio, string path)
    {
        NodoHash *busqueda = dominio_path->buscar(dominio, path);
        if (!busqueda)
        {
            cout << "Recurso_no_encontrado" << endl;
        }
        else
        {
            cout << busqueda->titulo << " " << busqueda->tiempo << endl;
        }
    }

    void REMOVE(string dominio, string path)
    {

    }

    void CONTAINS(string dominio, string path)
    {
        NodoHash* busqueda = dominio_path->buscar(dominio,path);
        if (!busqueda){
            cout << "false" << endl;
        } else {
            cout << "true" << endl;
        }
    }

    void COUNT_DOMAIN(string dominio)
    {
        cout << dominio_hash->getLargoDominio(dominio) << endl;
    }

    void LIST_DOMAIN(string dominio)
    {
        dominio_hash->recorrerDominio(dominio);
    }

    void CLEAR_DOMAIN(string dominio)
    {
        cout << "TODO_Porque_Soy_Vago" << endl;
    }

    void SIZE()
    {
        cout << dominio_path->cantElementos() << endl;
    }

    void CLEAR()
    {
        cout << "TODO_Porque_Soy_Vago" << endl;
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
        cout << "Comando_a_procesar:" << command << endl;
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
        cout << "Comando_procesado" << endl;
    }
    return 0;
}