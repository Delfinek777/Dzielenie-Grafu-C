#ifndef DZIELENIE_H
#define DZIELENIE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
typedef struct {
    int *wierzcholki;
    int rozmiar;
} Podgraf;

typedef struct {
    int u, v;
} Krawedz;

void dfs(int wierzcholek, int *odwiedzone, int n, int **graf);
int policz_polaczone_wierzcholki(int start, int n, int **graf);
int policz_graf(int n, int **graf);
void przetwarzanie_grafu(FILE *plik, int ***graf, int *n);
void print_macierz(int **macierz, int rozmiar);
void dzielenie_grafu(int **graf, int n, int margines_procentowy, int docelowa_liczba_podgrafow, FILE *output, int czy_binarny);
double oblicz_margines(int a, int b);
int czy_podgraf_spojny(int **graf, Podgraf *p, int n);
int znajdz_spojna_skladowa(int **graf, int n, int start, int *wierzcholki, int docelowy_rozmiar, int *odwiedzone);
void zwolnij_graf(int **graf, int n);
void wypisz_przeciete_krawedzie(Krawedz *krawedzie, int liczba_krawedzi, FILE *output, int czy_binarny);
int podziel_podgraf(int **graf, Podgraf *rodzic, Podgraf *dziecko, int n, int max_margines, FILE *output, int czy_binarny);

#endif
