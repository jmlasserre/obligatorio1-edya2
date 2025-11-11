#ifndef MIN_HEAP
#define MIN_HEAP

template <class T>
class MinHeap
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
        while (index > 0 && heap[index] < heap[padre(index)])
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
            if (heap[j] > heap[j + 1])
                j++;
            if (heap[index] <= heap[j])
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
    MinHeap(int capacidad)
    {
        sizeActual = 0;
        max = capacidad;
        heap = new T[capacidad];
    }
    ~MinHeap()
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

    T eliminarMin()
    {
        if (esVacio())
            return T();
        T min = heap[0];
        heap[0] = heap[--sizeActual];
        hundir(0);
        return min;
    }

    T getMin(){
        return heap[0];
    }
};

#endif