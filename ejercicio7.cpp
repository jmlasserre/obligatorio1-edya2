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
#include "tads/AVL.cpp"

using namespace std;

class ParEstPos
{
private:
    string estudiante;
    int pos;

public:
    ParEstPos() : estudiante(""), pos(-1) {}
    ParEstPos(string est, int posicion) : estudiante(est), pos(posicion) {}
    string getEstudiante() { return this->estudiante; }
    int getPos() { return this->pos; }
    bool operator==(const ParEstPos &otro)
    {
        return this->estudiante == otro.estudiante;
    }
    bool operator<(const ParEstPos &otro)
    {
        return this->estudiante < otro.estudiante;
    }
    bool operator>(const ParEstPos &otro)
    {
        return this->estudiante > otro.estudiante;
    }
};

ParEstPos buscarPosicionEstudiante(ArbolAVL<ParEstPos> *rankingOficial, int n, string estudiante)
{
    return rankingOficial->buscar(ParEstPos(estudiante, 0));
}

long long mezclar(int *array, int *temp, int izq, int medio, int der)
{

    int i = izq;       // indice para la mitad izquierda
    int j = medio + 1; // indice para la mitad derecha
    int k = izq;       // indice para el array temporal
    long long inversiones = 0;

    while (i <= medio && j <= der)
    {
        if (array[i] <= array[j])
        {
            temp[k] = array[i];
            k++;
            i++;
        }
        else // array[i] > array[j]
        {
            temp[k] = array[j];
            k++;
            j++;
            inversiones += (long long)(medio - i + 1); // se invierte con todos los de la izquierda hasta llegar a i;
        }
    }

    while (i <= medio) // si sobraron elementos en la izquierda los copio
    {
        temp[k] = array[i];
        k++;
        i++;
    }

    while (j <= der) // si sobraron elementos en la derecha los copio
    {
        temp[k] = array[j];
        k++;
        j++;
    }

    for (int z = izq; z <= der; z++)
    {
        array[z] = temp[z];
    }
    return inversiones;
}

// esta completa el DAC, divide los array,y cuenta las inversiones
long long inversionesMergeSort(int *array, int *temp, int izq, int der)
{
    long long inversiones = 0;

    if (izq < der)
    {
        int medio = (izq + der) / 2;

        // Divido el array en dos hasta llegar a casos base
        inversiones += inversionesMergeSort(array, temp, izq, medio);
        inversiones += inversionesMergeSort(array, temp, medio + 1, der);
        // mezclo las mitades y sumo
        inversiones += mezclar(array, temp, izq, medio, der);
    }

    return inversiones;
}

int main()
{
    int cantidadEstudiantes;
    cin >> cantidadEstudiantes;
    ArbolAVL<ParEstPos> *rankingOficial = new ArbolAVL<ParEstPos>();
    for (int i = 0; i < cantidadEstudiantes; i++)
    { // O(N)
        string estudiante;
        cin >> estudiante;
        rankingOficial->insertar(ParEstPos(estudiante, i)); // O(log N)
    }
    // total: O(N log N)

    string *rankingAyudante = new string[cantidadEstudiantes];
    for (int i = 0; i < cantidadEstudiantes; i++)
    {
        string estudiante;
        cin >> estudiante;
        rankingAyudante[i] = estudiante;
    }

    // las posiciones oficiales estan en el rankingOficial, veo en que posiciones puso el ayudante cada posicionOficial
    int *posicionesSegunAyudante = new int[cantidadEstudiantes];
    for (int i = 0; i < cantidadEstudiantes; i++)
    {
        string estudianteActual = rankingAyudante[i];
        ParEstPos posicionOficial = buscarPosicionEstudiante(rankingOficial, cantidadEstudiantes, estudianteActual);
        posicionesSegunAyudante[i] = posicionOficial.getPos();
    }

    int *arrayAux = new int[cantidadEstudiantes]; // este es el temporal del mergeSort

    long long totalInversiones = inversionesMergeSort(posicionesSegunAyudante, arrayAux, 0, cantidadEstudiantes - 1);

    cout << totalInversiones << endl;

    delete rankingOficial;
    delete[] rankingAyudante;
    delete[] posicionesSegunAyudante;
    delete[] arrayAux;

    return 0;
}