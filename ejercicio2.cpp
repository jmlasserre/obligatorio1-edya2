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
    // POS: a ver si funciona con esto
    void eliminar(NodoLista *&nodo)
    {
        if (!nodo)
            return;
        if (nodo == head)
            head = nodo->sig;
        if (nodo == tail)
            tail = nodo->ant;
        if (nodo->ant)
            nodo->ant->sig = nodo->sig;
        if (nodo->sig)
            nodo->sig->ant = nodo->ant;

        delete nodo;
        nodo = nullptr;
        largo--;
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

    ~Lista()
    {
        if (!head)
            return; // O(1) para listas vacías así que no afecta REMOVE
        NodoLista *aBorrar = head;
        while (aBorrar->sig)
        {
            NodoLista *borro = aBorrar;
            aBorrar = aBorrar->sig;
            delete borro;
        }
        delete aBorrar;
        head = tail = nullptr;
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

    NodoHash(string dominio, string path, string titulo, int tiempo) : dominio(dominio), path(path), titulo(titulo), tiempo(tiempo), libre(false), l(Lista()) {}
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
    NodoHash **tabla;
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

    void recorrerDominio(string dominio)
    {
        int intentos = 1;
        int pos = miHash1(dominio) % largo;
        while (intentos <= largo && tabla[pos])
        {
            if (tabla[pos]->dominio == dominio)
            {
                NodoLista *p = tabla[pos]->l.head;
                while (p)
                {
                    int idx = p->index;
                    if (tabla[idx] && !tabla[idx]->libre)      // filtrar tombstones
                        cout << tabla[idx]->path << " ";
                    p = p->sig;
                }
                cout << endl;
                return;
            }
            pos = (miHash1(dominio) + (++intentos) * miHash2(dominio)) % largo;
        }
        cout << endl; // dominio inexistente => línea vacía
    }

    void insertarTablaDominio(string dominio, string path, string titulo, int tiempo)
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
                if (encontrado)
                    return;
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
            if (tabla[pos]->dominio == dominio)
                return tabla[pos]->l.getLargo();
            pos = (miHash1(dominio) + (++intentos) * miHash2(dominio)) % largo;
        }
        return 0;
    }

    void eliminarTablaDominio(string dominio, string path)
    {
        int intentos = 1;
        int pos = miHash1(dominio) % largo;
        while (tabla[pos] && intentos <= largo)
        {
            if (tabla[pos]->dominio == dominio)
            {
                NodoLista *listaPaths = tabla[pos]->l.head;
                NodoLista *aBorrar = nullptr;
                NodoLista *cand = nullptr;
                /* La idea es poder siempre borrar el nodo, incluso cuando es el home.
                Entonces hay que ir llevando registro de si tenemos un candidato para reemplazarlo si llega a pasar.
                Por las dudas, el "home" es el nodo de la tabla dominio_hash con key = dominio.*/
                while (listaPaths)
                {
                    int idx = listaPaths->index;
                    if (tabla[idx] && tabla[idx]->path == path)
                    {
                        aBorrar = listaPaths; // este sería el caso que te digo si se da al principio
                    }
                    if (!cand && idx != pos)
                    { // si no hay candidato y no estoy parado en el inicio...
                        cand = listaPaths;
                    }
                    listaPaths = listaPaths->sig;
                }
                if (!aBorrar)
                    return; // no existía ese path
                int idx = aBorrar->index;
                if (idx != pos)
                {
                    tabla[pos]->l.eliminar(aBorrar);
                    tombstone(idx);
                    cantidad--;
                    return;
                }
                if (!cand)
                { // existía, pero era el único
                    tabla[pos]->l.eliminar(aBorrar);
                    tombstone(pos);
                    cantidad--;
                    return;
                }
                // existe y hay más: hay que mover la lista
                int indexCand = cand->index;
                tabla[pos]->path = tabla[indexCand]->path;
                tabla[pos]->titulo = tabla[indexCand]->titulo;
                tabla[pos]->tiempo = tabla[indexCand]->tiempo;

                cand->index = pos;
                tabla[pos]->l.eliminar(aBorrar);
                tombstone(indexCand);
                cantidad--;
                return;
                // borramos el que estaba ahí y movemos los datos del candidato al home.
                // de esta forma no hace falta modificar la lista.
            }
            pos = (miHash1(dominio) + (intentos++) * miHash2(dominio)) % largo;
        }
    }

    void eliminarCompuesto(string dominio, string path)
    {
        int intentos = 1;
        int pos = miHash1(dominio + path) % largo;
        while (tabla[pos] && intentos <= largo) // si tabla[pos] es nullptr, no estaba en la tabla
        {
            if (tabla[pos]->dominio == dominio && tabla[pos]->path == path)
            {
                tombstone(pos);
                cantidad--;
                return;
            }
            pos = (miHash1(dominio + path) + (++intentos) * miHash2(dominio + path)) % largo;
        }
    }

    NodoLista* getRecursosAEliminar(string dominio){
        int intentos = 1;
        int pos = miHash1(dominio)%largo;
        while (tabla[pos] && intentos <= largo){
            if (tabla[pos]->dominio == dominio) return tabla[pos]->l.head;
            pos = (miHash1(dominio)+(++intentos)*miHash2(dominio))%largo;
        }
        return nullptr; 
    }

    void eliminarDominio(string dominio)
    {
        int intentos = 1;
        int pos = miHash1(dominio) % largo;
        while (tabla[pos] && intentos <= largo)
        {
            if (!tabla[pos]->libre && tabla[pos]->dominio == dominio)
            {
                // Mientras haya recursos del dominio, borrarlos cuidando el anchor
                while (tabla[pos]->l.head)
                {
                    NodoLista* node = tabla[pos]->l.head;     // siempre borramos el head actual
                    int idx = node->index;

                    if (idx != pos)
                    {
                        // Caso simple: no es el anchor
                        tombstone(idx);
                        tabla[pos]->l.eliminar(node);          // O(1): actualiza head/tail/largo
                        cantidad--;
                    }
                    else
                    {
                        // Estamos borrando el anchor
                        NodoLista* next = node->sig;           // siguiente en la lista del dominio
                        if (!next)
                        {
                            // Era el único recurso del dominio
                            tombstone(pos);
                            tabla[pos]->l.eliminar(node);      // lista queda vacía
                            cantidad--;
                            break;
                        }
                        // Promover el siguiente al anchor
                        int nextIdx = next->index;
                        // Copiamos payload del siguiente al anchor (dominio es el mismo)
                        tabla[pos]->path   = tabla[nextIdx]->path;
                        tabla[pos]->titulo = tabla[nextIdx]->titulo;
                        tabla[pos]->tiempo = tabla[nextIdx]->tiempo;

                        // El nodo 'next' ahora representa al anchor (pos)
                        next->index = pos;

                        // Eliminar el nodo que apuntaba al viejo anchor de la lista
                        tabla[pos]->l.eliminar(node);

                        // Liberar el slot físico del promovido
                        tombstone(nextIdx);
                        cantidad--;
                        // Continuar: aún puede haber más recursos del dominio
                    }
                }
                return;
            }
            pos = (miHash1(dominio) + (++intentos) * miHash2(dominio)) % largo;
        }
        // dominio no encontrado: nada que hacer
    }

