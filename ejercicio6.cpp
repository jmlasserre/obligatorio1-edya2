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
#include "tads/MinHeap.cpp"
#include "tads/MaxHeap.cpp"
using namespace std;

struct Pozo
{
    int inicio, final;
    int getInicio() { return this->inicio; }
    int getFinal() { return this->final; }
    Pozo() : inicio(-1), final(-1) {}
    Pozo(int inicio, int final) : inicio(inicio), final(final) {}
    bool operator==(const Pozo &otro)
    {
        return this->inicio == otro.inicio && this->final == otro.final;
    }
};

struct Mejora
{
    int pos, valor;
    int getPos() { return this->pos; }
    int getValor() { return this->valor; }
    Mejora() : pos(-1), valor(-1) {}
    Mejora(int pos, int valor) : pos(pos), valor(valor) {};
    bool operator==(const Mejora &otro) const
    {
        return this->valor == otro.valor;
    }

    bool operator<(const Mejora &otro) const
    {
        return this->valor < otro.valor;
    }

    bool operator>(const Mejora &otro) const
    {
        return this->valor > otro.valor;
    }
    bool operator>=(const Mejora &otro) const{
        return this->valor >= otro.valor;
    }
};

int main()
{
    int N, M, F;
    cin >> N >> M >> F;
    //MinHeap<Mejora> *mejoras = new MinHeap<Mejora>(M);
    ListImp<Pozo> *pozos = new ListImp<Pozo>();

    for (int i = 0; i < N; i++)
    {
        int inicio, fin;
        cin >> inicio >> fin;
        pozos->insert(Pozo(inicio, fin));
    }
    /*for (int i = 0; i < M; i++)
    {
        int direccion, mejora;
        cin >> direccion >> mejora;
        mejoras->insertar(Mejora(direccion, mejora));
    }

    while (!mejoras->esVacio()){
        Mejora m = mejoras->eliminarMin();
        cout << "(" << m.getPos() << ", " << m.getValor() << ")" << endl;
    }*/
    MaxHeap<Mejora>* inventario = new MaxHeap<Mejora>(M);
    int minMejoras = 0, potencia = 1, pos = 1, posMaxMejora = 0, maxMejora = 0, maxPos = 0, pozoActual = 0, cantMejoras=0;
    int movs = 1; // eliminar
    while (!pozos->isEmpty())
    {
        int rango = pos + potencia;
        if (rango >= F)
        {
            cout << minMejoras << endl;
            return 0;
        }
        Pozo p = pozos->get(0);
        int inicio = p.getInicio();
        int final = p.getFinal();
        cout << "Debug: Inicio del pozo: " << inicio << endl;
        cout << "Debug: Final del pozo: " << final << endl;
        if (rango > final)
        {
            pos = final + 1; // hay al menos un espacio entre pozos
            pozos->removeAt(0);
        }
        else
        {
            // si está en un agujero, recién ahí empiezo a agarrar mejoras
            int posTemp = inicio-1;
            while (cantMejoras < M){
                int pos, valor;
                cin >> pos >> valor;
                cout << "Debug: posTemp = " << posTemp << endl;
                if (pos > posTemp) break; // está más allá de mi alcance, no la puedo acceder
                inventario->insertar(Mejora(pos, valor));
                cantMejoras++;
            }
            while (!inventario->esVacio() && (posTemp + potencia) <= final)
            {
                Mejora m = inventario->eliminarMax();
                cout << "Debug: Valor de la mejora: " << m.getValor() << endl;
                potencia += m.getValor();
                minMejoras++;
            }
            cout << "Debug: Potencia actual: " << potencia << endl;
            cout << "Debug: Nuevo rango: " << posTemp + potencia << endl;
            if (posTemp + potencia <= final)
            { // agarré todas las mejoras y no lo pude pasar, no se puede terminar.
                cout << "Imposible" << endl;
                return 0;
            }
            else
            {
                pos = final + 1;
                pozos->removeAt(0);
            }
        }
    }
    cout << minMejoras << endl;
    return 0;
}

// NOTA: PENSAR DE ESTA MANERA:
// a medida que voy avanzando, voy consiguiendo mejoras. ESAS mejoras van en un MaxHeap para así ir sacando
// siempre la más grande antes. Habría que ver cómo hacer esta parte. Después la lógica es similar.