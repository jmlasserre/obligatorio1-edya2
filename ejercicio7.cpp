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

    int i = izq;
    int j = medio + 1;
    int k = izq;
    long long inversiones = 0;

    while (i <= medio && j <= der)
    {
        if (array[i] <= array[j])
        {
            temp[k] = array[i];
            k++;
            i++;
        }
        else
        {
            temp[k] = array[j];
            k++;
            j++;
            inversiones += (long long)(medio - i + 1);
        }
    }

    while (i <= medio)
    {
        temp[k] = array[i];
        k++;
        i++;
    }

    while (j <= der)
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

long long inversionesMergeSort(int *array, int *temp, int izq, int der)
{
    long long inversiones = 0;

    if (izq < der)
    {
        int medio = (izq + der) / 2;

        inversiones += inversionesMergeSort(array, temp, izq, medio);
        inversiones += inversionesMergeSort(array, temp, medio + 1, der);
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
    {
        string estudiante;
        cin >> estudiante;
        rankingOficial->insertar(ParEstPos(estudiante, i));
    }

    string *rankingAyudante = new string[cantidadEstudiantes];
    for (int i = 0; i < cantidadEstudiantes; i++)
    {
        string estudiante;
        cin >> estudiante;
        rankingAyudante[i] = estudiante;
    }

    int *posicionesSegunAyudante = new int[cantidadEstudiantes];
    for (int i = 0; i < cantidadEstudiantes; i++)
    {
        string estudianteActual = rankingAyudante[i];
        ParEstPos posicionOficial = buscarPosicionEstudiante(rankingOficial, cantidadEstudiantes, estudianteActual);
        posicionesSegunAyudante[i] = posicionOficial.getPos();
    }

    int *arrayAux = new int[cantidadEstudiantes];

    long long totalInversiones = inversionesMergeSort(posicionesSegunAyudante, arrayAux, 0, cantidadEstudiantes - 1);

    cout << totalInversiones << endl;

    return 0;
}