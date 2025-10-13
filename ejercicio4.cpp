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

class minHeap
{
private:
    int *heapListas;
    int *heapValores;
    int capacidad;
    int tamano;

    int padre(int i)
    {
        return (i - 1) / 2;
    }
    int hijoIzq(int i)
    {
        return 2 * i + 1;
    }
    int hijoDer(int i)
    {
        return 2 * i + 2;
    }

    void intercambiarValores(int i, int j)
    {
        int auxValores = heapValores[i];
        heapValores[i] = heapValores[j];
        heapValores[j] = auxValores;

        int auxLista = heapListas[i];
        heapListas[i] = heapListas[j];
        heapListas[j] = auxLista;
    }

    void flotar(int i)
    {
        while (i > 0 && heapValores[padre(i)] > heapValores[i])
        {
            intercambiarValores(i, padre(i));
            i = padre(i);
        }
    }

    void hundir(int i)
    {
        int menor = i;
        int izq = hijoIzq(i);
        int der = hijoDer(i);

        if (izq < tamano && heapValores[izq] < heapValores[menor])
        {
            menor = izq;
        }
        if (der < tamano && heapValores[der] < heapValores[menor])
        {
            menor = der;
        }
        if (menor != i)
        {
            intercambiarValores(i, menor);
            hundir(menor);
        }
    }

public:
    minHeap(int capacidadMax)
    {
        capacidad = capacidadMax;
        tamano = 0;
        heapValores = new int[capacidad];
        heapListas = new int[capacidad];
    }

    ~minHeap()
    {
        delete[] heapValores;
        delete[] heapListas;
    }

    void insertar(int valor, int listaOrigen)
    {
        if (tamano == capacidad)
        {
            cout << "Heap lleno" << endl;
            return;
        }
        heapValores[tamano] = valor;
        heapListas[tamano] = listaOrigen;
        flotar(tamano);
        tamano++;
    }

    int valorTope()
    {
        if (tamano == 0)
        {
            cout << "Heap vacío" << endl;
            return -1;
        }
        return heapValores[0];
    }

    int listaOrigenTope()
    {
        if (tamano == 0)
        {
            cout << "Heap vacío" << endl;
            return -1;
        }
        return heapListas[0];
    }

    void eliminarTope()
    {
        if (tamano == 0)
        {
            cout << "Heap vacío" << endl;
            return;
        }
        heapValores[0] = heapValores[tamano - 1];
        heapListas[0] = heapListas[tamano - 1];
        tamano--;
        hundir(0);
    }

    bool vacio()
    {
        return tamano == 0;
    }
};

int main()
{

    int k;
    cin >> k;

    int **listas = new int *[k];
    int *tamanos = new int[k];

    for (int i = 0; i < k; i++)
    {
        int Ni;
        cin >> Ni;
        tamanos[i] = Ni;
        listas[i] = new int[Ni];

        for (int j = 0; j < Ni; j++)
        {
            cin >> listas[i][j];
        }
    }

    int *indices = new int[k]; // indices por lista para ver en que lugar estoy de cada lista;
    for (int i = 0; i < k; i++)
    {
        indices[i] = 0;
    }

    minHeap heap(k);

    for (int i = 0; i < k; i++)
    {
        if (tamanos[i] > 0)
        {
            heap.insertar(listas[i][0], i);
            indices[i] = 1;
        }
    }

    while (!heap.vacio())
    {
        int valor = heap.valorTope();
        int listaActual = heap.listaOrigenTope();

        cout << valor << endl;
        heap.eliminarTope();

        if (indices[listaActual] < tamanos[listaActual])
        {
            heap.insertar(listas[listaActual][indices[listaActual]], listaActual);
            indices[listaActual]++;
        }
    }

    for (int i = 0; i < k; i++)
    {
        delete[] listas[i];
    }
    delete[] listas;
    delete[] indices;
    delete[] tamanos;

    return 0;
}