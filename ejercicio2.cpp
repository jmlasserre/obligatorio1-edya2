/*
    Facundo García - 328757
    Juan Martín Lasserre - 289139
    Universidad ORT Uruguay - M4A - 2025
*/

#include <cassert>
#include <string>
#include <iostream>
#include <limits>

using namespace std;

class NodoHash
{
public:
    NodoHash()
    {
        dominio = path = titulo = "";
        tiempo = reps = 0;
        libre = true;
    }
    string dominio;
    string path;
    string titulo;
    int tiempo;
    bool libre;
    int reps;
};

class TablaHash
{
public:
    TablaHash(int largo)
    {
        largo = largo;
        cantidad = 0;
        tabla = new NodoHash *[largo];
        for (int i = 0; i < largo; i++)
        {
            tabla[i] = new NodoHash();
        }
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
    void agregar(string dominio, string path, string titulo, int tiempo, string key, bool tieneReps)
    {
        int pos = miHash1(key);
        insertarNoReps(dominio, path, titulo, tiempo, pos, 1);
    }

    NodoHash *buscar(string dominio, string path, string key) {
        int hash1 = miHash1(key);
        int hash2 = miHash2(key);
        int intento = 0;

        while (intento < largo) {
            int pos = (hash1 + intento * hash2) % largo;

            if (tabla[pos]->libre) {
                return nullptr;
            }

            if (tabla[pos]->dominio == dominio && tabla[pos]->path == path) {
                return tabla[pos];
            }
            intento++;
        }
        return nullptr;
    }

    void eliminar(string dominio, string path, string key) {
        
        int hash1 = miHash1(key);
        int hash2 = miHash2(key);
        int intento = 0;

        while(intento < largo) {
            int pos = (hash1 + intento * hash2) % largo;

            if(tabla[pos]->dominio == dominio && tabla[pos]->path == path) {
                
                tabla[pos]->dominio = "";
                tabla[pos]->path = "";
                tabla[pos]->titulo = "";
                tabla[pos]->reps = 0;
                tabla[pos]->tiempo = 0;
                tabla[pos]->libre = true;
                cantidad--;
            }
            intento++;
        }
    }

private:
    int cantidad;
    int largo;
    int fc;
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
        return hash_value % largo;
    }
    // Adaptado de: https://helloacm.com/the-simplest-string-hash-function-djb2-algorithm-and-implementations/ (djb2 algorithm por Dan Bernstein)
    unsigned int miHash2(string key)
    {
        unsigned int hash = 5381;
        for (char c : key)
        {
            hash = ((hash << 5) + hash) + c;
        }
        return hash % largo;
    }

    void insertarNoReps(string dominio, string path, string titulo, int tiempo, int pos, int intento)
    {
        if (tabla[pos]->libre)
        {
            NodoHash *&nodo = tabla[pos];
            nodo->dominio = dominio;
            nodo->path = path;
            nodo->titulo = titulo;
            nodo->tiempo = tiempo;
            nodo->libre = false;
        }
        else
        {
            insertarNoReps(dominio, path, titulo, tiempo, ++intento * miHash2(dominio), intento);
        }
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

    Cache(int largo)
    {
        dominio_path = new TablaHash(nextPrime(largo * 2));
        dominio_hash = new TablaHash(nextPrime(largo * 2));
    }
    void PUT(string dominio, string path, string titulo, int tiempo)
    {
        dominio_path->agregar(dominio, path, titulo, tiempo, (dominio + path), false);
        dominio_hash->agregar(dominio, path, titulo, tiempo, dominio, true);
    }
    void GET(string dominio, string path)
    {
        NodoHash* resultado = dominio_path->buscar(dominio, path, (dominio + path));
        if(resultado != nullptr){
            cout << resultado->titulo << "" << resultado->tiempo;
        }
        else {
            cout << "recurso_no_encontrado" << endl;
        }
    }

    void REMOVE(string dominio, string path)
    {
        dominio_path->eliminar(dominio, path, (dominio + path));
        dominio_hash->eliminar(dominio, path, dominio);
        
    }
    void CONTAINS(string dominio, string path)
    {
        NodoHash* resultado = dominio_path->buscar(dominio, path, (dominio + path));

        if(resultado != nullptr) {
            cout << true << endl;
        }
        else {
            cout << false << endl;
        }
    }
    void COUNT_DOMAIN(string dominio)
    {
        
    }
    void LIST_DOMAIN(string dominio)
    {
        // TODO: LIST_DOMAIN
    }
    void CLEAR_DOMAIN(string dominio)
    {
        // TODO: CLEAR_DOMAIN
    }
    void SIZE()
    {
        cout << dominio_path->cantElementos() << endl;
    }
    void CLEAR()
    {
        dominio_path->clear();
        dominio_hash->clear();
    }
};

int main()
{
    int N;
    string command;
    cin >> N;
    for (int i = 0; i < N; i++)
    {
        cin >> command;
        if (command == "PUT")
        {
            string dominio, path, titulo;
            int tiempo;
        }
    }
    return 0;
}