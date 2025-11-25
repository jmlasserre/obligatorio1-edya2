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

/* Fuentes: 
- LeetCode (https://leetcode.com/problems/remove-boxes/solutions/1402561/cjavapython-top-down-dp-clear-explanatio-0fxk/)
- AlgoMonster (https://algo.monster/liteproblems/546)
*/

int dp(int *cristales, int l, int r, int k, int ***memo)
{
    if (l > r)
        return 0;
    if (memo[l][r][k] > 0)
        return memo[l][r][k];
    int lOriginal = l, kOriginal = k;

    while (l + 1 <= r && cristales[l] == cristales[l + 1])
    {
        l += 1;
        k += 1;
    }
    int res = (k + 1) * (k + 1) + dp(cristales, l + 1, r, 0, memo);
    for (int m = l + 1; m <= r; ++m)
        if (cristales[m] == cristales[l])
            res = max(res, dp(cristales, m, r, k + 1, memo) + dp(cristales, l + 1, m - 1, 0, memo));
    return memo[lOriginal][r][kOriginal] = res;
}

int eliminarCristales(int *&cristales, int largo)
{
    int ***memo = new int **[largo];
    for (int i = 0; i < largo; i++) // inicializar matriz
    {
        memo[i] = new int *[largo];
        for (int j = 0; j < largo; j++)
        {
            memo[i][j] = new int[largo];
            for (int k = 0; k < largo; k++)
            {
                memo[i][j][k] = 0;
            }
        }
    }
    return dp(cristales, 0, largo - 1, 0, memo);
}

int main()
{
    int N;
    cin >> N;
    int *cristales = new int[N];
    for (int i = 0; i < N; i++)
    {
        int c;
        cin >> c;
        cristales[i] = c;
    }
    cout << eliminarCristales(cristales, N) << endl;
    return 0;
}