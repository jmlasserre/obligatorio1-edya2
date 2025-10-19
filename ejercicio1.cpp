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

struct NodoAVL
{
    NodoAVL *izq;
    NodoAVL *der;
    int altura;
    int paramMain;
    int tamSub;
    int paramSec;
    string nombre;
    int reps;

    NodoAVL(int paramMainJugador, int paramSecJugador, string nombreJugador)
    {
        izq = NULL;
        der = NULL;
        paramMain = paramMainJugador;
        paramSec = paramSecJugador;
        altura = 1;
        nombre = nombreJugador;
        reps = 0;
        tamSub = 1;
    }
};

class ArbolAVL
{
private:
    bool aceptaReps;

    int tamanio(NodoAVL *n) { return n ? n->tamSub : 0; }

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
        assert(raiz != nullptr);
        assert(raiz->izq != nullptr);
        NodoAVL *x = raiz->izq;
        NodoAVL *T2 = x->der;

        // Rotación
        x->der = raiz;
        raiz->izq = T2;

        // Actualizo alturas
        raiz->altura = max(altura(raiz->izq), altura(raiz->der)) + 1;
        x->altura = max(altura(x->izq), altura(x->der)) + 1;

        raiz->tamSub = tamanio(raiz->izq) + tamanio(raiz->der) + 1 + raiz->reps;
        x->tamSub = tamanio(x->izq) + tamanio(x->der) + 1 + x->reps;

        raiz = x;
    }
    // Adaptado de las slides del curso (https://avl.uruguayan.ninja/8)
    void rotarIzquierda(NodoAVL *&raiz)
    {
        assert(raiz != nullptr);
        assert(raiz->der != nullptr);
        NodoAVL *y = raiz->der;
        NodoAVL *T2 = y->izq;

        // Rotación
        y->izq = raiz;
        raiz->der = T2;

        // Actualizo alturas
        raiz->altura = max(altura(raiz->izq), altura(raiz->der)) + 1;
        y->altura = max(altura(y->izq), altura(y->der)) + 1;

        raiz->tamSub = tamanio(raiz->izq) + tamanio(raiz->der) + 1 + raiz->reps;
        y->tamSub = tamanio(y->izq) + tamanio(y->der) + 1 + y->reps;

        raiz = y;
    }

public:
    int cantidad;
    NodoAVL *raiz;
    ArbolAVL(bool aceptaReps)
    {
        cantidad = 0;
        raiz = NULL;
        this->aceptaReps = aceptaReps;
    }
    void encontrarJugador(NodoAVL *raiz, int id)
    {
        if (!raiz)
        {
            cout << "Jugador_no_encontrado" << endl;
            return;
        }
        if (raiz->paramMain == id)
        {
            cout << raiz->nombre << " " << raiz->paramSec << endl;
            return;
        }
        if (raiz->paramMain < id)
            encontrarJugador(raiz->der, id);
        else
            encontrarJugador(raiz->izq, id);
    }
    // Adaptado de las slides del curso (https://avl.uruguayan.ninja/7)
    void agregarNodo(NodoAVL *&raiz, int paramMainJugador, int paramSecJugador, string nombreJugador)
    {
        if (raiz == NULL)
        {
            raiz = new NodoAVL(paramMainJugador, paramSecJugador, nombreJugador);
            cantidad++;
            return;
        }
        if (paramMainJugador < raiz->paramMain)
        {
            agregarNodo(raiz->izq, paramMainJugador, paramSecJugador, nombreJugador);
        }
        else if (paramMainJugador > raiz->paramMain)
        {
            agregarNodo(raiz->der, paramMainJugador, paramSecJugador, nombreJugador);
        }
        else
        {
            if (aceptaReps)
            {
                raiz->reps++;
                raiz->tamSub++;
            }
            else
            {
                return;
            }
        }
        raiz->altura = max(altura(raiz->izq), altura(raiz->der));
        raiz->tamSub = tamanio(raiz->izq) + tamanio(raiz->der) + 1 + raiz->reps;
        int balance = obtenerBalance(raiz);

        // Left Left (LL)
        if (balance > 1 && raiz->izq && paramMainJugador < raiz->izq->paramMain)
            rotarDerecha(raiz);

        // Right Right (RR)
        if (balance < -1 && raiz->der && paramMainJugador > raiz->der->paramMain)
            rotarIzquierda(raiz);

        // Left Right (LR)
        if (balance > 1 && raiz->izq && paramMainJugador > raiz->izq->paramMain)
        {
            rotarIzquierda(raiz->izq);
            rotarDerecha(raiz);
        }

        // Right Left (RL)
        if (balance < -1 && raiz->der && paramMainJugador < raiz->der->paramMain)
        {
            rotarDerecha(raiz->der);
            rotarIzquierda(raiz);
        }
    }
};

class Ranking
{
private:
    ArbolAVL *jugadores;
    ArbolAVL *puntajes;
    int puntajeMax;
    string top1;

    int puntajeSuperiorA(NodoAVL *r, int puntaje)
    {
        if (!r)
            return 0;
        if (r->paramMain > puntaje){
            return (r->der ? r->der->tamSub : 0) + 1 + r->reps + puntajeSuperiorA(r->izq, puntaje);
        } else if (r->paramMain < puntaje){
            return puntajeSuperiorA(r->der, puntaje);
        } else {
            return (r->der ? r->der->tamSub : 0) + 1 + r->reps;
        }
    }

public:
    Ranking()
    {
        jugadores = new ArbolAVL(false);
        puntajes = new ArbolAVL(true);
        puntajeMax = 0;
        top1 = "";
    }
    void ADD(int idJugador, string nombreJugador, int puntajeJugador)
    {
        int cantTemp = jugadores->cantidad;
        jugadores->agregarNodo(jugadores->raiz, idJugador, puntajeJugador, nombreJugador);
        if (cantTemp < jugadores->cantidad)
        { // se agregó un jugador
            puntajes->agregarNodo(puntajes->raiz, puntajeJugador, idJugador, nombreJugador);
            if (puntajeJugador > puntajeMax)
            {
                puntajeMax = puntajeJugador;
                top1 = nombreJugador;
            }
        }
    }

    void FIND(int id)
    {
        jugadores->encontrarJugador(jugadores->raiz, id);
    }

    void RANK(int puntaje)
    {
        cout << puntajeSuperiorA(puntajes->raiz, puntaje) << endl;
    }

    void TOP1()
    {
        cout << top1 << " " << puntajeMax << endl;
    }

    void COUNT()
    {
        cout << jugadores->cantidad << endl;
    }
};

int main()
{
    int N;
    string valor;
    cin >> N;
    Ranking *ranking = new Ranking();
    for (int i = 0; i < N; i++)
    {
        string comando;
        cin >> comando;
        if (comando == "ADD")
        {
            int id;
            int puntaje;
            string nombre;
            cin >> id >> nombre >> puntaje;
            ranking->ADD(id, nombre, puntaje);
        }
        else if (comando == "FIND")
        {
            int id;
            cin >> id;
            ranking->FIND(id);
        }
        else if (comando == "RANK")
        {
            int puntaje;
            cin >> puntaje;
            ranking->RANK(puntaje);
        }
        else if (comando == "TOP1")
        {
            ranking->TOP1();
        }
        else if (comando == "COUNT")
        {
            ranking->COUNT();
        }
        else
        {
            cout << "Comando_inválido";
        }
    }
    return 0;
}