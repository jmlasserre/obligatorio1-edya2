#include <cassert>
#include <string>
#include <iostream>
#include <limits>

using namespace std;

class NodoAVL
{
private:
    NodoAVL *izq;
    NodoAVL *der;
    int id;
    string nombre;
    int puntaje;

public:
    NodoAVL(int idJugador, string nombreJugador, int puntajeJugador)
    {
        izq = NULL;
        der = NULL;
        id = idJugador;
        nombre = nombreJugador;
        puntaje = puntajeJugador;
    }
    ~NodoAVL()
    {

    }
};

/*class ArbolAVL{

}
*/
    

int main()
{
    return 0;
}