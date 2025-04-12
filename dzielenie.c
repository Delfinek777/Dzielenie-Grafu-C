#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dzielenie.h"

int policz_wierzcholki_subgrafu(int n, int **graf, int poczatek)
{
    int *odwiedzone = (int *)calloc(n, sizeof(int));
    int licznik = 0;
    dfs(poczatek, odwiedzone, n, graf);
    for (int i = 0; i < n; i++)
    {
        if (odwiedzone[i] == 1)
        {
            licznik++;
        }
    }
    free(odwiedzone);
    return licznik;
}

int czy_polaczone(int i, int j, int **graf)
{
    return graf[i][j] == 1;
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

int ile_spojnych_skladowych(int **graf, int n)
{
    int *ktora_skladowa = (int *)malloc(n * sizeof(int));
    memset(ktora_skladowa, -1, n * sizeof(int));
    int ile_skladowych = 0;
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

// Funkcja rekurencyjna do sprawdzania kombinacji usunięć
void sprawdz_kombinacje_usuniec(int **graf, int n, int margines_procentowy, int liczba_przeciec, int aktualne_przeciecia, int start, int *znaleziono, int **usuniete_krawedzie)
{
    if (aktualne_przeciecia == liczba_przeciec)
    {
        // Oblicz liczbę spójnych składowych po usunięciu krawędzi
        int liczba_skladowych = ile_spojnych_skladowych(graf, n);

        // Jeśli liczba spójnych składowych jest większa niż 1, sprawdź wszystkie pary subgrafów
        if (liczba_skladowych > 1)
        {
            // Tablica do przechowywania liczby wierzchołków w każdej składowej
            int *wielkosci_skladowych = (int *)malloc(liczba_skladowych * sizeof(int));
            memset(wielkosci_skladowych, 0, liczba_skladowych * sizeof(int));

            // Oblicz liczbę wierzchołków w każdej składowej
            int *ktora_skladowa = (int *)malloc(n * sizeof(int));
            memset(ktora_skladowa, -1, n * sizeof(int));
            int nr_skladowej = 0;
            for (int k = 0; k < n; k++)
            {
                if (ktora_skladowa[k] == -1)
                {
                    dfs_skladowe(k, ktora_skladowa, graf, n, nr_skladowej);
                    wielkosci_skladowych[nr_skladowej] = policz_wierzcholki_subgrafu(n, graf, k);
                    nr_skladowej++;
                }
            }

            // Sprawdź wszystkie pary subgrafów
            int warunek_spelniony = 0;
            for (int a = 0; a < liczba_skladowych; a++)
            {
                for (int b = a + 1; b < liczba_skladowych; b++)
                {
                    double margines = (2.0 * abs(wielkosci_skladowych[a] - wielkosci_skladowych[b])) / 
                                     (wielkosci_skladowych[a] + wielkosci_skladowych[b]) * 100.0;
                    
                    if (margines <= margines_procentowy)
                    {
                        warunek_spelniony = 1;
                        break;
                    }
                }
                if (warunek_spelniony)
                    break;
            }

            // Jeśli warunek jest spełniony, wypisz informację i przerwij rekurencję
            if (warunek_spelniony)
            {
                printf("Usunieto krawedzie: ");
                for (int x = 0; x < liczba_przeciec; x++)
                {
                    printf("%d-%d ", usuniete_krawedzie[x][0], usuniete_krawedzie[x][1]);
                }
                printf(", nowy graf spelnia warunki.\n");
                *znaleziono = 1; // Oznacz, że znaleziono rozwiązanie
            }

            // Zwolnij pamięć
            free(wielkosci_skladowych);
            free(ktora_skladowa);
        }
        return;
    }

    for (int i = start; i < n; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            if (graf[i][j] == 1)
            {
                // Usuń krawędź
                graf[i][j] = 0;
                graf[j][i] = 0;

                // Zapisz usuniętą krawędź
                usuniete_krawedzie[aktualne_przeciecia][0] = i;
                usuniete_krawedzie[aktualne_przeciecia][1] = j;

                // Rekurencyjnie sprawdź kolejne kombinacje usunięć
                sprawdz_kombinacje_usuniec(graf, n, margines_procentowy, liczba_przeciec, aktualne_przeciecia + 1, i, znaleziono, usuniete_krawedzie);

                // Przywróć krawędź
                graf[i][j] = 1;
                graf[j][i] = 1;

                // Jeśli znaleziono rozwiązanie, przerwij pętlę
                if (*znaleziono)
                    return;
            }
        }
    }
}

void dzielenie_grafu(int **graf, int n, int margines_procentowy, int max_przeciec)
{
    int znaleziono = 0; // Flaga oznaczająca, czy znaleziono rozwiązanie

    // Tablica do przechowywania usuniętych krawędzi
    int **usuniete_krawedzie = (int **)malloc(max_przeciec * sizeof(int *));
    for (int i = 0; i < max_przeciec; i++)
    {
        usuniete_krawedzie[i] = (int *)malloc(2 * sizeof(int));
    }

    // Próbuj znaleźć rozwiązanie z minimalną liczbą przecięć
    for (int liczba_przeciec = 1; liczba_przeciec <= max_przeciec; liczba_przeciec++)
    {
        sprawdz_kombinacje_usuniec(graf, n, margines_procentowy, liczba_przeciec, 0, 0, &znaleziono, usuniete_krawedzie);
        if (znaleziono)
            break; // Znaleziono rozwiązanie, przerwij pętlę
    }

    // Jeśli nie znaleziono rozwiązania
    if (!znaleziono)
    {
        printf("Nie udalo sie znalezc rozwiazania nawet po usunieciu %d krawedzi.\n", max_przeciec);
    }

    // Zwolnij pamięć
    for (int i = 0; i < max_przeciec; i++)
    {
        free(usuniete_krawedzie[i]);
    }
    free(usuniete_krawedzie);
}

