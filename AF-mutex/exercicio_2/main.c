#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>

//Funções auxiliares. Definidas em helper.c -- NÃO DEVEM SER ALTERADAS
void gerar_matrizes();
void imprimir_matriz(FILE *arquivo, int **matriz);
void imprimir_matrizes();
void liberar_matrizes();

// Função executada pelas worker threads. Definida em thread.c
// É nessa função que está ocorrendo um data race
void *matrix_mult_worker(void *arg);

// Matrizes a serem multiplicadas
int **matriz1;
int **matriz2;
// Matriz resultante
int **resultado;
// Argumento de linha de comando
int tamanho_matriz;
// Usados em matrix_mult_worker
int linha_atual, coluna_atual;

// Dica: Use esse mutex para resolver o problema
// Dica: Inicialize o mutex antes de usar!
pthread_mutex_t matrix_mutex;

int main(int argc, char* argv[]) {
    //Verifica se recebemos os argumentos necessários
    if (argc < 3) {
        printf("Uso: %s [tamanho da matriz] [threads]\n", argv[0]);
        return 1;
    }
    
    //Parseia argumentos
    tamanho_matriz = atoi(argv[1]);
    int num_threads = atoi(argv[2]);

    //Dica: pense sobre qual é o papel dessas duas variáveis
    linha_atual = 0;
    coluna_atual = 0;

    //Aloca a memória das matrizes e já gera os números aleatórios das
    //matrizes 1 e 2.
    //As matrizes serão colocadas nas globais matriz1 e matriz2
    gerar_matrizes();

    pthread_mutex_init(&matrix_mutex, NULL);

    //Crias as threads
    pthread_t threads[num_threads];
    for (int i = 0; i < num_threads; i++) {
        pthread_create(&threads[i], NULL, matrix_mult_worker, NULL);
    }

    //Aguarda elas terminarem...
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }   

    pthread_mutex_destroy(&matrix_mutex);

    //Imprime as matrizes em um arquivo resultado.txt
    imprimir_matrizes();

    //Libera a memória das matrizes
    liberar_matrizes();
    return 0;
}
