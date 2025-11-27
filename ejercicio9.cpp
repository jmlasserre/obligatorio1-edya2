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

int main()
{
    int N, S, L;
    cin >> N >> S >> L;
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
    for (int i = 1; i <= N; i++)
    {
        int tamanio, lineas, puntos;
        cin >> tamanio >> lineas >> puntos;
        for (int j = 1; j <= S; j++)
        {
            for (int k = 1; k <= L; k++)
            {
                if (!(tamanio <= j && lineas <= k))
                {
                    maxPuntos[i][j][k] = maxPuntos[i - 1][j][k];
                }
                else
                {
                    maxPuntos[i][j][k] = max(
                        maxPuntos[i - 1][j][k],
                        puntos + maxPuntos[i - 1][j - tamanio][k - lineas]);
                }
            }
        }
    }
    cout << maxPuntos[N][S][L] << endl;
}