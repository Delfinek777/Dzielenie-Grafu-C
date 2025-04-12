#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dzielenie.h"
#include <math.h>
#include <stdbool.h>

void dfs(int wierzcholek, int *odwiedzone, int n, int **graf)
{
    odwiedzone[wierzcholek] = 1;
    for (int i = 0; i < n; i++)
    {
        if (graf[wierzcholek][i] == 1 && odwiedzone[i] == 0)
        {
            dfs(i, odwiedzone, n, graf);
        }
    }
}
void print_macierz(int **macierz, int rozmiar)
{
    for (int i = 0; i < rozmiar; i++)
    {
        for (int j = 0; j < rozmiar; j++)
        {
            printf("%d ", macierz[i][j]);
        }
        printf("\n");
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
    if (argc < 4)
    {
        printf("Sposób użycia: %s <plik_z_grafem> <margines_procentowy> <docelowa_liczba_podgrafow>\n", argv[0]);
        printf("Przykład: %s graf.txt 30 3\n", argv[0]);
        return 1;
    }

    // Wczytanie parametrów
    char *nazwa_pliku = argv[1];
    int margines_procentowy = atoi(argv[2]);
    int docelowa_liczba_podgrafow = atoi(argv[3]);

    // Wczytanie grafu z pliku
    FILE *plik = fopen(nazwa_pliku, "r");
    if (!plik)
    {
        printf("Nie można otworzyć pliku %s\n", nazwa_pliku);
        return 1;
    }

    int liczba_wierzcholkow;
    int **graf = NULL;
    przetwarzanie_grafu(plik, &graf, &liczba_wierzcholkow);
    fclose(plik);

    // Wyświetlenie informacji początkowych
    printf("\nWczytany graf (%d wierzchołków):\n", liczba_wierzcholkow);
    print_macierz(graf, liczba_wierzcholkow);

    printf("\nPoczatkowa liczba spojnych skladowych: %d\n", policz_graf(liczba_wierzcholkow, graf));
    printf("Docelowa liczba podgrafow: %d\n", docelowa_liczba_podgrafow);
    printf("Dopuszczalny margines procentowy: %d%%\n", margines_procentowy);

    // Dzielenie grafu
    dzielenie_grafu(graf, liczba_wierzcholkow, margines_procentowy, docelowa_liczba_podgrafow);

    // Zwolnienie pamięci
    for (int i = 0; i < liczba_wierzcholkow; i++)
    {
        free(graf[i]);
    }
    free(graf);

    return 0;
}