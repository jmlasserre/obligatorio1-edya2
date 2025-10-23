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

class NodoHash
{
private:
    string dominio;
    string path;
    string titulo;
    int tiempo;
    bool libre;

public:
    NodoHash()
    {
        dominio = path = titulo = "";
        tiempo = 0;
        libre = true;
    }

    NodoHash(string dominio, string path, string titulo, int tiempo) : dominio(dominio), path(path), titulo(titulo), tiempo(tiempo), libre(false) {}

    string getDominio() { return this->dominio; }
    string getPath() { return this->path; }
    string getTitulo() { return this->titulo; }
    int getTiempo() { return this->tiempo; }
    void actualizar(string titulo, int tiempo)
    {
        this->titulo = titulo;
        this->tiempo = tiempo;
    }

    bool operator==(const NodoHash &otroNodo)
    {
        return this->dominio == otroNodo.dominio && this->path == otroNodo.path && this->titulo == otroNodo.titulo && this->tiempo == otroNodo.tiempo;
    }
};

class NodoDominio
{
private:
    ListImp<NodoHash> recursos;
    string nombre;

public:
    NodoDominio() : recursos(ListImp<NodoHash>()), nombre("") {}

    NodoDominio(string nombre) : recursos(ListImp<NodoHash>()), nombre(nombre) {}

    string getNombre() { return this->nombre; }

    int getCant() { return recursos.getSize(); }

    void insertarRecurso(string dominio, string path, string titulo, int tiempo)
    {
        int bucketSize = recursos.getSize();
        ListImp<NodoHash> *posibleAct = new ListImp<NodoHash>();
        bool actualizo = false;
        NodoHash actualizado;
        for (int i = 0; i < bucketSize; i++)
        {
            NodoHash nodo = recursos.get(0);
            recursos.removeAt(0);
            if (!actualizo && nodo.getDominio() == dominio && nodo.getPath() == path)
            {
                nodo.actualizar(titulo, tiempo);
                actualizado = nodo;
                actualizo = true;
            }
            else
                recursos.insert(nodo);
        }
        if (actualizo)
        {
            if (posibleAct->getSize() == 0)
                recursos.insert(actualizado);
            else
                recursos.insertAt(0, actualizado);
        }
        else
        {
            if (recursos.getSize() == 0)
                recursos.insert(NodoHash(dominio, path, titulo, tiempo));
            else
                recursos.insertAt(0, NodoHash(dominio, path, titulo, tiempo));
        }
    }

    bool operator==(const NodoDominio &otroNodo)
    {
        return this->nombre == otroNodo.nombre;
    }

    void listarRecursos()
    {
        int size = recursos.getSize();
        for (int i = 0; i < size; i++)
        {
            NodoHash rec = recursos.get(0);
            cout << rec.getPath() << " ";
            recursos.removeAt(0);
            recursos.insert(rec);
        }
        cout << endl;
    }

    ListImp<string> *obtenerPaths()
    {
        int bucketSize = recursos.getSize();
        if (bucketSize == 0)
            return nullptr;
        ListImp<string> *listaPaths = new ListImp<string>();
        for (int i = 0; i < bucketSize; i++)
        {
            NodoHash rec = recursos.get(0);
            listaPaths->insert(rec.getPath());
            recursos.removeAt(0);
            recursos.insert(rec);
        }
        return listaPaths;
    }

    void eliminarRecurso(string path)
    {
        int bucketSize = recursos.getSize();
        ListImp<NodoHash> *nueva = new ListImp<NodoHash>();
        for (int i = 0; i < bucketSize; i++)
        {
            NodoHash nodo = recursos.get(0);
            recursos.removeAt(0);
            if (nodo.getPath() == path)
            {
                continue;
            }
            nueva->insert(nodo);
        }
        while (nueva->getSize() > 0)
        {
            NodoHash nodo = nueva->get(0);
            recursos.insert(nodo);
            nueva->removeAt(0);
        }
    }
};

class TablaHash
{
private:
    ListImp<NodoHash> **tabla;
    ListImp<NodoDominio *> **tablaDominios;
    int cantidad;
    int largo;
    float fc;

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
    TablaHash(int largo, bool dominios)
    {
        this->largo = largo;
        cantidad = 0;
        fc = 0;
        if (!dominios)
        {
            tablaDominios = nullptr;
            tabla = new ListImp<NodoHash> *[largo];
            for (int i = 0; i < largo; i++)
                tabla[i] = new ListImp<NodoHash>();
        }
        else
        {
            tabla = nullptr;
            tablaDominios = new ListImp<NodoDominio *> *[largo];
            for (int i = 0; i < largo; i++)
            {
                tablaDominios[i] = new ListImp<NodoDominio *>();
            }
        }
    }

