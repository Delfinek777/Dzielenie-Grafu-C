#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "dzielenie.h"

typedef struct {
    int *wierzcholki;
    int rozmiar;
} Podgraf;

double oblicz_margines(int a, int b) {
    return 2.0 * abs(a - b) / (a + b) * 100.0;
}

int czy_podgraf_spojny(int **graf, Podgraf *p, int n) {
    if (p->rozmiar == 0) return 1;
    
    int *odwiedzone = (int *)calloc(n, sizeof(int));
    dfs(p->wierzcholki[0], odwiedzone, n, graf);
    
    for (int i = 0; i < p->rozmiar; i++) {
        if (!odwiedzone[p->wierzcholki[i]]) {
            free(odwiedzone);
            return 0;
        }
    }
    
    free(odwiedzone);
    return 1;
}

void wypisz_przeciete_krawedzie(int **graf, Podgraf *A, Podgraf *B,FILE *output) {
    printf("\nPrzeciete krawedzie:\n");
    fprintf(output,"\nPrzeciete krawedzie:\n");
    int licznik = 0;
    
    for (int i = 0; i < A->rozmiar; i++) {
        int u = A->wierzcholki[i];
        for (int j = 0; j < B->rozmiar; j++) {
            int v = B->wierzcholki[j];
            if (graf[u][v] == 1) {
                printf("%d -- %d\n", u, v);
                fprintf(output,"%d -- %d\n", u, v);
                licznik++;
            }
        }
    }
    
    if (licznik == 0) {
        printf("Brak krawedzi do przecięcia\n");
        fprintf(output,"Brak krawedzi do przecięcia\n");
    } else {
        printf("Lacznie przecieto %d krawedzi\n", licznik);
        fprintf(output,"Lacznie przecieto %d krawedzi\n", licznik);
    }
}

