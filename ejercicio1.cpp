#include <cassert>
#include <string>
#include <iostream>
#include <limits>

using namespace std;

struct NodoLista
{
    NodoLista *sig;
    string nombre;
    int id;
};

typedef NodoLista *Lista;

Lista crearListaVacia()
{
    Lista l = new NodoLista;
    l->sig = NULL;
    l->nombre = "";
    l->id = 0;
    return l;
}

void insertar(Lista &l, int id, string nombre)
{
    NodoLista *nuevo = new NodoLista;
    nuevo->id = id;
    nuevo->nombre = nombre;
    nuevo->sig = l;
}

struct NodoAVL
{
    NodoAVL *izq;
    NodoAVL *der;
    int altura;
    int puntaje;
    NodoLista *listaJugadores;

    NodoAVL(int idJugador, string nombreJugador, int puntajeJugador)
    {
        izq = NULL;
        der = NULL;
        NodoLista *jugadores = crearListaVacia();
        listaJugadores = jugadores;
        insertar(listaJugadores, idJugador, nombreJugador);
        puntaje = puntajeJugador;
        altura = 1;
    }
};

class ArbolAVL
{
private:
    int cantidad;
    NodoAVL *raiz;

public:
    ArbolAVL(int cantidad)
    {
        cantidad = cantidad;
        raiz = NULL;
    }

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
    void ADD(NodoAVL *&raiz, int idJugador, string nombreJugador, int puntajeJugador)
    {
        if (raiz == NULL)
        {
            raiz = new NodoAVL(idJugador, nombreJugador, puntajeJugador);
            return;
        }
        if (puntajeJugador < raiz->puntaje)
        {
            ADD(raiz->izq, idJugador, nombreJugador, puntajeJugador);
        }
        else if (puntajeJugador > raiz->puntaje)
            ADD(raiz->der, idJugador, nombreJugador, puntajeJugador);
        else
            insertar(raiz->listaJugadores, idJugador, nombreJugador);
        raiz->altura = 1 + max(altura(raiz->izq), altura(raiz->der));

        int balance = obtenerBalance(raiz);

        // Caso II
        if (balance < -1 && puntajeJugador < raiz->izq->puntaje)
            rotarDerecha(raiz);

        // Caso DD
        if (balance > 1 && puntajeJugador > raiz->der->puntaje)
            rotarIzquierda(raiz);

        // Caso ID
        if (balance < -1 && puntajeJugador > raiz->izq->puntaje)
        {
            rotarIzquierda(raiz->izq);
            rotarDerecha(raiz);
        }

        // Caso DI
        if (balance > 1 && puntajeJugador < raiz->der->puntaje)
        {
            rotarDerecha(raiz->der);
            rotarIzquierda(raiz);
        }
    }

    void FIND(int id)
    {
        // TODO
        // Debe imprimir "{nombre} {puntaje}" o "jugador_no_encontrado"
    }

    void RANK(int puntaje)
    {
        // TODO
        // imprime la cantidad de jugadores con puntaje >= al indicado
    }

    void TOP1()
    {
        // TODO
        // retorna el jugador con mayor puntaje
    }

    void COUNT()
    {
        cout << cantidad;
    }
};

int main()
{
    return 0;
}