#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <unistd.h>
#include "dzielenie.h"
#include "pliki.h"

void print_macierz(int **macierz, int rozmiar)
{
    for (int i = 0; i < rozmiar; i++)
    {
        for (int j = 0; j < rozmiar; j++)
        {
            printf("%d ", macierz[i][j]);
        }
        printf("\n");
    }
}

int policz_polaczone_wierzcholki(int start, int n, int **graf)
{
    int *odwiedzone = (int *)calloc(n, sizeof(int));
    dfs(start, odwiedzone, n, graf);
    int licznik = 0;
    for (int i = 0; i < n; i++)
    {
        if (odwiedzone[i])
        {
            licznik++;
        }
    }
    free(odwiedzone);
    return licznik;
}

int policz_graf(int n, int **graf)
{
    int *odwiedzone = (int *)calloc(n, sizeof(int));
    int licznik = 0;

    for (int i = 0; i < n; i++)
    {
        if (odwiedzone[i] == 0)
        {
            licznik += 1;
            dfs(i, odwiedzone, n, graf);
        }
    }
    return licznik;
}
// void przetwarzanie_grafu(FILE *plik, int ***graf, int *n)
// {
//     char linia[1024];
//     int rozmiar = 0;
//     /*1.  Trzeba ogarnąć ile wierszy ma graf podany w pliku
//         Liczba wierszy powinna się równać liczbie wierzchołków w grafie
//         Przyjmuje że właśnie w pliku będą nam dawać macierz sąsiadów
//         Na isodzie napisali coś takiego...
//         Graf do podziału zostanie przekazany w formacie tekstowym - opis pojawi się poniżej.

//     2.  Wczytywanie wartości z pliku do macierzy
//     */

//     // 1.
//     while (fgets(linia, sizeof(linia), plik))
//     {
//         rozmiar++;
//     }
//     rewind(plik);
//     *n = rozmiar;
//     printf("Rozmiar = %d \n", rozmiar);


//     *graf = (int **)calloc(rozmiar, sizeof(int *));

//     for (int i = 0; i < rozmiar; i++)
//     {
//         (*graf)[i] = (int *)calloc(rozmiar, sizeof(int));
//     }
//     // 2.
//     for (int i = 0; i < rozmiar; i++)
//     {
//         for (int j = 0; j < rozmiar; j++)
//         {
//             if (fscanf(plik, "%d", &(*graf)[i][j]) != 1)
//             {
//                 printf("Wystapil blad w odczytywaniu pliku do grafu!\n");
//                 return;
//             }
//         }
//     }
// }
int main(int argc, char *argv[])
{
    // domyslne parametry
    char *input_plik = NULL;
    char *output_plik = NULL;
    int margines_procentowy = 10;
    int docelowa_liczba_podgrafow = 2;
    int czy_terminal=0;
    int czy_binarny=0;

    int opt; 
    while ((opt = getopt(argc, argv, "n:m:o:tb")) != -1)  //n: to oczekujemy czegos po n, jak nie ma dwukropka to niczego
    { 
        switch(opt) {
            case 'n':
                docelowa_liczba_podgrafow = atoi(optarg);
                break;
            case 'm':
                margines_procentowy = atoi(optarg);
                break;
            case 'o':
                output_plik = strdup(optarg);
                break;
            case 't':
                czy_terminal = 1;
                break;
            case 'b':
                czy_binarny = 1;
                break;
            default:
                fprintf(stderr, "Nieznany argument: %c\n", optopt);
                exit(EXIT_FAILURE);
                break;
        }
    }
    if (optind >= argc) {
        fprintf(stderr, "Nie podano pliku wejsciowego!\n");
        exit(EXIT_FAILURE);
    }
    // domyślny plik wyjściowy jeśli nie ma -t ani -o
    if (!czy_terminal && output_plik == NULL) {
        output_plik = strdup(czy_binarny ? "wynik.bin" : "wynik.txt");
    }
    

    // Otwieranie pliku wyjściowego
    FILE *output = NULL;
    if (czy_terminal) {
        output = stdout;
    } else if (output_plik) {
        output = fopen(output_plik, czy_binarny ? "wb" : "w");
    }

    if (output == NULL) {
        perror("Blad przy otwieraniu pliku wyjsciowego");
        exit(EXIT_FAILURE);
    }

    input_plik= argv[optind];
    FILE *plik = fopen(input_plik, "r");
    if (plik == NULL) {
        perror("Blad przy otwieraniu pliku wejsciowego");
        exit(EXIT_FAILURE);
    }
    

    int liczba_wierzcholkow;
    int **graf = NULL;
    
    przetwarzanie_grafu(plik, &graf, &liczba_wierzcholkow);
    //zbuduj_macierz_sasiedztwa(plik, graf, liczba_wierzcholkow);
    fclose(plik);

    // Wyświetlenie informacji początkowych
    printf("\nWczytany graf (%d wierzcholkow):\n", liczba_wierzcholkow);
    //print_macierz(graf, liczba_wierzcholkow);

    printf("\nPoczatkowa liczba spojnych skladowych: %d\n", policz_graf(liczba_wierzcholkow, graf));
    printf("Docelowa liczba podgrafow: %d\n", docelowa_liczba_podgrafow);
    printf("Dopuszczalny margines procentowy: %d%%\n", margines_procentowy);

    // Dzielenie grafu
    dzielenie_grafu(graf, liczba_wierzcholkow, margines_procentowy, docelowa_liczba_podgrafow, output, czy_binarny);

    // Zwolnienie pamięci
    for (int i = 0; i < liczba_wierzcholkow; i++)
    {
        free(graf[i]);
    }
    free(graf);
    if (output_plik) {
        free(output_plik);
    }
   
    return 0;
}