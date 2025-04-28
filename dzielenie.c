#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "dzielenie.h"

void dfs(int wierzcholek, int *odwiedzone, int n, int **graf)
{
    odwiedzone[wierzcholek] = 1;
    for (int i = 0; i < n; i++)
    {
        if (graf[wierzcholek][i] == 1 && odwiedzone[i] == 0)
        {
            dfs(i, odwiedzone, n, graf);
        }
    }
}
double oblicz_margines(int a, int b) {
    if (a + b == 0) return 0.0; // Avoid division by zero
    return 2.0 * abs(a - b) / (a + b) * 100.0;
}

int czy_podgraf_spojny(int **graf, Podgraf *p, int n) {
    if (p->rozmiar <= 1) return 1; // Single vertex or empty is connected
    
    int *odwiedzone = (int *)calloc(n, sizeof(int));
    if (!odwiedzone) {
        fprintf(stderr, "Blad alokacji pamieci w czy_podgraf_spojny\n");
        exit(EXIT_FAILURE);
    }
    
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

void wypisz_przeciete_krawedzie(Krawedz *krawedzie, int liczba_krawedzi, FILE *output) {
    printf("\nPrzeciete krawedzie:\n");
    fprintf(output, "\nPrzeciete krawedzie:\n");
    
    if (liczba_krawedzi == 0) {
        printf("Brak krawedzi do przeciecia\n");
        fprintf(output, "Brak krawedzi do przeciecia\n");
    } else {
        for (int i = 0; i < liczba_krawedzi; i++) {
            printf("%d -- %d\n", krawedzie[i].u, krawedzie[i].v);
            fprintf(output, "%d -- %d\n", krawedzie[i].u, krawedzie[i].v);
        }
        printf("Lacznie przecieto %d krawedzi\n", liczba_krawedzi);
        fprintf(output, "Lacznie przecieto %d krawedzi\n", liczba_krawedzi);
    }
}

int znajdz_spojna_skladowa(int **graf, int n, int start, int *wierzcholki, int docelowy_rozmiar, int *odwiedzone) {
    int *kolejka = (int *)malloc(n * sizeof(int));
    if (!kolejka) {
        fprintf(stderr, "Blad alokacji pamieci w znajdz_spojna_skladowa\n");
        exit(EXIT_FAILURE);
    }
    
    int head = 0, tail = 0;
    int licznik = 0;

    kolejka[tail++] = start;
    odwiedzone[start] = 1;
    wierzcholki[licznik++] = start;

    while (head < tail && licznik < docelowy_rozmiar) {
        int u = kolejka[head++];
        
        for (int v = 0; v < n; v++) {
            if (graf[u][v] && !odwiedzone[v] && licznik < docelowy_rozmiar) {
                odwiedzone[v] = 1;
                kolejka[tail++] = v;
                wierzcholki[licznik++] = v;
            }
        }
    }

    free(kolejka);
    return licznik;
}

int **kopiuj_graf(int **graf, int n) {
    int **kopia = (int **)calloc(n, sizeof(int *));
    if (!kopia) {
        fprintf(stderr, "Blad alokacji pamieci dla kopii grafu\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < n; i++) {
        kopia[i] = (int *)calloc(n, sizeof(int));
        if (!kopia[i]) {
            fprintf(stderr, "Blad alokacji pamieci dla kopii grafu\n");
            exit(EXIT_FAILURE);
        }
        memcpy(kopia[i], graf[i], n * sizeof(int));
    }
    return kopia;
}

void zwolnij_graf(int **graf, int n) {
    for (int i = 0; i < n; i++) {
        free(graf[i]);
    }
    free(graf);
}

int podziel_podgraf(int **graf, Podgraf *rodzic, Podgraf *dziecko, int n, int max_margines, FILE *output) {
    if (rodzic->rozmiar < 2) {
        return 0; // Cannot split a subgraph with fewer than 2 vertices
    }

    int *odwiedzone = (int *)calloc(n, sizeof(int));
    if (!odwiedzone) {
        fprintf(stderr, "Blad alokacji pamieci w podziel_podgraf\n");
        exit(EXIT_FAILURE);
    }
    
    // Initialize child subgraph
    dziecko->wierzcholki = (int *)malloc(rodzic->rozmiar * sizeof(int));
    if (!dziecko->wierzcholki) {
        fprintf(stderr, "Blad alokacji pamieci dla wierzcholkow dziecka\n");
        free(odwiedzone);
        exit(EXIT_FAILURE);
    }
    dziecko->rozmiar = 0;
    
    // Copy the graph to simulate edge cuts
    int **graf_kopia = kopiuj_graf(graf, n);
    
    // Store cut edges
    Krawedz *krawedzie = (Krawedz *)malloc(rodzic->rozmiar * rodzic->rozmiar * sizeof(Krawedz));
    if (!krawedzie) {
        fprintf(stderr, "Blad alokacji pamieci dla krawedzi\n");
        free(dziecko->wierzcholki);
        zwolnij_graf(graf_kopia, n);
        free(odwiedzone);
        exit(EXIT_FAILURE);
    }
    
    // Try different starting vertices and split sizes
    for (int i = 0; i < rodzic->rozmiar; i++) {
        int start_vertex = rodzic->wierzcholki[i];
        for (int target_size = 1; target_size < rodzic->rozmiar; target_size++) {
            memset(odwiedzone, 0, n * sizeof(int));
            dziecko->rozmiar = znajdz_spojna_skladowa(graf_kopia, n, start_vertex, dziecko->wierzcholki, target_size, odwiedzone);
            
            if (dziecko->rozmiar == 0 || dziecko->rozmiar == rodzic->rozmiar) {
                continue; // Invalid split
            }
            
            // Create temporary parent subgraph
            Podgraf temp_rodzic;
            temp_rodzic.wierzcholki = (int *)malloc(rodzic->rozmiar * sizeof(int));
            if (!temp_rodzic.wierzcholki) {
                fprintf(stderr, "Blad alokacji pamieci dla tymczasowego rodzica\n");
                free(dziecko->wierzcholki);
                free(krawedzie);
                zwolnij_graf(graf_kopia, n);
                free(odwiedzone);
                exit(EXIT_FAILURE);
            }
            temp_rodzic.rozmiar = 0;
            
            for (int j = 0; j < rodzic->rozmiar; j++) {
                if (!odwiedzone[rodzic->wierzcholki[j]]) {
                    temp_rodzic.wierzcholki[temp_rodzic.rozmiar++] = rodzic->wierzcholki[j];
                }
            }
            
            // Collect edges to be cut
            int liczba_krawedzi = 0;
            for (int j = 0; j < dziecko->rozmiar; j++) {
                for (int k = 0; k < temp_rodzic.rozmiar; k++) {
                    int u = dziecko->wierzcholki[j];
                    int v = temp_rodzic.wierzcholki[k];
                    if (graf_kopia[u][v] == 1) {
                        krawedzie[liczba_krawedzi].u = u;
                        krawedzie[liczba_krawedzi].v = v;
                        liczba_krawedzi++;
                    }
                }
            }
            
            // Simulate edge cuts in the copied graph
            for (int j = 0; j < dziecko->rozmiar; j++) {
                for (int k = 0; k < temp_rodzic.rozmiar; k++) {
                    int u = dziecko->wierzcholki[j];
                    int v = temp_rodzic.wierzcholki[k];
                    graf_kopia[u][v] = graf_kopia[v][u] = 0;
                }
            }
            
            // Check connectivity after edge cuts
            if (czy_podgraf_spojny(graf_kopia, dziecko, n) && czy_podgraf_spojny(graf_kopia, &temp_rodzic, n)) {
                double margines = oblicz_margines(temp_rodzic.rozmiar, dziecko->rozmiar);
                if (margines <= max_margines) {
                    // Valid split found, report and apply edge cuts
                    wypisz_przeciete_krawedzie(krawedzie, liczba_krawedzi, output);
                    for (int j = 0; j < dziecko->rozmiar; j++) {
                        for (int k = 0; k < temp_rodzic.rozmiar; k++) {
                            int u = dziecko->wierzcholki[j];
                            int v = temp_rodzic.wierzcholki[k];
                            graf[u][v] = graf[v][u] = 0;
                        }
                    }
                    // Update parent
                    free(rodzic->wierzcholki);
                    rodzic->wierzcholki = temp_rodzic.wierzcholki;
                    rodzic->rozmiar = temp_rodzic.rozmiar;
                    free(krawedzie);
                    zwolnij_graf(graf_kopia, n);
                    free(odwiedzone);
                    return 1; // Success
                }
            }
            
            // Restore graph copy for next attempt
            zwolnij_graf(graf_kopia, n);
            graf_kopia = kopiuj_graf(graf, n);
            free(temp_rodzic.wierzcholki);
        }
    }
    
    // No valid split found
    free(dziecko->wierzcholki);
    dziecko->wierzcholki = NULL;
    dziecko->rozmiar = 0;
    free(krawedzie);
    zwolnij_graf(graf_kopia, n);
    free(odwiedzone);
    return 0; // Failure
}

int policz_graf(int n, int **graf); // Forward declaration

void inicjalizuj_poczatkowe_podgrafy(int **graf, int n, Podgraf *podgrafy, int *liczba_podgrafow) {
    int *odwiedzone = (int *)calloc(n, sizeof(int));
    if (!odwiedzone) {
        fprintf(stderr, "Blad alokacji pamieci w inicjalizuj_poczatkowe_podgrafy\n");
        exit(EXIT_FAILURE);
    }
    
    *liczba_podgrafow = 0;
    
    for (int i = 0; i < n; i++) {
        if (!odwiedzone[i]) {
            int *wierzcholki = (int *)malloc(n * sizeof(int));
            if (!wierzcholki) {
                fprintf(stderr, "Blad alokacji pamieci dla wierzcholkow podgrafu\n");
                free(odwiedzone);
                exit(EXIT_FAILURE);
            }
            
            int rozmiar = 0;
            int *kolejka = (int *)malloc(n * sizeof(int));
            if (!kolejka) {
                fprintf(stderr, "Blad alokacji pamieci dla kolejki\n");
                free(wierzcholki);
                free(odwiedzone);
                exit(EXIT_FAILURE);
            }
            
            int head = 0, tail = 0;
            kolejka[tail++] = i;
            odwiedzone[i] = 1;
            wierzcholki[rozmiar++] = i;
            
            while (head < tail) {
                int u = kolejka[head++];
                for (int v = 0; v < n; v++) {
                    if (graf[u][v] && !odwiedzone[v]) {
                        odwiedzone[v] = 1;
                        kolejka[tail++] = v;
                        wierzcholki[rozmiar++] = v;
                    }
                }
            }
            
            podgrafy[*liczba_podgrafow].wierzcholki = wierzcholki;
            podgrafy[*liczba_podgrafow].rozmiar = rozmiar;
            (*liczba_podgrafow)++;
            free(kolejka);
        }
    }
    
    free(odwiedzone);
}

void podziel_graf_iteracyjnie(int **graf, int n, int liczba_podzielen, int max_margines, FILE *output) {
    int max_possible_subgraphs = n;
    Podgraf *podgrafy = (Podgraf *)calloc(max_possible_subgraphs, sizeof(Podgraf));
    if (!podgrafy) {
        fprintf(stderr, "Blad alokacji pamieci dla podgrafow\n");
        exit(EXIT_FAILURE);
    }
    
    int aktualna_liczba_podgrafow = 0;
    int wykonane_podzialy = 0;
    
    // Track which subgraphs cannot be split further
    int *cannot_split = (int *)calloc(max_possible_subgraphs, sizeof(int));
    if (!cannot_split) {
        fprintf(stderr, "Blad alokacji pamieci dla cannot_split\n");
        free(podgrafy);
        exit(EXIT_FAILURE);
    }
    
    inicjalizuj_poczatkowe_podgrafy(graf, n, podgrafy, &aktualna_liczba_podgrafow);
    
    printf("Poczatkowa liczba spojnych skladowych: %d\n", aktualna_liczba_podgrafow);
    fprintf(output, "Poczatkowa liczba spojnych skladowych: %d\n", aktualna_liczba_podgrafow);
    
    while (wykonane_podzialy < liczba_podzielen) {
        int najwiekszy_idx = -1;
        int najwiekszy_rozmiar = 0;
        
        // Find the largest splittable subgraph that hasn't failed
        for (int i = 0; i < aktualna_liczba_podgrafow; i++) {
            if (!cannot_split[i] && podgrafy[i].rozmiar > najwiekszy_rozmiar && czy_podgraf_spojny(graf, &podgrafy[i], n)) {
                najwiekszy_rozmiar = podgrafy[i].rozmiar;
                najwiekszy_idx = i;
            }
        }
        
        if (najwiekszy_idx == -1) {
            printf("Nie mozna wykonac dalszych podzialow - brak odpowiednich podgrafow\n");
            fprintf(output, "Nie mozna wykonac dalszych podzialow - brak odpowiednich podgrafow\n");
            break;
        }
        
        Podgraf *rodzic = &podgrafy[najwiekszy_idx];
        Podgraf *dziecko = &podgrafy[aktualna_liczba_podgrafow];
        
        // Attempt to split the subgraph
        if (!podziel_podgraf(graf, rodzic, dziecko, n, max_margines, output)) {
            printf("Nie udalo sie wykonac podzialu podgrafu %d - brak spojnego podzialu\n", najwiekszy_idx);
            fprintf(output, "Nie udalo sie wykonac podzialu podgrafu %d - brak spojnego podzialu\n", najwiekszy_idx);
            cannot_split[najwiekszy_idx] = 1; // Mark as unsplittable
            continue;
        }
        
        printf("Wykonano podzial %d podgrafu %d na:\n", wykonane_podzialy + 1, najwiekszy_idx);
        printf("- Podgraf %d: %d wierzcholkow\n", najwiekszy_idx, rodzic->rozmiar);
        printf("- Podgraf %d: %d wierzcholkow\n", aktualna_liczba_podgrafow, dziecko->rozmiar);
        printf("Margines: %.2f%%\n", oblicz_margines(rodzic->rozmiar, dziecko->rozmiar));
        
        fprintf(output, "Wykonano podzial %d podgrafu %d na:\n", wykonane_podzialy + 1, najwiekszy_idx);
        fprintf(output, "- Podgraf %d: %d wierzcholkow\n", najwiekszy_idx, rodzic->rozmiar);
        fprintf(output, "- Podgraf %d: %d wierzcholkow\n", aktualna_liczba_podgrafow, dziecko->rozmiar);
        fprintf(output, "Margines: %.2f%%\n", oblicz_margines(rodzic->rozmiar, dziecko->rozmiar));
        
        wykonane_podzialy++;
        aktualna_liczba_podgrafow++;
    }
    
    printf("\nOstateczny podzial (liczba podgrafow: %d):\n", aktualna_liczba_podgrafow);
    fprintf(output, "\nOstateczny podzial (liczba podgrafow: %d):\n", aktualna_liczba_podgrafow);
    for (int i = 0; i < aktualna_liczba_podgrafow; i++) {
        printf("Podgraf %d (%d wierzcholkow): ", i, podgrafy[i].rozmiar);
        fprintf(output, "Podgraf %d (%d wierzcholkow): ", i, podgrafy[i].rozmiar);
        for (int j = 0; j < podgrafy[i].rozmiar; j++) {
            printf("%d ", podgrafy[i].wierzcholki[j]);
            fprintf(output, "%d ", podgrafy[i].wierzcholki[j]);
        }
        printf(czy_podgraf_spojny(graf, &podgrafy[i], n) ? "[spojny]\n" : "[niespojny]\n");
        fprintf(output, czy_podgraf_spojny(graf, &podgrafy[i], n) ? "[spojny]\n" : "[niespojny]\n");
    }
    
    if (wykonane_podzialy < liczba_podzielen) {
        printf("Wykonano tylko %d z %d zadanym podzialow z powodu ograniczen spojnosci lub marginesu\n", wykonane_podzialy, liczba_podzielen);
        fprintf(output, "Wykonano tylko %d z %d zadanym podzialow z powodu ograniczen spojnosci lub marginesu\n", wykonane_podzialy, liczba_podzielen);
    }
    
    for (int i = 0; i < aktualna_liczba_podgrafow; i++) {
        if (podgrafy[i].wierzcholki) {
            free(podgrafy[i].wierzcholki);
        }
    }
    free(podgrafy);
    free(cannot_split);
}

void dzielenie_grafu(int **graf, int n, int margines_procentowy, int liczba_podzielen, FILE *output) {
    printf("\nRozpoczecie dzielenia grafu na %d podzialow z marginesem %d%%\n", 
           liczba_podzielen, margines_procentowy);
    fprintf(output, "\nRozpoczecie dzielenia grafu na %d podzialow z marginesem %d%%\n", 
           liczba_podzielen, margines_procentowy);
    
    podziel_graf_iteracyjnie(graf, n, liczba_podzielen, margines_procentowy, output);
}
