#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pliki.h"
#define MAX_LINE 10000

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void zbuduj_macierz_sasiedztwa(FILE *plik, int ***macierz, int *n) {
    char line[4096]; // Bufor na linie pliku
    int max_nodes_per_row = 0;
    int *nodes = NULL;
    int *row_pointers = NULL;
    int *groups = NULL;
    int *group_pointers = NULL;
    int node_count = 0;
    int row_pointer_count = 0;
    int group_count = 0;
    int group_pointer_count = 0;

    // 1. Odczytaj maksymalną liczbę węzłów w wierszu
    if (!fgets(line, sizeof(line), plik)) {
        fprintf(stderr, "Błąd odczytu maksymalnej liczby węzłów\n");
        exit(1);
    }
    max_nodes_per_row = atoi(line);

    // 2. Odczytaj indeksy węzłów
    if (!fgets(line, sizeof(line), plik)) {
        fprintf(stderr, "Błąd odczytu listy węzłów\n");
        exit(1);
    }

    // Parsowanie listy węzłów
    char *token = strtok(line, ";");
    while (token != NULL) {
        nodes = realloc(nodes, (node_count + 1) * sizeof(int));
        nodes[node_count++] = atoi(token);
        token = strtok(NULL, ";");
    }

    // 3. Odczytaj wskaźniki na pierwsze indeksy w wierszach
    if (!fgets(line, sizeof(line), plik)) {
        fprintf(stderr, "Błąd odczytu wskaźników wierszy\n");
        exit(1);
    }

    token = strtok(line, ";");
    while (token != NULL) {
        row_pointers = realloc(row_pointers, (row_pointer_count + 1) * sizeof(int));
        row_pointers[row_pointer_count++] = atoi(token);
        token = strtok(NULL, ";");
    }

    // Liczba wierzchołków to liczba wskaźników wierszy minus 1
    *n = row_pointer_count - 1;

    // Alokacja macierzy sąsiedztwa
    *macierz = (int **)calloc(*n, sizeof(int *));
    for (int i = 0; i < *n; i++) {
        (*macierz)[i] = (int *)calloc(*n, sizeof(int));
    }

    // 4. Odczytaj grupy węzłów (krawędzie)
    while (fgets(line, sizeof(line), plik)) {
        if (strlen(line) < 2) continue; // Pomiń puste linie

        // Sprawdź czy to nowa sekcja grup
        if (strstr(line, ";") != NULL) {
            // Parsowanie grup
            token = strtok(line, ";");
            while (token != NULL) {
                groups = realloc(groups, (group_count + 1) * sizeof(int));
                groups[group_count++] = atoi(token);
                token = strtok(NULL, ";");
            }

            // Następna linia to wskaźniki grup
            if (fgets(line, sizeof(line), plik)) {
                token = strtok(line, ";");
                while (token != NULL) {
                    group_pointers = realloc(group_pointers, (group_pointer_count + 1) * sizeof(int));
                    group_pointers[group_pointer_count++] = atoi(token);
                    token = strtok(NULL, ";");
                }
            }
        }
    }

    // Budowanie macierzy sąsiedztwa na podstawie wierszy
    for (int i = 0; i < *n; i++) {
        int start = row_pointers[i];
        int end = (i < *n - 1) ? row_pointers[i + 1] : node_count;

        for (int j = start; j < end; j++) {
            int neighbor = nodes[j];
            if (neighbor >= 0 && neighbor < *n) {
                (*macierz)[i][neighbor] = 1;
                (*macierz)[neighbor][i] = 1; // Graf nieskierowany
            }
        }
    }

    // Budowanie macierzy na podstawie grup (jeśli istnieją)
    if (group_pointer_count > 0) {
        for (int i = 0; i < group_pointer_count - 1; i++) {
            int start = group_pointers[i];
            int end = group_pointers[i + 1];

            // Wszystkie węzły w grupie są połączone ze sobą
            for (int j = start; j < end; j++) {
                for (int k = start; k < end; k++) {
                    if (groups[j] >= 0 && groups[j] < *n && 
                        groups[k] >= 0 && groups[k] < *n) {
                        (*macierz)[groups[j]][groups[k]] = 1;
                    }
                }
            }
        }
    }

    // Zwolnienie pamięci tymczasowych struktur
    free(nodes);
    free(row_pointers);
    if (groups) free(groups);
    if (group_pointers) free(group_pointers);
}

void przetwarzanie_grafu(FILE *plik, int ***graf, int *n) {
    rewind(plik); // Przewiń plik na początek
    zbuduj_macierz_sasiedztwa(plik, graf, n);
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
