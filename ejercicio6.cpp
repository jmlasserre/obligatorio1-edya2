/*
    Facundo García - 328757
    Juan Martín Lasserre - 289139
    Universidad ORT Uruguay - M4A - 2025
*/

#include <cassert>
#include <string>
#include <iostream>
#include <limits>
#include "tads/MaxHeap.cpp"
#include "tads/ListImp.cpp"

using namespace std;

struct Mejora
{
    int pos, potencia;
    Mejora() : pos(-1), potencia(0) {}
    Mejora(int pos, int pot) : pos(pos), potencia(pot) {}
    int getPos() { return this->pos; }
    int getPotencia() { return this->potencia; }

    bool operator==(const Mejora &otra)
    {
        return this->potencia == otra.potencia;
    }

    bool operator>=(const Mejora &otra)
    {
        return this->potencia >= otra.potencia;
    }

    bool operator<(const Mejora &otra)
    {
        return this->potencia < otra.potencia;
    }

    bool operator>(const Mejora &otra)
    {
        return this->potencia > otra.potencia;
    }
};
struct Pozo
{
    int inicio, final;
    Pozo() : inicio(-1), final(-1) {}
    Pozo(int i, int f) : inicio(i), final(f) {}
    int getInicio() { return this->inicio; }
    int getFinal() { return this->final; }
};

int main()
{
    // está mal la complejidad pero honestamente, ni idea. Por lo menos pasa las pruebas
    int N, M, F;
    cin >> N >> M >> F;
    Pozo* pozos = new Pozo[N];
    for (int i = 0; i < N; i++){
        int inicio, final;
        cin >> inicio >> final;
        pozos[i] = Pozo(inicio, final);
    }

    ListImp<Mejora> *mejoras = new ListImp<Mejora>;
    for (int i = 0; i < M; i++)
    {
        int pos, potencia;
        cin >> pos >> potencia;
        mejoras->insert(Mejora(pos, potencia));
    }
    int minMejoras = 0;
    int pos = 0;
    int potencia = 1;
    MaxHeap<Mejora> *inventario = new MaxHeap<Mejora>(M);

    for (int i = 0; i < N && (pos + potencia) < F; i++)
    {
        Pozo p = pozos[i];
        int inicio, final;
        inicio = p.getInicio();
        final = p.getFinal();
        pos = inicio - 1;
        int rango = pos + potencia;
        if (rango >= F)
            break;
        while (!mejoras->isEmpty() && mejoras->get(0).getPos() <= pos)
        {
            inventario->insertar(mejoras->get(0));
            mejoras->removeAt(0);
        }
        while (!inventario->esVacio() && (pos + potencia) <= final)
        {
            potencia += inventario->eliminarMax().getPotencia();
            minMejoras++;
        }
        if ((pos + potencia) <= final)
        {
            cout << "Imposible" << endl;
            return 0;
        }
        pos = final + 1;
    }
    cout << minMejoras << endl;
    return 0;
}