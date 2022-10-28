#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>

/*******************************
 *          ATENÇÃO            *
 *   NÃO EDITAR ESSE ARQUIVO   *
 *          ATENÇÃO            *
 *******************************/

extern int **matriz1;
extern int **matriz2;
extern int **resultado;
extern int tamanho_matriz;

/*******************************
 *          ATENÇÃO            *
 *   NÃO EDITAR ESSE ARQUIVO   *
 *          ATENÇÃO            *
 *******************************/

//Gera as matrizes usando números aleatórios
void gerar_matrizes() {
    srand(time(NULL));

    matriz1 = malloc(tamanho_matriz * sizeof(int *));
    matriz2 = malloc(tamanho_matriz * sizeof(int *));
    resultado = malloc(tamanho_matriz * sizeof(int *));

    int i, j;
    for (i = 0; i < tamanho_matriz; i++) {
        matriz1[i] = malloc(tamanho_matriz * sizeof(int));
        matriz2[i] = malloc(tamanho_matriz * sizeof(int));
        resultado[i] = malloc(tamanho_matriz * sizeof(int));
        for (j = 0; j < tamanho_matriz; j++) {
            matriz1[i][j] = rand()%10;
            matriz2[i][j] = rand()%10;
            resultado[i][j] = 0;
        }
    }
}

/*******************************
 *          ATENÇÃO            *
 *   NÃO EDITAR ESSE ARQUIVO   *
 *          ATENÇÃO            *
 *******************************/

//Imprime os valores de uma matriz no arquivo
void imprimir_matriz(FILE *arquivo, int **matriz) {
    int i, j;
    for (i = 0; i < tamanho_matriz; i++) {
        for (j = 0; j < tamanho_matriz; j++) {
            fprintf(arquivo, "%d ", matriz[i][j]);
        }
        fprintf(arquivo, "\n");
    }
    fprintf(arquivo, "\n");
}

/*******************************
 *          ATENÇÃO            *
 *   NÃO EDITAR ESSE ARQUIVO   *
 *          ATENÇÃO            *
 *******************************/

//Imprime todas as matrizes no arquivo de resultados
void imprimir_matrizes() {
    FILE *arquivo;
    arquivo = fopen("resultado.txt", "w");
    if (arquivo == NULL) {
        printf("Erro ao abrir arquivo de resultados!\n");
        exit(1);
    }

    fprintf(arquivo, "Matriz 1\n");
    imprimir_matriz(arquivo, matriz1);
    fprintf(arquivo, "Matriz 2\n");
    imprimir_matriz(arquivo, matriz2);
    fprintf(arquivo, "Resultado 2\n");
    imprimir_matriz(arquivo, resultado);
    fclose(arquivo);
}

/*******************************
 *          ATENÇÃO            *
 *   NÃO EDITAR ESSE ARQUIVO   *
 *          ATENÇÃO            *
 *******************************/

//Libera a memória alocada para as matrizes
void liberar_matrizes() {
    int i;
    for (i = 0; i < tamanho_matriz; i++) {
        free(matriz1[i]);
        free(matriz2[i]);
        free(resultado[i]);
    }

    free(matriz1);
    free(matriz2);
    free(resultado);
}

/*******************************
 *          ATENÇÃO            *
 *   NÃO EDITAR ESSE ARQUIVO   *
 *          ATENÇÃO            *
 *******************************/
