#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dzielenie.h"
#include "pliki.h"
void dfs(int wierzcholek, int *odwiedzone, int n, int **graf)
{
    odwiedzone[wierzcholek] = 1;
    for (int i = 0; i < n; i++)
    {
        if (graf[wierzcholek][i] == 1 && odwiedzone[i] == 0)
        { // Jeśli jest krawędź i wierzchołek jeszcze nie został odwiedzona
            dfs(i, odwiedzone, n, graf);
        }
    }
}
int policz_polaczone_wierzcholki(int start, int n, int **graf)
{
    int *odwiedzone = (int *)calloc(n, sizeof(int));
    dfs(start, odwiedzone, n, graf);
    int licznik = 0;
    for (int i = 0; i < n; i++)
    {
        if (odwiedzone[i])
        {
            licznik++;
        }
    }
    free(odwiedzone);
    return licznik;
}

int policz_graf(int n, int **graf)
{
    int *odwiedzone = (int *)calloc(n, sizeof(int));
    int licznik = 0;

    for (int i = 0; i < n; i++)
    {
        if (odwiedzone[i] == 0)
        {
            licznik += 1;
            dfs(i, odwiedzone, n, graf);
        }
    }
    return licznik;
}
void przetwarzanie_grafu(FILE *plik, int ***graf, int *n)
{
    char linia[1024];
    int rozmiar = 0;
    /*1.  Trzeba ogarnąć ile wierszy ma graf podany w pliku
        Liczba wierszy powinna się równać liczbie wierzchołków w grafie
        Przyjmuje że właśnie w pliku będą nam dawać macierz sąsiadów
        Na isodzie napisali coś takiego...
        Graf do podziału zostanie przekazany w formacie tekstowym - opis pojawi się poniżej.

    2.  Wczytywanie wartości z pliku do macierzy
    */

    // 1.
    while (fgets(linia, sizeof(linia), plik))
    {
        rozmiar++;
    }
    rewind(plik);
    *n = rozmiar;
    printf("Rozmiar = %d \n", rozmiar);
    *graf = (int **)calloc(rozmiar, sizeof(int *));

    for (int i = 0; i < rozmiar; i++)
    {
        (*graf)[i] = (int *)calloc(rozmiar, sizeof(int));
    }
    // 2.
    for (int i = 0; i < rozmiar; i++)
    {
        for (int j = 0; j < rozmiar; j++)
        {
            if (fscanf(plik, "%d", &(*graf)[i][j]) != 1)
            {
                printf("Wystapil blad w odczytywaniu pliku do grafu!\n");
                return;
            }
        }
    }
}

int main(int argc, char *argv[])
{
    char nazwa_pliku[30];
    int liczba_wierzcholkow = 0;
    int margines_procentowy;
    int liczba_subgrafow;
    int liczba_przeciec = 1;
    if (argc < 3)
    {
        printf("Nie podano prawidlowych argumentow!\n");
        return 1;
    }
    if (argc >= 3)
    {
        liczba_przeciec = atoi(argv[3]);
    }
    strcpy(nazwa_pliku, argv[1]);
    FILE *plik = fopen(nazwa_pliku, "r");
    int **graf = NULL;
    margines_procentowy = atoi(argv[2]);

    przetwarzanie_grafu(plik, &graf, &liczba_wierzcholkow);
    fclose(plik);
    wypisz_macierz_sasiedztwa(graf, liczba_wierzcholkow);

    liczba_subgrafow = policz_graf(8, graf);

    printf("Liczba oddzielnych grafow wynosi %d\n", liczba_subgrafow); // Działa bez zarzutu bo jest wierzchołek 0 który nie jest z niczym połączony
    printf("Liczba wierzcholkow w grafie wynosi %d\n", liczba_wierzcholkow);
    printf("Margines procentowy wynosi %d\n", margines_procentowy);
    printf("Liczba przeciec wynosi %d\n", liczba_przeciec);
    dzielenie_grafu(graf, liczba_wierzcholkow, margines_procentowy, liczba_przeciec);

    return 0;
}