private:
    int cantidad;
    int largo;
    float fc;

    // en vez de borrar los nodos, hay que marcarlos como "vacíos".
    void tombstone(int index)
    {
        if (!tabla[index])
            return;
        tabla[index]->libre = true;
        tabla[index]->dominio = "";
        tabla[index]->path = "";
        tabla[index]->titulo = "";
        tabla[index]->tiempo = 0;
    }

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
        dominio_hash->insertarTablaDominio(dominio, path, titulo, tiempo);
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
        dominio_hash->eliminarTablaDominio(dominio, path);
        dominio_path->eliminarCompuesto(dominio, path);
    }

    void CONTAINS(string dominio, string path)
    {
        NodoHash *busqueda = dominio_path->buscar(dominio, path);
        if (!busqueda)
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
        cout << dominio_hash->getLargoDominio(dominio) << endl;
    }

    void LIST_DOMAIN(string dominio)
    {
        dominio_hash->recorrerDominio(dominio);
    }

    void CLEAR_DOMAIN(string dominio)
    {
        NodoLista *recursosABorrar = dominio_hash->getRecursosAEliminar(dominio);
        cout << "hola" << endl;
        while (recursosABorrar)
        {
            dominio_path->eliminarCompuesto(dominio_hash->tabla[recursosABorrar->index]->dominio, dominio_hash->tabla[recursosABorrar->index]->path);
            cout << "hola" << endl;
            NodoLista *borro = recursosABorrar;
            recursosABorrar = recursosABorrar->sig;
            delete borro;
        }
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