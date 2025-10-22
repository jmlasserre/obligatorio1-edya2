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

class NodoPosicion
{
private:
    int arrayPosicion[2];

public:
    NodoPosicion(int ubicacionEnHash, int index)
    {
        arrayPosicion[0] = ubicacionEnHash;
        arrayPosicion[1] = index;
    }

    int getUbicacionEnHash()
    {
        return arrayPosicion[0];
    }

    void setUbicacion(int ubic) { arrayPosicion[0] = ubic; }

    void setIndex(int ind) { arrayPosicion[1] = ind; }

    int getIndexEnBucket()
    {
        return arrayPosicion[1];
    }

    // necesito overload del == para poder usar ListImp<NodoPosicion>
    bool operator==(const NodoPosicion &otroNodo)
    {
        return (this->arrayPosicion[0] == otroNodo.arrayPosicion[0] && this->arrayPosicion[1] == otroNodo.arrayPosicion[1]); // O(1)
    }
};

class NodoDominio
{
public:
    string dominio;
    ListImp<NodoPosicion *> indices;

    NodoDominio(string dom) : dominio(dom), indices(ListImp<NodoPosicion *>()) {}
    bool operator==(const NodoDominio &otroNodo)
    {
        return this->dominio == otroNodo.dominio;
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

    NodoPosicion *insertarEnPaths(string dominio, string path, string titulo, int tiempo)
    {
        float fc = (float)(cantidad + 1) / (float)largo;
        if (fc > 0.7)
            return nullptr;
        int pos = miHash1(dominio + path) % largo;
        if (!tabla[pos]) // nullptr
        {
            NodoRecurso *aAgregar = new NodoRecurso(dominio, path, titulo, tiempo);
            tabla[pos] = new NodoHash(dominio, path, titulo, tiempo);
            tabla[pos]->bucket.insert(aAgregar);
            cantidad++;
            NodoPosicion *posicion = new NodoPosicion(pos, 0);
            return posicion;
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
            NodoPosicion *posicion = new NodoPosicion(pos, 0);
            return posicion;
        }
        else
        { // nodo ocupado, puede ser update
            int largo = tabla[pos]->bucket.getSize();
            bool actualizo = false;
            int indexActualizo = -1;
            for (int i = 0; i < largo; i++)
            {
                NodoRecurso *rec = tabla[pos]->bucket.get(0);
                if (rec->dominio == dominio && rec->path == path)
                {
                    rec->tiempo = tiempo;
                    rec->titulo = titulo;
                    actualizo = true;
                    indexActualizo = i;
                }
                tabla[pos]->bucket.removeAt(0); // O(1)
                tabla[pos]->bucket.insert(rec); // O(1)
            }
            if (actualizo)
            {
                NodoPosicion *posicion = new NodoPosicion(pos, indexActualizo);
                return posicion; // no aumento cantidad porque actualicé un nodo
            }
            NodoRecurso *aAgregar = new NodoRecurso(dominio, path, titulo, tiempo);
            tabla[pos]->bucket.insert(aAgregar);
            cantidad++;
            NodoPosicion *posicion = new NodoPosicion(pos, largo);
            return posicion;
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

    void insertarDominio(string dominio, NodoPosicion *index)
    {
        float fc = (float)(cantidad + 1) / (float)largo;
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
                        NodoPosicion *currentIndex = nodoAct->indices.get(0);
                        if (index && currentIndex && index->getIndexEnBucket() == currentIndex->getIndexEnBucket() && index->getUbicacionEnHash() == currentIndex->getUbicacionEnHash())
                        {
                            actualizo = true;
                        }
                        nodoAct->indices.removeAt(0);
                        nodoAct->indices.insert(currentIndex);
                    }
                    // recorrí todos los índices del dominio y no encontré el que quiero insertar; debo insertarlo
                    if (actualizo)
                        return;
                    if (nodoAct->indices.getSize() == 0) nodoAct->indices.insert(index);
                    else nodoAct->indices.insertAt(0, index);
                    break;
                }
                tabla[pos]->indices.removeAt(0);
                tabla[pos]->indices.insert(nodoAct);
            }
            // recorrí todos los dominios de esa ubicación y ninguno era el que buscaba: debo agregarlo
            if (fc > 0.7)
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

    ListImp<NodoPosicion *> *getRecursosByDominio(string dominio)
    {
        int pos = miHash1(dominio) % largo;
        if (!tabla[pos] || tabla[pos]->libre)
            return nullptr;
        int largo = tabla[pos]->indices.getSize();
        for (int i = 0; i < largo; i++)
        {
            NodoDominio *dom = tabla[pos]->indices.get(0);
            if (dom->dominio == dominio)
                return &dom->indices;
            tabla[pos]->indices.removeAt(0);
            tabla[pos]->indices.insert(dom);
        }
        return nullptr;
    }

    void listarPaths(ListImp<NodoPosicion *> *&lista, string dominio)
    {
        int largoRecs = lista->getSize();
        for (int i = 0; i < largoRecs; i++)
        {
            NodoPosicion *index = lista->get(0);
            ListImp<NodoRecurso *> listaRecs = tabla[index->getUbicacionEnHash()]->bucket;
            int largoListaRecs = listaRecs.getSize();
            for (int j = 0; j < largoListaRecs; j++)
            {
                NodoRecurso *currentIndex = listaRecs.get(0);
                if (j == index->getIndexEnBucket())
                {
                    cout << listaRecs.get(0)->path << " ";
                    lista->removeAt(0);
                    lista->insert(index);
                }
                listaRecs.removeAt(0);
                listaRecs.insert(currentIndex);
            }
        }
        cout << endl;
    }

    NodoPosicion *eliminarEnPaths(string dominio, string path)
    {
        int pos = miHash1(dominio + path) % largo;
        if (!tabla[pos] || tabla[pos]->libre)
            return nullptr;
        int largo = tabla[pos]->bucket.getSize();
        for (int i = 0; i < largo; i++)
        {
            NodoRecurso *rec = tabla[pos]->bucket.get(0);
            if (rec->dominio == dominio && rec->path == path)
            {
                // eliminar el recurso del bucket y liberar memoria
                tabla[pos]->bucket.removeAt(0);
                delete rec;
                cantidad--;
                // si ya no hay recursos en el bucket, marcar la casilla como libre
                if (tabla[pos]->bucket.getSize() == 0)
                {
                    tabla[pos]->libre = true;
                    tabla[pos]->dominio = "";
                    tabla[pos]->path = "";
                    tabla[pos]->titulo = "";
                    tabla[pos]->tiempo = 0;
                }
                return new NodoPosicion(pos, i);
            }
            tabla[pos]->bucket.removeAt(0);
            tabla[pos]->bucket.insert(rec);
        }
        return nullptr;
    }

    void eliminarEnDominio(string dominio, string path, NodoPosicion *index)
    {
        int pos = miHash1(dominio) % largo;
        if (!tabla[pos] || tabla[pos]->libre)
            return;
        int largo = tabla[pos]->indices.getSize(); // esta es la cantidad de dominios en el bucket
        for (int i = 0; i < largo; i++)
        {
            NodoDominio *nodoAct = tabla[pos]->indices.get(0);
            if (nodoAct->dominio == dominio)
            {
                int largoDom = nodoAct->indices.getSize();
                bool borre = false;
                ListImp<NodoPosicion *> temp;
                for (int k = 0; k < largoDom; k++)
                {
                    NodoPosicion *currentIndex = nodoAct->indices.get(0);
                    nodoAct->indices.removeAt(0);
                    // si coincide con el índice a borrar, liberamos y marcamos borrado
                    if (index && currentIndex &&
                        index->getUbicacionEnHash() == currentIndex->getUbicacionEnHash() &&
                        index->getIndexEnBucket() == currentIndex->getIndexEnBucket())
                    {
                        delete currentIndex;
                        borre = true;
                        continue;
                    }
                    // si el recurso está en el mismo bucket y su índice es mayor al borrado, decrementar su índice
                    if (index && currentIndex &&
                        index->getUbicacionEnHash() == currentIndex->getUbicacionEnHash() &&
                        currentIndex->getIndexEnBucket() > index->getIndexEnBucket())
                    {
                        currentIndex->setIndex(currentIndex->getIndexEnBucket() - 1);
                    }
                    temp.insert(currentIndex); // conservar el resto
                }
                // volver a insertar los elementos en nodoAct en el mismo orden
                while (!temp.isEmpty())
                {
                    NodoPosicion *rp = temp.get(0);
                    temp.removeAt(0);
                    nodoAct->indices.insert(rp);
                }
                if (borre)
                {
                    if (nodoAct->indices.getSize() == 0)
                    {
                        tabla[pos]->indices.removeAt(0);
                        delete nodoAct;
                        cantidad--;
                    }
                    return;
                }
                else
                    return;
            }
            tabla[pos]->indices.removeAt(0);
            tabla[pos]->indices.insert(nodoAct);
        }
    }

    string getPathAt(int ubicacionEnHash, int indexEnBucket)
    {
        return tabla[ubicacionEnHash]->bucket.get(indexEnBucket)->path;
    }

    string getDominioAt(int pos)
    {
        if (!tabla[pos] || tabla[pos]->libre)
            return "";
        return tabla[pos]->dominio;
    }
};

// Declaración de la clase Cache (mismos atributos, métodos vacíos)
class Cache
{
private:
    TablaHash *dominio_path;
    TablaHash *dominios;

public:
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
        NodoPosicion *index = dominio_path->insertarEnPaths(dominio, path, titulo, tiempo);
        if (index)
            dominios->insertarDominio(dominio, index);
    }
    void GET(string dominio, string path)
    {
        dominio_path->buscar(dominio, path);
    }
    void REMOVE(string dominio, string path)
    {
        int cantActual = dominio_path->cantidadElementos();
        NodoPosicion *indexABorrar = dominio_path->eliminarEnPaths(dominio, path);
        if (indexABorrar != nullptr)
        {
            dominios->eliminarEnDominio(dominio, path, indexABorrar);
            delete indexABorrar;
        }
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
        ListImp<NodoPosicion *> *recs = dominios->getRecursosByDominio(dominio);
        if (recs)
            dominio_path->listarPaths(recs, dominio);
        else
            cout << endl;
    }
    void CLEAR_DOMAIN(string dominio)
    {
        ListImp<NodoPosicion *> *listaRecs = dominios->getRecursosByDominio(dominio);
        if (!listaRecs || listaRecs->getSize() == 0)
            return;
        while (!listaRecs->isEmpty())
        {
            NodoPosicion *pos = listaRecs->get(0);
            listaRecs->removeAt(0);
            string path = dominio_path->getPathAt(pos->getUbicacionEnHash(), pos->getIndexEnBucket());
            NodoPosicion *nodoPos = dominio_path->eliminarEnPaths(dominio, path);
            dominios->eliminarEnDominio(dominio, path, pos);
        }
    }
    void SIZE()
    {
        cout << dominio_path->cantidadElementos() << endl;
    }
    void CLEAR()
    {
        int largo = dominios->cantidadElementos();
        for (int i = 0; i < largo; i++)
        {
            string dom = dominios->getDominioAt(i);
            if (dom != "")
            {
                CLEAR_DOMAIN(dom);
            }
        }
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
        //cout << "Ejecutando comando " << command << ", num " << i + 2 << ": ";
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
        //cout << endl;
    }
    return 0;
}