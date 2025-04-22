#include "dzielenie.h"
#include "pliki.h"
#include <stdio.h>
#include <assert.h>

// Pomocnicza funkcja do tworzenia grafów testowych
int** stworz_testowy_graf(int n) {
    int** graf = (int**)calloc(n, sizeof(int*));
    for (int i = 0; i < n; i++) {
        graf[i] = (int*)calloc(n, sizeof(int));
    }
    return graf;
}

// Testy
void test_oblicz_margines() {
    printf("Test oblicz_margines... ");
    assert(oblicz_margines(10, 10) == 0.0); // Margines dla równych wartości
    assert(oblicz_margines(10, 5) >= 66.0); // Margines dla różnych wartości
    printf("OK\n");
}

void test_czy_podgraf_spojny() {
    printf("Test czy_podgraf_spojny... ");
    
    int** graf = stworz_testowy_graf(3);
    graf[0][1] = graf[1][0] = 1;  // Krawędź 0-1
    graf[1][2] = graf[2][1] = 1;  // Krawędź 1-2
    
    Podgraf p = {.wierzcholki = (int[]){0, 1, 2}, .rozmiar = 3};
    assert(czy_podgraf_spojny(graf, &p, 3) == 1); // Graf spójny
    
    zwolnij_graf(graf, 3); // Zwolnienie pamięci
    printf("OK\n");
}

void test_przypadki_brzegowe() {
    printf("Test przypadków brzegowych... ");
    
    // Graf o rozmiarze 1
    int** graf = stworz_testowy_graf(1);
    Podgraf p = {.wierzcholki = (int[]){0}, .rozmiar = 1};
    assert(czy_podgraf_spojny(graf, &p, 1) == 1); // Graf z jednym wierzchołkiem jest spójny
    
    zwolnij_graf(graf, 1); // Zwolnienie pamięci
    printf("OK\n");
}

// Główna funkcja uruchamiająca testy
int main() {
    printf("\n=== Rozpoczynam testy ===\n");
    test_oblicz_margines();
    test_czy_podgraf_spojny();
    test_przypadki_brzegowe();
    printf("\n=== Wszystkie testy zaliczone ===\n");
    return 0;
}