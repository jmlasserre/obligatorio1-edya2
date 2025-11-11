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

int buscarPosicionEstudiante(string *rankingOficial, int n, string estudiante)
{
    for (int i = 0; i < n; i++)
    {
        if (rankingOficial[i] == estudiante)
        {
            return i;
        }
    }
    return -1;
}

int mezclar(int *array, int *temp, int izq, int medio, int der)
{

    int i = izq;       // indice para la mitad izquierda
    int j = medio + 1; // indice para la mitad derecha
    int k = izq;       // indice para el array temporal
    int inversiones = 0;

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
            inversiones += (medio - i + 1); // se invierte con todos los de la izquierda hasta llegar a i;
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

//esta completa el DAC, divide los array,y cuenta las inversiones
int inversionesMergeSort(int *array, int *temp, int izq, int der)
{
    int inversiones = 0;

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

    string *rankingOficial = new string[cantidadEstudiantes];
    for (int i = 0; i < cantidadEstudiantes; i++)
    {
        string estudiante;
        cin >> estudiante;
        rankingOficial[i] = estudiante;
    }

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
        int posicionOficial = buscarPosicionEstudiante(rankingOficial, cantidadEstudiantes, estudianteActual);
        posicionesSegunAyudante[i] = posicionOficial;
    }

    int *arrayAux = new int[cantidadEstudiantes]; // este es el temporal del mergeSort

    int totalInversiones = inversionesMergeSort(posicionesSegunAyudante, arrayAux, 0, cantidadEstudiantes - 1);

    cout << totalInversiones << endl;

    delete[] rankingOficial;
    delete[] rankingAyudante;
    delete[] posicionesSegunAyudante;
    delete[] arrayAux;

    return 0;
}