#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "dzielenie.h"

// DFS
static void dfs(int wierzcholek, int *odwiedzone, int n, int **graf) {
    odwiedzone[wierzcholek] = 1;
    for (int i = 0; i < n; i++) {
        if (graf[wierzcholek][i] && !odwiedzone[i]) {
            dfs(i, odwiedzone, n, graf);
        }
    }
}

// DFS z numerowaniem składowych
static void dfs_skladowe_numerowanie(int wierzcholek, int *ktora_skladowa, int **graf, int n, int nr_skladowej) {
    ktora_skladowa[wierzcholek] = nr_skladowej;
    for (int i = 0; i < n; i++) {
        if (graf[wierzcholek][i] && ktora_skladowa[i] == -1) {
            dfs_skladowe_numerowanie(i, ktora_skladowa, graf, n, nr_skladowej);
        }
    }
}

// Znajduje składowe spójne
static int znajdz_skladowe(int **graf, int n, int *ktora_skladowa) {
    memset(ktora_skladowa, -1, n * sizeof(int));
    int skladowe = 0;
    for (int i = 0; i < n; i++) {
        if (ktora_skladowa[i] == -1) {
            dfs_skladowe_numerowanie(i, ktora_skladowa, graf, n, skladowe++);
        }
    }
    return skladowe;
}

// Liczy wielkości składowych
static void oblicz_wielkosci(int *ktora_skladowa, int n, int *wielkosci, int liczba_skladowych) {
    memset(wielkosci, 0, liczba_skladowych * sizeof(int));
    for (int i = 0; i < n; i++) {
        if (ktora_skladowa[i] != -1) {
            wielkosci[ktora_skladowa[i]]++;
        }
    }
}

// Tworzy podgraf danej składowej
static int** stworz_podgraf(int **graf, int n, int *ktora_skladowa, int nr_skladowej, int *rozmiar, int *mapowanie) {
    *rozmiar = 0;
    for (int i = 0; i < n; i++) {
        if (ktora_skladowa[i] == nr_skladowej) {
            mapowanie[(*rozmiar)++] = i;
        }
    }

    int **podgraf = (int **)malloc((*rozmiar) * sizeof(int *));
    for (int i = 0; i < *rozmiar; i++) {
        podgraf[i] = (int *)malloc((*rozmiar) * sizeof(int));
        for (int j = 0; j < *rozmiar; j++) {
            podgraf[i][j] = graf[mapowanie[i]][mapowanie[j]];
        }
    }
    return podgraf;
}

// Liczy liczbę spójnych składowych
int ile_spojnych_skladowych(int **graf, int n) {
    int *odwiedzone = (int *)calloc(n, sizeof(int));
    int licznik = 0;
    for (int i = 0; i < n; i++) {
        if (!odwiedzone[i]) {
            dfs(i, odwiedzone, n, graf);
            licznik++;
        }
    }
    free(odwiedzone);
    return licznik;
}

