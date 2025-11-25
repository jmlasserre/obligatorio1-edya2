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
#define MAX_VALUE numeric_limits<int>::max()

// Adaptado de: https://bt.uruguayan.ninja/ (diapositivas del curso)

bool posicionValida(int i, int j, int M, int N)
{
    return (i >= 0 && j >= 0 && i < M && j < N);
}

bool puedoAplicarMovimiento(int i, int j, bool** vis, char** fc, int M, int N, char aBuscar){
    return posicionValida(i, j, M, N) && (fc[i][j] == 'C' || fc[i][j] == aBuscar) && !vis[i][j];
}

void aplicarMovimiento(bool** vis, int i, int j, int pasosActuales, int** minDist){
    vis[i][j] = true;
    if (minDist[i][j] == -1 || pasosActuales < minDist[i][j]){
        minDist[i][j] = pasosActuales;
    }
}

void deshacerMovimiento(bool** vis, int i, int j){
    vis[i][j] = false;
}

bool esMejorSolucion(int pasosActuales, int mejoresPasos){
    return pasosActuales < mejoresPasos;
}

bool esMejorOIgualSolucion(int pasosActuales, int mejoresPasos){
    return pasosActuales <= mejoresPasos;
}

bool esSolucion(){
    return false;
}

bool puedoPodar(int pasosActuales, int mejoresPasos, int** minDist, int i, int j){
    return (pasosActuales >= mejoresPasos || (minDist[i][j] != MAX_VALUE && pasosActuales > minDist[i][j]));
} 


void buscarProductoBT(char **fc, bool **vis, int** minDist, int M, int N, char aBuscar, int i, int j, int pasosActuales, int& mejoresPasos)
{
    if (!puedoPodar(pasosActuales, mejoresPasos, minDist, i, j)){
        if ((fc[i][j] == aBuscar) && esMejorOIgualSolucion(pasosActuales, mejoresPasos)){
            if (esMejorSolucion(pasosActuales, mejoresPasos)){
                mejoresPasos = pasosActuales;
            }
        }
        else {
            int movsFila[4] = { -1, 0, 1, 0 };
            int movsCol[4] = {0, 1, 0, -1};
            for (int mov = 0; mov < 4; mov++){
                int iCand = i + movsFila[mov];
                int jCand = j + movsCol[mov];
                if (puedoAplicarMovimiento(iCand, jCand, vis, fc, M, N, aBuscar)){
                    aplicarMovimiento(vis, iCand, jCand, pasosActuales+1, minDist);
                    buscarProductoBT(fc, vis, minDist, M, N, aBuscar, iCand, jCand, pasosActuales+1, mejoresPasos);
                    deshacerMovimiento(vis, iCand, jCand);
                }
            }
        }
    }
}

int buscarProducto(char **fc, int M, int N, char aBuscar)
{
    bool **visitados = new bool *[M];
    int** minDist = new int* [M];
    for (int i = 0; i < M; i++)
    {
        visitados[i] = new bool[N]();
        minDist[i] = new int[N]();
    }
    for (int i = 0; i < M; i++){
        for (int j = 0; j < N; j++){
            minDist[i][j] = MAX_VALUE;
        }
    }
    minDist[0][0] = 0; // celda inicial
    visitados[0][0] = true;
    int mejoresPasos = MAX_VALUE;
    buscarProductoBT(fc, visitados, minDist, M, N, aBuscar, 0, 0, 0, mejoresPasos);
    for (int i = 0; i < M; i++){
        delete[] visitados[i];
        delete[] minDist[i];
    }
    delete[] visitados;
    delete[] minDist;
    return mejoresPasos;
}

void imprimir(char **matriz, int M, int N)
{
    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < N; j++)
        {
            cout << matriz[i][j] << " ";
        }
        cout << endl;
    }
}

int main()
{
    char aBuscar;
    int P, minFC, minPasos;
    minPasos = MAX_VALUE;
    minFC = 0;
    cin >> aBuscar >> P;
    for (int i = 0; i < P; i++)
    {
        int M, N;
        cin >> M >> N;
        char **fc = new char *[M];
        for (int j = 0; j < M; j++)
        {
            fc[j] = new char[N];
            for (int k = 0; k < N; k++)
            {
                char valor;
                cin >> valor;
                fc[j][k] = valor;
            }
        }
        int pasos = buscarProducto(fc, M, N, aBuscar);
        //cout << pasos << endl;
        if (pasos < minPasos)
        {
            minPasos = pasos;
            minFC = i + 1;
        }
    }
    cout << minFC << " " << minPasos << endl;
}