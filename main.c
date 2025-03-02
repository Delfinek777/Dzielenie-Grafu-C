#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_macierz(int **macierz,int rozmiar){
      for(int i = 0; i<rozmiar; i++){
        for(int j = 0; j<rozmiar ; j++){
            printf("%d ",macierz[i][j]);
        }
        printf("\n");
    }
}
void przetwarzanie_grafu(FILE *plik,int ***graf,int *n){
    char linia[1024];
    int rozmiar = 0;
    /*1.  Trzeba ogarnąć ile wierszy ma graf podany w pliku
          Liczba wierszy powinna się równać liczbie wierzchołków w grafie
          Przyjmuje że właśnie w pliku będą nam dawać macierz sąsiadów
          Na isodzie napisali coś takiego...
          Graf do podziału zostanie przekazany w formacie tekstowym - opis pojawi się poniżej.
      
      2.  Wczytywanie wartości z pliku do macierzy
    */

    //1.
    while(fgets(linia,sizeof(linia),plik)){
        rozmiar++;
    }
    rewind(plik);
    *n = rozmiar;
    printf("Rozmiar = %d \n",rozmiar);
    *graf = (int**)calloc(rozmiar,sizeof(int*));

    for(int i = 0;i<rozmiar;i++){
        (*graf)[i]  = (int*)calloc(rozmiar,sizeof(int));
    }
    //2.
    for(int i = 0; i<rozmiar; i++){
        for(int j = 0; j<rozmiar ; j++){
            if(fscanf(plik,"%d",&(*graf)[i][j]) != 1){
                printf("Wystapil blad w odczytywaniu pliku do grafu!\n");
                return;
            }
        }
    }
}

int main(int argc, char *argv[]){
    char nazwa_pliku[30];
    int liczba_wierzcholkow = 0;
    int margines_procentowy;
    if (argc<3){
        printf("Nie podano prawidlowych argumentow!\n");
        return 1;
    }
    strcpy(nazwa_pliku,argv[1]);
    FILE *plik = fopen(nazwa_pliku,"r");
    int **graf = NULL;
    margines_procentowy = atoi(argv[2]);

    przetwarzanie_grafu(plik,&graf,&liczba_wierzcholkow);
    fclose(plik);
    print_macierz(graf,liczba_wierzcholkow);
    return 0;
}