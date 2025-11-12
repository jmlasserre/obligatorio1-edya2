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
    bool operator==(const ParEstPos& otro){
        return this->estudiante == otro.estudiante && this->pos == otro.pos;
    }
};

// hash abierto para acelerar recorrida
class TablaHash
{
private:
    ListImp<ParEstPos> **tabla;
    int largo, cantidad;

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

public:
    TablaHash() : tabla(nullptr), largo(-1), cantidad(0) {}
    TablaHash(int max)
    {
        tabla = new ListImp<ParEstPos>*[max];
        cantidad = 0;
        largo = max;
        for (int i = 0; i < largo; i++) tabla[i] = new ListImp<ParEstPos>();
    }

    void insertar(ParEstPos estpos)
    {
        int pos = miHash1(estpos.getEstudiante())%largo;
        tabla[pos]->insert(estpos);
        cantidad++;
    }

    int buscar(string estudiante){
        int pos = miHash1(estudiante)%largo;
        ListImp<ParEstPos>* bucket = tabla[pos];
        int largoBucket = bucket->getSize();
        for (int i = 0; i < largoBucket; i++){
            ParEstPos estpos = bucket->get(i);
            if (estpos.getEstudiante() == estudiante) return estpos.getPos();
        }
        return -1;
    }
};

int buscarPosicionEstudiante(TablaHash* rankingOficial, int n, string estudiante){
    return rankingOficial->buscar(estudiante);
}

// hay que cambiar esta función por una tabla de hash para acelerar. Demasiado lenta para n >= 100000
/*int buscarPosicionEstudiante(string *rankingOficial, int n, string estudiante)
{
    for (int i = 0; i < n; i++)
    {
        if (rankingOficial[i] == estudiante)
        {
            return i;
        }
    }
    return -1;
}*/

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
    TablaHash* rankingOficial = new TablaHash(cantidadEstudiantes);
    for (int i = 0; i < cantidadEstudiantes; i++){
        string estudiante;
        cin >> estudiante;
        rankingOficial->insertar(ParEstPos(estudiante, i));
    }
    /*string *rankingOficial = new string[cantidadEstudiantes];
    for (int i = 0; i < cantidadEstudiantes; i++)
    {
        string estudiante;
        cin >> estudiante;
        rankingOficial[i] = estudiante;
    }*/

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

    long long totalInversiones = inversionesMergeSort(posicionesSegunAyudante, arrayAux, 0, cantidadEstudiantes - 1);

    cout << totalInversiones << endl;

    delete rankingOficial;
    delete[] rankingAyudante;
    delete[] posicionesSegunAyudante;
    delete[] arrayAux;

    return 0;
}

/* Pedimos ayuda a ChatGPT para saber por qué el programa daba error con las pruebas más grandes. Inmediatamente sospechamos una cuestión de memoria.
   Sugirió utilizar 'long long' en vez de 'int' para almacenar los resultados por las limitaciones de memoria del primero. Aplicamos su sugerencia y el problema se resolvió.
*/