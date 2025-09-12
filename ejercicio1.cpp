#include <cassert>
#include <string>
#include <iostream>
#include <limits>

using namespace std;

struct NodoAVL
{
    NodoAVL *izq;
    NodoAVL *der;
    int altura;
    int puntaje;
    int id;
    string nombre;

    NodoAVL(int idJugador, string nombreJugador, int puntajeJugador)
    {
        izq = NULL;
        der = NULL;
        puntaje = puntajeJugador;
        id = idJugador;
        altura = 1;
        nombre = nombreJugador;
    }
};

class ArbolAVL
{
private:
    int cantJugadores;
    int puntajeMax;
    string top1;
    NodoAVL *raiz;

    int altura(NodoAVL *raiz)
    {
        if (!raiz)
            return 0;
        return raiz->altura;
    }

    int obtenerBalance(NodoAVL *raiz)
    {
        if (!raiz)
            return 0;
        return altura(raiz->izq) - altura(raiz->der);
    }

    // Adaptado de las slides del curso (https://avl.uruguayan.ninja/8)
    void rotarDerecha(NodoAVL *&raiz)
    {
        NodoAVL *x = raiz->izq;
        NodoAVL *T2 = x->der;

        // Rotación
        x->der = raiz;
        raiz->izq = T2;

        // Actualizo alturas
        raiz->altura = max(altura(raiz->izq), altura(raiz->der)) + 1;
        x->altura = max(altura(x->izq), altura(x->der)) + 1;

        raiz = x;
    }
    // Adaptado de las slides del curso (https://avl.uruguayan.ninja/8)
    void rotarIzquierda(NodoAVL *&raiz)
    {
        NodoAVL *y = raiz->der;
        NodoAVL *T2 = y->izq;

        // Rotación
        y->izq = raiz;
        raiz->der = T2;

        // Actualizo alturas
        raiz->altura = max(altura(raiz->izq), altura(raiz->der)) + 1;
        y->altura = max(altura(y->izq), altura(y->der)) + 1;

        raiz = y;
    }

    // Adaptado de las slides del curso (https://avl.uruguayan.ninja/7)
    void agregarJugador(NodoAVL *&raiz, int idJugador, string nombreJugador, int puntajeJugador)
    {
        if (raiz == NULL)
        {
            raiz = new NodoAVL(idJugador, nombreJugador, puntajeJugador);
            cantJugadores++;
            puntajeMax = puntajeJugador;
            top1 = nombreJugador;
            return;
        }
        if (idJugador < raiz->id)
        {
            agregarJugador(raiz->izq, idJugador, nombreJugador, puntajeJugador);
        }
        else if (idJugador > raiz->id)
            agregarJugador(raiz->der, idJugador, nombreJugador, puntajeJugador);
        else
            return; // las ID son iguales. No hay que hacer nada.

        if (puntajeJugador > puntajeMax)
        {
            puntajeMax = puntajeJugador;
            top1 = nombreJugador;
        }

        cantJugadores++;
        raiz->altura = 1 + max(altura(raiz->izq), altura(raiz->der));
        int balance = obtenerBalance(raiz);

        // Caso II
        if (balance < -1 && idJugador < raiz->izq->id)
            rotarDerecha(raiz);

        // Caso DD
        if (balance > 1 && idJugador > raiz->der->id)
            rotarIzquierda(raiz);

        // Caso ID
        if (balance < -1 && idJugador > raiz->izq->id)
        {
            rotarIzquierda(raiz->izq);
            rotarDerecha(raiz);
        }

        // Caso DI
        if (balance > 1 && idJugador < raiz->der->id)
        {
            rotarDerecha(raiz->der);
            rotarIzquierda(raiz);
        }
    }

    void encontrarJugador(NodoAVL *raiz, int id)
    {
        if (!raiz)
            cout << "jugador_no_encontrado" << endl;
        return;
        if (raiz->id == id)
        {
            cout << raiz->nombre << raiz->puntaje << endl;
            return;
        }
        if (raiz->id < id)
            encontrarJugador(raiz->der, id);
        else
            encontrarJugador(raiz->izq, id);
    }

    int puntajeSuperiorA(NodoAVL *r, int puntaje)
    {
        if (!r)
            return 0;
        if (r->puntaje >= puntaje)
            return 1 + puntajeSuperiorA(r->izq, puntaje) + puntajeSuperiorA(r->der, puntaje);
    }

public:
    ArbolAVL()
    {
        cantJugadores = 0;
        puntajeMax = 0;
        raiz = NULL;
    }

    void ADD(int idJugador, string nombreJugador, int puntajeJugador)
    {
        agregarJugador(raiz, idJugador, nombreJugador, puntajeJugador);
    }

    void FIND(int id)
    {
        encontrarJugador(raiz, id);
    }

    void RANK(int puntaje)
    {
        cout << puntajeSuperiorA(raiz, puntaje) << endl;
    }

    void TOP1()
    {
        cout << top1 << puntajeMax << endl;
    }

    void COUNT()
    {
        cout << cantJugadores << endl;
    }
};

int main()
{
    return 0;
}