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

class ConjuntosDisjuntos {
    private:
        int* padre; // vertice "raiz"
        int* rango; // para optimizar, que el rango mas chico se una al de rango mas grande (no lo tengo muy claro a esto)
        int tam;
        int cantidadConjuntos;

    public:
        ConjuntosDisjuntos(int n){
            tam = n;
            cantidadConjuntos = n;
            padre = new int[n];
            rango = new int[n];

            for(int i = 0; i < n; i++){
                padre[i] = i;
                rango[i] = 0;
            }
        }

        ~ConjuntosDisjuntos(){
            delete[] padre;
            delete[] rango;
        }

        int encontrar(int x){
            if(padre[x] != x){
                padre[x] = encontrar(padre[x]); // voy subiendo hasta llegar a la raiz. Es el padre de todo el conjunto
            }
            return padre[x];
        }

        void unir(int x, int y){
            int raizX = encontrar(x);
            int raizY = encontrar(y);

            if(raizX == raizY) return;

            if(rango[raizX] < rango[raizY]){ // para optimizar, el de rango mas chico se une al de rango mas grande
                padre[raizX] = raizY;
            } 
            else if(rango[raizX] > rango[raizY]){
                padre[raizY] = raizX;
            }
            else{
                padre[raizY] = raizX;
                rango[raizX]++;
            }
            cantidadConjuntos--;
        }

        bool mismoConjunto(int x, int y){
            return encontrar(x) == encontrar(y);
        }

        int obtenerCantidadConjuntos() {
            return cantidadConjuntos;
        }
};

class Arista {
    private:
        int peso;
        int o; // vertice de origen
        int f; // vertice de llegada;

    public:
        
        Arista() : o(-1), f(-1), peso(-1) {}
        Arista(int peso, int origen, int final): o(origen), f(final), peso(peso) {}

        int getPeso() {return peso;}
};

void intercambiarAristas(Arista& a, Arista&b){
    Arista aux = a;
    a = b;
    b = aux;
}


// El algoritmo de particion y el quicksort esta basado en la version de Lomuto presentada 
// en 'Introduction to Algorithms' (Cormen et al.), capítulo 7, página 171.

int particion(Arista aristas[], int primEl, int segEl){

    int pivot = aristas[segEl].getPeso();
    int i = primEl -1;

    for(int j = primEl; j < segEl; j++){
        if(aristas[j].getPeso() <= pivot){
            i++;
            intercambiarAristas(aristas[i], aristas[j]);
        }
    }
    intercambiarAristas(aristas[i+1], aristas[segEl]);
    return i+1;
}


void quickSort(Arista aristas[], int primEl, int segEl){
    if(primEl < segEl){
        int pi = particion(aristas, primEl, segEl); // posicion del pivote

        quickSort(aristas, primEl, pi - 1); // Elementos menores que el pivote
        quickSort(aristas, pi + 1, segEl); // Elementos mayores que el pivote
    }
}




int main()
{
    // TODO
    return 0;
}