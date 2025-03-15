#ifndef GRAF_H
#define GRAF_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int czy_polaczone(int i, int j, int **graf);
void dfs_skladowe(int wierzcholek, int *ktora_skladowa, int **graf, int n, int nr_skladowej);
int *znajdz_spojne_skladowe(int **graf, int n);
int ile_spojnych_skladowych(int **graf, int n);
int skladowe_po_usunieciu(int **graf, int n, int i, int j);
void znajdowanie_mostow(int **graf, int n);

#endif