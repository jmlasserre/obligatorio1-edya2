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

struct NodoHash
{
    ListImp<int> indices;
    std::string dominio;
    std::string path;
    std::string titulo;
    int tiempo;
    bool libre;

    NodoHash() : indices(), dominio(""), path(""), titulo(""), tiempo(0), libre(true) {}

    NodoHash(string dominio, string path, string titulo, int tiempo) : dominio(dominio), path(path), titulo(titulo), tiempo(tiempo) {}
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

    // Adaptado de: https://helloacm.com/the-simplest-string-hash-function-djb2-algorithm-and-implementations/ (Algoritmo djb2, por Dan Bernstein)
    unsigned int miHash2(string key)
    {
        unsigned int hash = 5381;
        for (char c : key)
        {
            hash = ((hash << 5) + hash) + c;
        }
        return hash;
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

    /*void clear()
    {
        for (int i = 0; i < largo; i++)
        {
            delete tabla[i];
            tabla[i] = NULL;
        }
    }*/

    int cantElementos()
    {
        return cantidad;
    }

    int getLargo()
    {
        return largo;
    }

    void mostrarPaths(ListImp<int> *&lista)
    {
        int k = lista->getSize();
        for (int i = 0; i < k; i++)
        {
            int index = lista->get(0); // O(1)
            lista->removeAt(0);        // O(1)
            lista->insert(index);      // O(1)
            cout << tabla[index]->path << " ";
        }
        cout << endl;
    }

    void insertarDominio(string dominio, int index)
    {
        float fc = (float)(cantidad + 1) / (float)largo;
        if (fc > 0.7 || index == -1)
            return;
        int intentos = 0;
        int pos = miHash1(dominio) % largo;
        while (tabla[pos] && intentos <= largo)
        {
            if (!tabla[pos]->libre)
            {
                if (tabla[pos]->dominio == dominio)
                {
                    tabla[pos]->indices.remove(index); // O(k)
                    if (tabla[pos]->indices.getSize() == 0)
                    {
                        tabla[pos]->indices.insert(index);
                    }
                    else
                    {
                        tabla[pos]->indices.insertAt(0, index); // O(1)
                    }
                    return;
                }
            }
            pos = (miHash1(dominio) + (++intentos) * ((miHash2(dominio) % (largo - 1)) + 1)) % largo;
        }
        if (!tabla[pos])
        {
            tabla[pos] = new NodoHash(dominio, "", "", 0);
            tabla[pos]->indices.insert(index);
            cantidad++;
        }
        else if (tabla[pos]->libre)
        {
            tabla[pos]->dominio = dominio;
            tabla[pos]->libre = false;
            tabla[pos]->indices.insert(index);
            cantidad++;
        }
    }

    ListImp<int> *getDominios(string dominio)
    {
        int pos = miHash1(dominio) % largo;
        int intentos = 0;
        while (tabla[pos] && intentos <= largo)
        {
            if (!tabla[pos]->libre)
            {
                if (tabla[pos]->dominio == dominio)
                    return &tabla[pos]->indices;
            }
            pos = (miHash1(dominio) + (++intentos) * ((miHash2(dominio) % (largo - 1)) + 1)) % largo;
        }
        return nullptr;
    }

    int insertarEnPaths(string dominio, string path, string titulo, int tiempo)
    {
        float fc = (float)(cantidad + 1) / (float)largo;
        int intentos = 0;
        int pos = miHash1(dominio + path) % largo;
        while (tabla[pos] && intentos <= largo)
        {
            if (!tabla[pos]->libre)
            {
                if (tabla[pos]->dominio == dominio && tabla[pos]->path == path)
                {
                    tabla[pos]->titulo = titulo;
                    tabla[pos]->tiempo = tiempo;
                    return pos;
                }
            }
            pos = (miHash1(dominio + path) + (++intentos) * ((miHash2(dominio + path) % (largo - 1)) + 1)) % largo;
        }
        if (fc <= 0.7)
        {
            if (!tabla[pos])
            {
                tabla[pos] = new NodoHash(dominio, path, titulo, tiempo);
                cantidad++;
                return pos;
            }
            else if (tabla[pos]->libre)
            {
                tabla[pos]->dominio = dominio;
                tabla[pos]->path = path;
                tabla[pos]->titulo = titulo;
                tabla[pos]->tiempo = tiempo;
                tabla[pos]->libre = false;
                cantidad++;
                return pos;
            }
        }
        else
            return -1;
    }

    string getPathAt(int index)
    {
        if (tabla[index] && !tabla[index]->libre)
            return tabla[index]->path;
        return "";
    }

    string getDominioAt(int index)
    {
        if (tabla[index] && !tabla[index]->libre)
            return tabla[index]->dominio;
        return "";
    }

    NodoHash *getNodo(string dominio, string path)
    {
        int intentos = 0;
        int pos = miHash1(dominio + path) % largo;
        while (tabla[pos] && intentos <= largo)
        {
            if (!tabla[pos]->libre)
            {
                if (tabla[pos]->dominio == dominio && tabla[pos]->path == path)
                {
                    return tabla[pos];
                }
            }
            pos = (miHash1(dominio + path) + (++intentos) * ((miHash2(dominio + path) % (largo - 1)) + 1)) % largo;
        }
        return nullptr;
    }

    int eliminarEnPaths(string dominio, string path)
    {
        int intentos = 0;
        int pos = miHash1(dominio + path) % largo;
        while (tabla[pos] && intentos <= largo)
        {
            if (!tabla[pos]->libre)
            {
                if (tabla[pos]->dominio == dominio && tabla[pos]->path == path)
                {
                    tabla[pos]->dominio = tabla[pos]->path = tabla[pos]->titulo = "";
                    tabla[pos]->tiempo = 0;
                    tabla[pos]->libre = true;
                    cantidad--;
                    return pos;
                }
            }
            pos = (miHash1(dominio + path) + (++intentos) * ((miHash2(dominio + path) % (largo - 1)) + 1)) % largo;
        }
        return -1;
    }

    void eliminarEnDominios(string dominio, int index)
    {
        int intentos = 0;
        int pos = miHash1(dominio) % largo;
        while (tabla[pos] && intentos <= largo)
        {
            if (!tabla[pos]->libre)
            {
                if (tabla[pos]->dominio == dominio)
                {
                    tabla[pos]->indices.remove(index);
                    if (tabla[pos]->indices.getSize() == 0)
                    {
                        tabla[pos]->dominio = "";
                        tabla[pos]->libre = true;
                        cantidad--;
                    }
                    return;
                }
            }
            pos = (miHash1(dominio) + (++intentos) * ((miHash2(dominio) % (largo - 1)) + 1)) % largo;
        }
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
        dominio_path = new TablaHash(nextPrime(n * 2));
        dominios = new TablaHash(nextPrime(n * 2));
    }

    void PUT(string dominio, string path, string titulo, int tiempo)
    {
        int pos = dominio_path->insertarEnPaths(dominio, path, titulo, tiempo); // O(1) prom
        dominios->insertarDominio(dominio, pos);                                // O(k) prom porque recorro todo el dominio
    }

    void GET(string dominio, string path)
    {
        NodoHash *nodo = dominio_path->getNodo(dominio, path); // O(1) prom
        if (!nodo)
        {
            cout << "recurso_no_encontrado" << endl;
        }
        else
        {
            cout << nodo->titulo << " " << nodo->tiempo << endl;
        }
    }

    void REMOVE(string dominio, string path)
    {
        int cantActual = dominio_path->cantElementos();
        int indexABorrar = dominio_path->eliminarEnPaths(dominio, path);
        if (indexABorrar != -1)
        {
            dominios->eliminarEnDominios(dominio, indexABorrar);
        }
    }

    void CONTAINS(string dominio, string path)
    {
        NodoHash *nodo = dominio_path->getNodo(dominio, path);
        if (!nodo)
        {
            cout << "false" << endl;
        }
        else
        {
            cout << "true" << endl;
        }
    }

    void COUNT_DOMAIN(string dominio)
    {
        ListImp<int> *listaDeDominios = dominios->getDominios(dominio); // O(1) prom
        if (!listaDeDominios)
        {
            cout << 0 << endl;
        }
        else
        {
            cout << listaDeDominios->getSize() << endl;
        }
    }

    void LIST_DOMAIN(string dominio)
    {
        ListImp<int> *listaDeDominios = dominios->getDominios(dominio);
        if (!listaDeDominios)
        {
            cout << " " << endl;
        }
        else
        {
            dominio_path->mostrarPaths(listaDeDominios);
        }
    }

    void CLEAR_DOMAIN(string dominio)
    {
        ListImp<int> *listaDeRecursos = dominios->getDominios(dominio);
        if (!listaDeRecursos)
            return;
        int largo = listaDeRecursos->getSize();
        for (int i = 0; i < largo; i++)
        {
            int index = listaDeRecursos->get(0);
            listaDeRecursos->removeAt(0);
            dominio_path->eliminarEnPaths(dominio, dominio_path->getPathAt(index));
            dominios->eliminarEnDominios(dominio, index);
        }
    }

    void SIZE()
    {
        cout << dominio_path->cantElementos() << endl; // O(1) pc
    }

    void CLEAR()
    {
        int largo = dominios->getLargo();
        for (int i = 0; i < largo; i++)
        {
            if (dominios->getDominioAt(i) != "")
            {
                string dominioABorrar = dominios->getDominioAt(i);
                CLEAR_DOMAIN(dominioABorrar);
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