#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int czy_polaczone(int i,int j, int **graf){
    return graf[i][j]==1;
}
void dfs_skladowe(int wierzcholek, int *ktora_skladowa, int **graf, int n, int nr_skladowej)
{
    ktora_skladowa[wierzcholek] = nr_skladowej;
    for (int i = 0; i < n; i++)
    {
        if (graf[wierzcholek][i] == 1 && ktora_skladowa[i] == -1)
        {
            dfs_skladowe(i, ktora_skladowa, graf, n, nr_skladowej);
        }
    }
}
int *znajdz_spojne_skladowe(int **graf, int n)
{
    int *ktora_skladowa = malloc(n * sizeof(int));
    memset(ktora_skladowa, -1, n * sizeof(int));
    int nr_skladowej = 1;

    for (int i = 0; i < n; i++)
    {
        if (ktora_skladowa[i] == -1)
        {
            dfs_skladowe(i, ktora_skladowa, graf, n, nr_skladowej);
            nr_skladowej++;
        }
    }
    return ktora_skladowa;
}
int ile_spojnych_skladowych(int **graf, int n)
{
    int *ktora_skladowa = malloc(n * sizeof(int));
    memset(ktora_skladowa, -1, n * sizeof(int));
    int ile_skladowych = 1;

    for (int i = 0; i < n; i++)
    {
        if (ktora_skladowa[i] == -1)
        {
            dfs_skladowe(i, ktora_skladowa, graf, n, ile_skladowych);
            ile_skladowych++;
        }
    }
    free(ktora_skladowa);
    return ile_skladowych;
}
int skladowe_po_usunieciu(int **graf, int n, int i, int j)
{
    int **pomocniczy_graf = malloc(n * sizeof(int *));
    for (int k = 0; k < n; k++) {
        pomocniczy_graf[k] = malloc(n * sizeof(int));
        memcpy(pomocniczy_graf[k], graf[k], n * sizeof(int));
    }
    pomocniczy_graf[i][j] = 0;
    pomocniczy_graf[j][i] = 0;
    int wynik = ile_spojnych_skladowych(pomocniczy_graf, n);
    for (int k = 0; k < n; k++) {
        free(pomocniczy_graf[k]);
    }
    free(pomocniczy_graf);
    return wynik;
}
void znajdowanie_mostow(int **graf, int n)
{   
    int skladowe_przed_usunieciem=ile_spojnych_skladowych(graf,n);
    for (int i=0;i<n;i++){
        for(int j=i+1;j<n;j++){
            if (czy_polaczone(i,j,graf)){
                int skladowe_po=skladowe_po_usunieciu(graf,n,i,j);
                if (skladowe_przed_usunieciem+1==skladowe_po){
                    printf("mostem jest polaczenie miedzy %d i %d\n", i, j);
                }
            }
        }
    }
}