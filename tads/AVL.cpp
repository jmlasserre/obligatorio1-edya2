#ifndef AVL
#define AVL

using namespace std;

template <class T>
class ArbolAVL
{
private:
    struct NodoAVL
    {
        NodoAVL *izq;
        NodoAVL *der;
        T dato;
        int altura;

        NodoAVL(T dato)
        {
            izq = NULL;
            der = NULL;
            this->dato = dato;
            altura = 1;
        }
    };
    NodoAVL *raiz;
    int tamanio(NodoAVL *n) { return n ? n->tamSub : 0; }
    int altura(NodoAVL *r)
    {
        if (!r)
            return 0;
        return r->altura;
    }
    int obtenerBalance(NodoAVL *r)
    {
        if (!r)
            return 0;
        return altura(r->izq) - altura(r->der);
    }
    // Adaptado de las slides del curso (https://avl.uruguayan.ninja/8)
    NodoAVL *rotarDerecha(NodoAVL *r)
    {
        assert(r != nullptr);
        assert(r->izq != nullptr);
        NodoAVL *x = r->izq;
        NodoAVL *T2 = x->der;

        // Rotación
        x->der = r;
        r->izq = T2;

        // Actualizo alturas
        r->altura = max(altura(r->izq), altura(r->der)) + 1;
        x->altura = max(altura(x->izq), altura(x->der)) + 1;

        r = x;

        return x;
    }
    // Adaptado de las slides del curso (https://avl.uruguayan.ninja/8)
    NodoAVL *rotarIzquierda(NodoAVL *r)
    {
        assert(r != nullptr);
        assert(r->der != nullptr);
        NodoAVL *y = r->der;
        NodoAVL *T2 = y->izq;

        // Rotación
        y->izq = r;
        r->der = T2;

        // Actualizo alturas
        r->altura = max(altura(r->izq), altura(r->der)) + 1;
        y->altura = max(altura(y->izq), altura(y->der)) + 1;

        r = y;
        return y;
    }
    NodoAVL *agregar(NodoAVL *r, T dato)
    {
        if (r == NULL)
        {
            cantidad++;
            return new NodoAVL(dato);
        }
        if (dato < r->dato)
        {
            r->izq = agregar(r->izq, dato);
        }
        else if (dato > r->dato)
        {
            r->der = agregar(r->der, dato);
        }
        else return r;

        r->altura = max(altura(r->izq), altura(r->der));
        int balance = obtenerBalance(r);

        // Left Left (LL)
        if (balance > 1 && r->izq && dato < r->izq->dato)
            return rotarDerecha(r);

        // Right Right (RR)
        if (balance < -1 && r->der && dato > r->der->dato)
            return rotarIzquierda(r);

        // Left Right (LR)
        if (balance > 1 && r->izq && dato > r->izq->dato)
        {
            r->izq = rotarIzquierda(r->izq);
            return rotarDerecha(r);
        }

        // Right Left (RL)
        if (balance < -1 && r->der && dato < r->der->dato)
        {
            r->der = rotarDerecha(r->der);
            return rotarIzquierda(r);
        }
        return r;
    }

    bool encontrarAux(NodoAVL *r, T dato)
    {
        if (!r)
            return false;
        if (r->dato == dato)
            return true;
        if (r->dato < dato)
            return encontrarAux(r->der, dato);
        return encontrarAux(r->izq, dato);
    }

    T buscarAux(NodoAVL *r, T dato)
    {
        if (!r)
            return T();
        if (r->dato == dato)
            return r->dato;
        if (r->dato < dato)
            return buscarAux(r->der, dato);
        return buscarAux(r->izq, dato);
    }

public:
    int cantidad;

    ArbolAVL()
    {
        cantidad = 0;
        raiz = NULL;
    }

    bool encontrar(T dato)
    {
        return encontrarAux(raiz, dato);
    }

    T buscar(T dato)
    {
        return buscarAux(raiz, dato);
    }

    // Adaptado de las slides del curso (https://avl.uruguayan.ninja/7)
    void insertar(T dato)
    {
        raiz = agregar(raiz, dato);
    }
};

#endif