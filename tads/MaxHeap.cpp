#ifndef MAX_HEAP
#define MAX_HEAP

// Recuperado del ejercicio 5.

template <class T>
class MaxHeap
{
private:
    int sizeActual;
    int max;
    T* heap;

    int padre(int index) { return (index - 1) / 2; }
    int hijoIzq(int index) { return 2 * index + 1; }
    int hijoDer(int index) { return 2 * index + 2; }

    void flotar(int index)
    {
        while (index > 0 && heap[index] > heap[padre(index)]) // T necesita bool operator>
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
            if (heap[j] < heap[j + 1]) // T necesita bool operator<
                j++;
            if (heap[index] >= heap[j]) // T necesita bool operator >=
                break;
            swap(index, j);
            index = j;
        }
    }

    void swap(int a, int b)
    {
        T temp = heap[a];
        heap[a] = heap[b];
        heap[b] = temp;
    }

public:
    MaxHeap(int capacidad)
    {
        sizeActual = 0;
        max = capacidad;
        heap = new T[capacidad];
    }
    ~MaxHeap()
    {
        delete[] heap;
    }

    bool esVacio() { return sizeActual == 0; }

    int getLargo() { return sizeActual; }

    void insertar(T e)
    {
        if (sizeActual == max)
            return;
        heap[sizeActual] = e;
        flotar(sizeActual++);
    }

    T eliminarMax()
    {
        if (esVacio())
            return T();
        T max = heap[0];
        heap[0] = heap[--sizeActual];
        hundir(0);
        return max;
    }

    T getMax(){
        return heap[0];
    }
};

#endif