    int getLargo() { return this->largo; }

    int cantElementos()
    {
        return cantidad;
    }

    void actualizarFc()
    {
        fc = (float)cantidad / (float)largo;
    }

    // para uso en dominio_path
    bool insertarEnPaths(string dominio, string path, string titulo, int tiempo)
    {
        if (fc > 0.7)
            return false;
        int pos = miHash1(dominio + path) % largo;
        ListImp<NodoHash> *bucket = tabla[pos];
        int bucketSize = bucket->getSize();
        for (int i = 0; i < bucketSize; i++)
        {
            NodoHash nodo = bucket->get(0);
            bucket->removeAt(0);
            if (nodo.getDominio() == dominio && nodo.getPath() == path)
            {
                nodo.actualizar(titulo, tiempo);
                if (bucket->getSize() == 0)
                    bucket->insert(nodo);
                else
                    bucket->insertAt(0, nodo);
                return true; // actualicé
            }
            bucket->insert(nodo);
        }
        if (fc <= 0.7)
        {
            if (bucketSize == 0)
                bucket->insert(NodoHash(dominio, path, titulo, tiempo));
            else
                bucket->insertAt(0, NodoHash(dominio, path, titulo, tiempo));
            cantidad++;
            actualizarFc();
            return true;
        }
        return false;
    }

    // para uso en dominios
    void insertarEnDominios(string dominio, string path, string titulo, int tiempo)
    {
        int pos = miHash1(dominio) % largo;
        ListImp<NodoDominio *> *bucket = tablaDominios[pos];
        int bucketSize = bucket->getSize();
        for (int i = 0; i < bucketSize; i++)
        {
            NodoDominio *nodo = bucket->get(0);
            bucket->removeAt(0);
            if (nodo->getNombre() == dominio)
            {
                // actualizar nodo, no la copia, y volver a actualizarlo! Por esto se rompía
                nodo->insertarRecurso(dominio, path, titulo, tiempo);
                if (bucket->getSize() == 0)
                    bucket->insert(nodo);
                else
                    bucket->insertAt(0, nodo);
                return;
            }
            bucket->insert(nodo);
        }
        if (fc <= 0.7)
        {
            NodoDominio *nuevo = new NodoDominio(dominio);
            nuevo->insertarRecurso(dominio, path, titulo, tiempo);
            if (bucketSize == 0)
                bucket->insert(nuevo);
            else
                bucket->insertAt(0, nuevo);
            cantidad++;
            actualizarFc();
        }
    }

    bool get(string dominio, string path, bool print)
    {
        int pos = miHash1(dominio + path) % largo;
        ListImp<NodoHash> *bucket = tabla[pos];
        int bucketSize = bucket->getSize();
        for (int i = 0; i < bucketSize; i++)
        {
            NodoHash nodo = bucket->get(0);
            if (nodo.getDominio() == dominio && nodo.getPath() == path)
            {
                if (print)
                    cout << nodo.getTitulo() << " " << nodo.getTiempo() << endl;
                return true;
            }
            bucket->removeAt(0);
            bucket->insert(nodo);
        }
        if (print)
            cout << "recurso_no_encontrado" << endl;
        return false;
    }

    // para dominios
    int cantDominio(string dominio)
    {
        int pos = miHash1(dominio) % largo;
        ListImp<NodoDominio *> *bucket = tablaDominios[pos];
        int bucketSize = bucket->getSize();
        for (int i = 0; i < bucketSize; i++)
        {
            NodoDominio *dom = bucket->get(0);
            if (dom->getNombre() == dominio)
                return dom->getCant();
            bucket->removeAt(0);
            bucket->insert(dom);
        }
        return 0;
    }

    void listarRecursos(string dominio)
    {
        int pos = miHash1(dominio) % largo;
        ListImp<NodoDominio *> *bucket = tablaDominios[pos];
        int bucketSize = bucket->getSize();
        for (int i = 0; i < bucketSize; i++)
        {
            NodoDominio *dom = bucket->get(0);
            if (dom->getNombre() == dominio)
            {
                dom->listarRecursos();
                return;
            }
            bucket->removeAt(0);
            bucket->insert(dom);
        }
        cout << endl;
    }

    // para uso en dominio_path
    bool eliminarEnPaths(string dominio, string path) // mirar no se si esta bien.
    {
        int pos = miHash1(dominio + path) % largo;
        ListImp<NodoHash> *bucket = tabla[pos];
        int bucketSize = bucket->getSize();

        for (int i = 0; i < bucketSize; i++)
        {
            NodoHash nodo = bucket->get(0);
            if (nodo.getDominio() == dominio && nodo.getPath() == path)
            {
                bucket->removeAt(0);
                cantidad--;
                actualizarFc();
                return true;
            }
            bucket->removeAt(0);
            bucket->insert(nodo);
        }
        return false;
    }

