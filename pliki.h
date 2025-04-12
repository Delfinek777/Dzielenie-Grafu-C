#ifndef PLIKI_H
#define PLIKI_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void zbuduj_macierz_sasiedztwa(const char *nazwa_pliku, int ***macierz, int *n);
void wypisz_macierz_sasiedztwa(int **macierz, int n);

#endif