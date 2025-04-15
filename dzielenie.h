#ifndef DZIELENIE_H
#define DZIELENIE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void dfs(int wierzcholek, int *odwiedzone, int n, int **graf);
int policz_polaczone_wierzcholki(int start, int n, int **graf);
int policz_graf(int n, int **graf);
void przetwarzanie_grafu(FILE *plik, int ***graf, int *n);
void print_macierz(int **macierz, int rozmiar);
void dzielenie_grafu(int **graf, int n, int margines_procentowy, int docelowa_liczba_podgrafow,FILE *output);

#endif