// Główna funkcja rekurencyjna
void dzielenie_grafu_rekurencyjne(int **graf, int n, int margines_procentowy, int docelowa_liczba_subgrafow, int aktualna_liczba_subgrafow) {
    if (aktualna_liczba_subgrafow >= docelowa_liczba_subgrafow) {
        printf("\n Reached target subgraphs count: %d\n", aktualna_liczba_subgrafow);
        return;
    }

    int *ktora_skladowa = (int *)malloc(n * sizeof(int));
    int liczba_skladowych = znajdz_skladowe(graf, n, ktora_skladowa);

    if (liczba_skladowych >= docelowa_liczba_subgrafow) {
        printf("\n Cannot further divide the graph to meet the margin requirement.\n");
        free(ktora_skladowa);
        return;
    }

    int *wielkosci = (int *)calloc(liczba_skladowych, sizeof(int));
    oblicz_wielkosci(ktora_skladowa, n, wielkosci, liczba_skladowych);

    int podzial_mozliwy = 0;
    for (int k = 0; k < liczba_skladowych; k++) {
        int rozmiar_podgrafu;
        int *mapowanie = (int *)malloc(wielkosci[k] * sizeof(int));
        int **podgraf = stworz_podgraf(graf, n, ktora_skladowa, k, &rozmiar_podgrafu, mapowanie);

        int max_edges_to_remove = 4;
        int *best_edges_i = NULL, *best_edges_j = NULL;
        int best_edge_count = 0;
        double best_margin = INFINITY;
        int best_size1 = 0, best_size2 = 0;

        void znajdz_zestaw(int start, int remaining, int *ei, int *ej, int count) {
            if (remaining == 0) {
                int **temp = (int **)malloc(rozmiar_podgrafu * sizeof(int *));
                for (int i = 0; i < rozmiar_podgrafu; i++) {
                    temp[i] = (int *)malloc(rozmiar_podgrafu * sizeof(int));
                    memcpy(temp[i], podgraf[i], rozmiar_podgrafu * sizeof(int));
                }

                for (int i = 0; i < count; i++) {
                    temp[ei[i]][ej[i]] = temp[ej[i]][ei[i]] = 0;
                }

                int *comp_ids = (int *)malloc(rozmiar_podgrafu * sizeof(int));
                int comps = znajdz_skladowe(temp, rozmiar_podgrafu, comp_ids);

                if (comps == 2) {
                    int s1 = 0, s2 = 0;
                    for (int i = 0; i < rozmiar_podgrafu; i++) {
                        (comp_ids[i] == 0) ? s1++ : s2++;
                    }
                    double margin = (2.0 * abs(s1 - s2)) / (s1 + s2) * 100.0;
                    if (margin <= margines_procentowy && (count < best_edge_count || best_edge_count == 0 || margin < best_margin)) {
                        best_edge_count = count;
                        best_margin = margin;
                        best_size1 = s1;
                        best_size2 = s2;
                        free(best_edges_i); free(best_edges_j);
                        best_edges_i = (int *)malloc(count * sizeof(int));
                        best_edges_j = (int *)malloc(count * sizeof(int));
                        memcpy(best_edges_i, ei, count * sizeof(int));
                        memcpy(best_edges_j, ej, count * sizeof(int));
                        podzial_mozliwy = 1;
                    }
                }
                free(comp_ids);
                for (int i = 0; i < rozmiar_podgrafu; i++) free(temp[i]);
                free(temp);
                return;
            }

            for (int i = start; i < rozmiar_podgrafu; i++) {
                for (int j = i + 1; j < rozmiar_podgrafu; j++) {
                    if (podgraf[i][j]) {
                        ei[count] = i; ej[count] = j;
                        znajdz_zestaw(i + 1, remaining - 1, ei, ej, count + 1);
                    }
                }
            }
        }

        for (int e = 1; e <= max_edges_to_remove; e++) {
            int *ei = (int *)malloc(e * sizeof(int));
            int *ej = (int *)malloc(e * sizeof(int));
            znajdz_zestaw(0, e, ei, ej, 0);
            free(ei); free(ej);
            if (podzial_mozliwy) break;
        }

        if (podzial_mozliwy) {
            printf("\n Component split (size: %d) -> [%d, %d] (margin: %.2f%%), removed edges:\n",
                   wielkosci[k], best_size1, best_size2, best_margin);
            for (int i = 0; i < best_edge_count; i++) {
                int orig_i = mapowanie[best_edges_i[i]];
                int orig_j = mapowanie[best_edges_j[i]];
                graf[orig_i][orig_j] = graf[orig_j][orig_i] = 0;
                printf(" %d-%d", orig_i, orig_j);
            }
            printf("\n Current subgraphs count: %d\n", aktualna_liczba_subgrafow + 1);
            free(best_edges_i); free(best_edges_j);
            free(mapowanie); free(ktora_skladowa); free(wielkosci);
            dzielenie_grafu_rekurencyjne(graf, n, margines_procentowy, docelowa_liczba_subgrafow, aktualna_liczba_subgrafow + 1);
            return;
        }

        free(mapowanie);
    }

    printf("\n Unable to split any component within the margin %.2f%%\n",
           (double)margines_procentowy);
    free(ktora_skladowa);
    free(wielkosci);
}

// Drukuje macierze sąsiedztwa końcowych subgrafów
void drukuj_macierze_subgrafow(int **graf, int n) {
    int *ktora_skladowa = (int *)malloc(n * sizeof(int));
    int liczba = znajdz_skladowe(graf, n, ktora_skladowa);
    printf("\n Final number of subgraphs: %d\n", liczba);

    for (int k = 0; k < liczba; k++) {
        int rozmiar;
        int *mapa = (int *)malloc(n * sizeof(int));
        int **podgraf = stworz_podgraf(graf, n, ktora_skladowa, k, &rozmiar, mapa);
        printf("\nSubgraph #%d (size: %d):\n", k + 1, rozmiar);
        for (int i = 0; i < rozmiar; i++) {
            for (int j = 0; j < rozmiar; j++) {
                printf("%d ", podgraf[i][j]);
            }
            printf("\n");
            free(podgraf[i]);
        }
        free(podgraf); free(mapa);
    }

    free(ktora_skladowa);
}

// Start dzielenia
void dzielenie_grafu(int **graf, int n, int margines_procentowy, int docelowa_liczba_subgrafow) {
    printf("\n Start graph division (target: %d subgraphs, margin: %d%%)\n", docelowa_liczba_subgrafow, margines_procentowy);
    int aktualna = ile_spojnych_skladowych(graf, n);
    printf(" Initial number of subgraphs: %d\n", aktualna);
    if (aktualna >= docelowa_liczba_subgrafow) {
        printf(" The graph already contains %d or more subgraphs.\n", docelowa_liczba_subgrafow);
        return;
    }
    dzielenie_grafu_rekurencyjne(graf, n, margines_procentowy, docelowa_liczba_subgrafow, aktualna);
    drukuj_macierze_subgrafow(graf, n);
}