    ListImp<string> *obtenerPathsDeDominio(string dominio)
    {
        int pos = miHash1(dominio) % largo;
        ListImp<NodoDominio *> *bucket = tablaDominios[pos];
        int bucketSize = bucket->getSize();

        for (int i = 0; i < bucketSize; i++)
        {
            NodoDominio *dom = bucket->get(0);
            bucket->removeAt(0);
            if (dom->getNombre() == dominio)
            {
                ListImp<string> *paths = dom->obtenerPaths();
                bucket->insert(dom);
                return paths; // retorna la lista de paths de ese dominio.
            }
            bucket->insert(dom);
        }
        return nullptr; // Si no tiene nada retorna lista vacia
    }

    bool eliminarEnDominios(string dominio)
    {
        int pos = miHash1(dominio) % largo;
        ListImp<NodoDominio *> *bucket = tablaDominios[pos];
        int bucketSize = bucket->getSize();
        bool eliminoAlguno = false;

        for (int i = 0; i < bucketSize; i++)
        {
            NodoDominio *dom = bucket->get(0);
            if (dom->getNombre() == dominio)
            {
                bucket->removeAt(0);
                delete dom;
                eliminoAlguno = true;
                cantidad--;
            }
            else
            {
                bucket->removeAt(0);
                bucket->insert(dom);
            }
        }
        if (eliminoAlguno)
            actualizarFc();
        return eliminoAlguno;
    }

    // NO CONFUNDIR CON eliminarEnDominios(), que elimina un dominio entero!!!
    void eliminarRecursoEnDominios(string dominio, string path)
    {
        int pos = miHash1(dominio) % largo;
        ListImp<NodoDominio *> *bucket = tablaDominios[pos];
        int bucketSize = bucket->getSize();
        for (int i = 0; i < bucketSize; i++)
        {
            NodoDominio *dom = bucket->get(0);
            bucket->removeAt(0);
            if (dom->getNombre() == dominio)
            {
                dom->eliminarRecurso(path);
                if (dom->getCant() == 0)
                {
                    // eliminar dominio vacío
                    delete dom;
                    cantidad--;
                    actualizarFc();
                }
                else
                {
                    // volver a insertar el dominio actualizado
                    if (bucket->getSize() == 0)
                        bucket->insert(dom);
                    else
                        bucket->insertAt(0, dom);
                }
                return;
            }
            bucket->insert(dom);
        }
    }

    ListImp<NodoDominio *> *getDominiosAt(int i)
    {
        return tablaDominios[i];
    }
};

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
        dominio_path = new TablaHash(largo, false);
        dominios = new TablaHash(largo, true);
    }

    void PUT(string dominio, string path, string titulo, int tiempo)
    {
        bool ins = dominio_path->insertarEnPaths(dominio, path, titulo, tiempo);
        if (ins)
            dominios->insertarEnDominios(dominio, path, titulo, tiempo);
    }
    void GET(string dominio, string path)
    {
        dominio_path->get(dominio, path, true);
    }
    void REMOVE(string dominio, string path)
    {
        bool eliminar = dominio_path->eliminarEnPaths(dominio, path);
        if (eliminar)
            dominios->eliminarRecursoEnDominios(dominio, path);
    }
    void CONTAINS(string dominio, string path)
    {
        bool contains = dominio_path->get(dominio, path, false);
        if (contains)
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
        dominios->listarRecursos(dominio);
    }
    void CLEAR_DOMAIN(string dominio, bool clear)
    {
        ListImp<string> *paths = dominios->obtenerPathsDeDominio(dominio);
        if (!paths)
            return;
        while (paths->getSize() > 0)
        {
            string path = paths->get(0);
            // cout << path << " ";
            dominio_path->eliminarEnPaths(dominio, path);
            paths->removeAt(0);
        }
        if (!clear) dominios->eliminarEnDominios(dominio);
    }
    void SIZE()
    {
        cout << dominio_path->cantElementos() << endl;
    }
    void CLEAR()
    {
        // cout << "Cantidad antes de clear: " << dominio_path->cantElementos() << endl;
        int M = dominios->getLargo();
        for (int i = 0; i < M; i++)
        {
            ListImp<NodoDominio *> *listaDominios = dominios->getDominiosAt(i);
            while (listaDominios->getSize() > 0)
            {
                NodoDominio *dom = listaDominios->get(0);
                CLEAR_DOMAIN(dom->getNombre(), true);
                dominios->eliminarEnDominios(dom->getNombre());
            }
        }
        // cout << "Limpié todo: hay " << dominio_path->cantElementos() << "elementos!";
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
            cache->CLEAR_DOMAIN(dominio, false);
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