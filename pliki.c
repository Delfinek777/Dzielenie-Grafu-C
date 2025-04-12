#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 10000

void zbuduj_macierz_sasiedztwa(const char *nazwa_pliku, int ***macierz, int *n) {
    FILE *plik = fopen(nazwa_pliku, "r");
    if (!plik) {
        printf("Nie udalo sie otworzyc pliku!\n");
        exit(1);
    }

    char linia1[MAX_LINE], linia2[MAX_LINE], linia3[MAX_LINE];
    fgets(linia1, MAX_LINE, plik); // max_sasiedzi — pomijamy
    fgets(linia2, MAX_LINE, plik); // sasiedzi
    fgets(linia3, MAX_LINE, plik); // start[]

    // Policz liczby
    int liczba_startow = 1;
    for (char *p = linia3; *p; p++) {
        if (*p == ';') liczba_startow++;
    }

    *n = liczba_startow - 1; // liczba wierzcholkow
    *macierz = (int **)calloc(*n, sizeof(int *));
    for (int i = 0; i < *n; i++) {
        (*macierz)[i] = (int *)calloc(*n, sizeof(int));
    }

    // Parsuj linie do tablic
    int *start = (int *)malloc(((*n)+1) * sizeof(int));
    int *sasiedzi = (int *)malloc(100000 * sizeof(int)); // duży bufor

    int i = 0;
    char *token = strtok(linia3, ";");
    while (token) {
        start[i++] = atoi(token);
        token = strtok(NULL, ";");
    }

    int licznik_sasiedzi = 0;
    token = strtok(linia2, ";");
    while (token) {
        sasiedzi[licznik_sasiedzi++] = atoi(token);
        token = strtok(NULL, ";");
    }

    // Twórz macierz sąsiedztwa na podstawie start[] i sasiedzi[]
    for (int v = 0; v < *n; v++) {
        for (int j = start[v]; j < start[v+1]; j++) {
            int sasiad = sasiedzi[j];
            (*macierz)[v][sasiad] = 1; // zakładamy nieskierowany graf
            (*macierz)[sasiad][v] = 1;
        }
    }

    free(start);
    free(sasiedzi);
    fclose(plik);
}
void wypisz_macierz_sasiedztwa(int **macierz, int n) {
    // Czytelne wypisanie macierzy z nagłówkami
    printf("    ");
    for (int j = 0; j < n; j++) {
        printf("%2d ", j);
    }
    printf("\n");

    for (int i = 0; i < n; i++) {
        printf("%2d: ", i);
        for (int j = 0; j < n; j++) {
            printf(" %d ", macierz[i][j]);
        }
        printf("\n");
    }
}