void podziel_graf_iteracyjnie(int **graf, int n, int max_podgrafow, int max_margines,FILE *output) {
    Podgraf *podgrafy = (Podgraf *)calloc(max_podgrafow, sizeof(Podgraf));
    int aktualna_liczba_podgrafow = 1;
    
    // Inicjalizacja pierwszego podgrafu (cały graf)
    podgrafy[0].wierzcholki = (int *)malloc(n * sizeof(int));
    podgrafy[0].rozmiar = n;
    for (int i = 0; i < n; i++) {
        podgrafy[0].wierzcholki[i] = i;
    }

    while (aktualna_liczba_podgrafow < max_podgrafow) {
        // Znajdź największy spójny podgraf do podziału
        int najwiekszy_idx = -1;
        for (int i = 0; i < aktualna_liczba_podgrafow; i++) {
            if (podgrafy[i].rozmiar > 1 && czy_podgraf_spojny(graf, &podgrafy[i], n)) {
                if (najwiekszy_idx == -1 || podgrafy[i].rozmiar > podgrafy[najwiekszy_idx].rozmiar) {
                    najwiekszy_idx = i;
                }
            }
        }

        if (najwiekszy_idx == -1) {
            printf("Nie mozna znalezc odpowiedniego podgrafu do podzialu\n");
            fprintf(output,"Nie mozna znalezc odpowiedniego podgrafu do podzialu\n");
            break;
        }

        Podgraf *A = &podgrafy[najwiekszy_idx];
        Podgraf *B = &podgrafy[aktualna_liczba_podgrafow];
        
        // Znajdź spójny podgraf B o rozmiarze ~A->rozmiar/2
        B->wierzcholki = (int *)malloc(A->rozmiar * sizeof(int));
        B->rozmiar = 0;
        
        // Użyj BFS do znalezienia spójnego podgrafu
        int *odwiedzone = (int *)calloc(n, sizeof(int));
        int celowy_rozmiar = A->rozmiar / 2;
        int start = A->wierzcholki[0];
        
        // Kolejka do BFS
        int *kolejka = (int *)malloc(A->rozmiar * sizeof(int));
        int head = 0, tail = 0;
        
        kolejka[tail++] = start;
        odwiedzone[start] = 1;
        B->wierzcholki[B->rozmiar++] = start;
        
        while (head < tail && B->rozmiar < celowy_rozmiar) {
            int u = kolejka[head++];
            
            for (int i = 0; i < A->rozmiar; i++) {
                int v = A->wierzcholki[i];
                if (!odwiedzone[v] && graf[u][v] && B->rozmiar < celowy_rozmiar) {
                    odwiedzone[v] = 1;
                    kolejka[tail++] = v;
                    B->wierzcholki[B->rozmiar++] = v;
                }
            }
        }
        
        // Aktualizuj podgraf A
        int nowy_rozmiar_A = 0;
        for (int i = 0; i < A->rozmiar; i++) {
            if (!odwiedzone[A->wierzcholki[i]]) {
                A->wierzcholki[nowy_rozmiar_A++] = A->wierzcholki[i];
            }
        }
        A->rozmiar = nowy_rozmiar_A;
        
        free(kolejka);
        free(odwiedzone);
        
        // Sprawdź warunki
        if (B->rozmiar == 0 || !czy_podgraf_spojny(graf, B, n)) {
            printf("Nie udalo sie znalezc odpowiedniego podgrafu do podzialu\n");
            fprintf(output,"Nie udalo sie znalezc odpowiedniego podgrafu do podzialu\n");
            free(B->wierzcholki);
            continue;
        }
        
        double margines = oblicz_margines(A->rozmiar, B->rozmiar);
        if (margines > max_margines) {
            printf("Margines %.2f%% przekracza dopuszczalny %d%%\n", margines, max_margines);
            fprintf(output,"Margines %.2f%% przekracza dopuszczalny %d%%\n", margines, max_margines);
            free(B->wierzcholki);
            continue;
        }
        
        // Wypisz informacje o przeciętych krawędziach
        wypisz_przeciete_krawedzie(graf, A, B,output);
        
        // Odłącz krawędzie między podgrafami
        for (int i = 0; i < A->rozmiar; i++) {
            for (int j = 0; j < B->rozmiar; j++) {
                int u = A->wierzcholki[i];
                int v = B->wierzcholki[j];
                if (graf[u][v] == 1) {
                    graf[u][v] = graf[v][u] = 0;
                }
            }
        }
        
        printf("Podzielono podgraf %d na:\n", najwiekszy_idx);
        printf("- Podgraf %d: %d wierzcholkow\n", najwiekszy_idx, A->rozmiar);
        printf("- Podgraf %d: %d wierzcholkow\n", aktualna_liczba_podgrafow, B->rozmiar);
        printf("Margines: %.2f%%\n", margines);
        
        fprintf(output,"Podzielono podgraf %d na:\n", najwiekszy_idx);
        fprintf(output,"- Podgraf %d: %d wierzcholkow\n", najwiekszy_idx, A->rozmiar);
        fprintf(output,"- Podgraf %d: %d wierzcholkow\n", aktualna_liczba_podgrafow, B->rozmiar);
        fprintf(output,"Margines: %.2f%%\n", margines);



        aktualna_liczba_podgrafow++;
    }
    
    // Sprawdź spójność ostatecznych podgrafów
    printf("\nOstateczny podzial:\n");
    fprintf(output,"\nOstateczny podzial:\n");
    for (int i = 0; i < aktualna_liczba_podgrafow; i++) {
        printf("Podgraf %d (%d wierzcholkow): ", i, podgrafy[i].rozmiar);
        fprintf(output,"Podgraf %d (%d wierzcholkow): ", i, podgrafy[i].rozmiar);
        for (int j = 0; j < podgrafy[i].rozmiar; j++) {
            printf("%d ", podgrafy[i].wierzcholki[j]);
            fprintf(output,"%d ", podgrafy[i].wierzcholki[j]);
        }
        printf(czy_podgraf_spojny(graf, &podgrafy[i], n) ? "[spojny]\n" : "[NIESPOJNY]\n");
        fprintf(output,czy_podgraf_spojny(graf, &podgrafy[i], n) ? "[spojny]\n" : "[NIESPOJNY]\n");
    }
    
    // Zwolnij pamięć
    for (int i = 0; i < aktualna_liczba_podgrafow; i++) {
        free(podgrafy[i].wierzcholki);
    }
    free(podgrafy);
}

void dzielenie_grafu(int **graf, int n, int margines_procentowy, int docelowa_liczba_podgrafow,FILE *output) {
    printf("\nRozpoczynanie dzielenia grafu na %d czesci z marginesem %d%%\n", 
           docelowa_liczba_podgrafow, margines_procentowy);
    fprintf(output,"Rozpoczynanie dzielenia grafu na %d czesci z marginesem %d%%\n", 
           docelowa_liczba_podgrafow, margines_procentowy);
    
    podziel_graf_iteracyjnie(graf, n, docelowa_liczba_podgrafow, margines_procentowy,output);
}