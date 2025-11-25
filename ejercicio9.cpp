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

const int INT_MIN = numeric_limits<int>::min();

class Archivo
{
private:
    int tamanio, lineas, puntos;

public:
    Archivo() : tamanio(0), lineas(0), puntos(0) {}
    Archivo(int t, int l, int p) : tamanio(t), lineas(l), puntos(p) {}
    int getTamanio() { return this->tamanio; }
    int getLineas() { return this->lineas; }
    int getPuntos() { return this->puntos; }
};

/* Fuentes consultadas:
- G4G (https://www.geeksforgeeks.org/dsa/0-1-knapsack-problem-dp-10/)
- AlgoMonster (https://algo.monster/problems/knapsack_intro)
- DeepSeek:
    Prompt: "Ayúdame a desarrollar la intuición detrás de un knapsack 0/1 con dos restricciones (un peso y un volumen)
    con tabulación en C++, no hagas el código pero dame algunas ideas de cómo poder razonarlo y verlo"
*/

void imprimir(int*** coso, int N, int S, int L){
    for (int i = 0; i < N; i++){
        for (int j = 0; j < S; j++){
            for (int k = 0; k < L; k++){
                cout << coso[i][j][k] << " ";
            }
        }
        cout << endl;
    }
}

int maxPuntos(Archivo *&archivos, int N, int S, int L)
{
    int ***maxPuntos = new int **[N + 1];
    for (int i = 0; i <= N; i++)
    {
        maxPuntos[i] = new int *[S + 1];
        for (int j = 0; j <= S; j++)
        {
            maxPuntos[i][j] = new int[L + 1];
            for (int k = 0; k <= L; k++)
            {
                maxPuntos[i][j][k] = 0;
            }
        }
    }
    
    for (int i = 1; i <= N; i++){
        Archivo a = archivos[i];
        int pesoArchivo = a.getTamanio();
        int lineasArchivo = a.getLineas();
        int puntos = a.getPuntos();
        for (int j = 1; j <= S; j++){
            for (int k = 1; k <= L; k++){
                if (!(pesoArchivo <= j && lineasArchivo <= k)){ 
                    maxPuntos[i][j][k] = maxPuntos[i-1][j][k];
                } else {
                    maxPuntos[i][j][k] = max(
                        maxPuntos[i-1][j][k],
                        puntos + maxPuntos[i-1][j-pesoArchivo][k-lineasArchivo]
                    );
                }
            }
        }
    }
    //imprimir(maxPuntos, N+1, S+1, L+1);
    return maxPuntos[N][S][L];
}

int main()
{
    int N, S, L;
    cin >> N >> S >> L;
    Archivo *archivos = new Archivo[N+1];
    for (int i = 1; i <= N; i++)
    {
        int t, l, p;
        cin >> t >> l >> p;
        archivos[i] = Archivo(t, l, p);
    }
    cout << maxPuntos(archivos, N, S, L) << endl;
}