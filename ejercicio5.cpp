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

class Arista
{
private:
    int peso;
    int o; // vertice de origen
    int f; // vertice de llegada;

public:
    Arista() : o(-1), f(-1), peso(-1) {}
    Arista(int peso, int origen, int final) : o(origen), f(final), peso(peso) {}

    int getPeso() { return peso; }
    int desde() { return o; }
    int hasta() { return f; }
};

// Adaptado de: https://www.geeksforgeeks.org/cpp/cpp-program-to-implement-binary-heap/
class MinHeapAristas
{
private:
    Arista *heap;
    int sizeActual;
    int max;

    int padre(int index) { return (index - 1) / 2; }
    int hijoIzq(int index) { return 2 * index + 1; }
    int hijoDer(int index) { return 2 * index + 2; }

    void flotar(int index)
    {
        while (index > 0 && heap[index].getPeso() < heap[padre(index)].getPeso())
        {
            swap(index, padre(index));
            index = padre(index);
        }
    }

    void hundir(int index)
    {
        while (hijoIzq(index) < sizeActual)
        {
            int j = hijoIzq(index);
            if (heap[j].getPeso() > heap[j + 1].getPeso())
                j++;
            if (heap[index].getPeso() <= heap[j].getPeso())
                break;
            swap(index, j);
            index = j;
        }
    }

    void swap(int a, int b)
    {
        Arista temp = heap[a];
        heap[a] = heap[b];
        heap[b] = temp;
    }

public:
    MinHeapAristas(int capacidad)
    {
        sizeActual = 0;
        max = capacidad;
        heap = new Arista[capacidad];
    }
    ~MinHeapAristas()
    {
        delete[] heap;
    }

    bool esVacio() { return sizeActual == 0; }

    int getLargo() { return sizeActual; }

    void insertar(Arista arista)
    {
        if (sizeActual == max)
            return;
        heap[sizeActual] = arista;
        flotar(sizeActual++);
    }

    Arista eliminarMin()
    {
        if (esVacio())
            return Arista();
        Arista min = heap[0];
        heap[0] = heap[--sizeActual];
        hundir(0);
        return min;
    }
};

class ConjuntosDisjuntos
{
private:
    int *padre; // vertice "raiz"
    int *rango; // para optimizar, que el rango mas chico se una al de rango mas grande (no lo tengo muy claro a esto)
    int tam;
    int cantidadConjuntos;

public:
    ConjuntosDisjuntos(int n)
    {
        tam = n;
        cantidadConjuntos = n;
        padre = new int[n];
        rango = new int[n];

        for (int i = 0; i < n; i++)
        {
            padre[i] = i;
            rango[i] = 0;
        }
    }

    ~ConjuntosDisjuntos()
    {
        delete[] padre;
        delete[] rango;
    }

    int encontrar(int x)
    {
        if (padre[x] != x)
        {
            padre[x] = encontrar(padre[x]); // voy subiendo hasta llegar a la raiz. Es el padre de todo el conjunto
        }
        return padre[x];
    }

    void unir(int x, int y)
    {
        int raizX = encontrar(x);
        int raizY = encontrar(y);

        if (raizX == raizY)
            return;

        if (rango[raizX] < rango[raizY])
        { // para optimizar, el de rango mas chico se une al de rango mas grande
            padre[raizX] = raizY;
        }
        else if (rango[raizX] > rango[raizY])
        {
            padre[raizY] = raizX;
        }
        else
        {
            padre[raizY] = raizX;
            rango[raizX]++;
        }
        cantidadConjuntos--;
    }

    bool mismoConjunto(int x, int y)
    {
        return encontrar(x) == encontrar(y);
    }

    int obtenerCantidadConjuntos()
    {
        return cantidadConjuntos;
    }
};

class Grafo
{
private:
    MinHeapAristas heap;
    int V, A;
    // no es necesario un bool esDirigido porque ya lo asumimos por letra.

public:
    Grafo(int vertices, int aristas) : heap(aristas), V(vertices), A(aristas) {}

    void agregarArista(int origen, int destino, int peso)
    {
        heap.insertar(Arista(peso, origen, destino));
    }

    int kruskal()
    {
        int pesoACM = 0;
        int cantAristasACM = 0;
        ConjuntosDisjuntos ds = ConjuntosDisjuntos(V);
        while (!heap.esVacio() && cantAristasACM < V - 1)
        {
            Arista arista = heap.eliminarMin();
            if (ds.encontrar(arista.desde()) != ds.encontrar(arista.hasta()))
            {
                cantAristasACM++;
                pesoACM += arista.getPeso();
                ds.unir(arista.desde(), arista.hasta());
            }
        }
        return pesoACM;
    }
};

int main()
{
    int V, E;
    cin >> V >> E;
    Grafo *grafo = new Grafo(V, E);
    for (int i = 0; i < E; i++)
    {
        int origen, destino, peso;
        cin >> origen >> destino >> peso;
        grafo->agregarArista(origen, destino, peso);
    }
    cout << grafo->kruskal() << endl;
}