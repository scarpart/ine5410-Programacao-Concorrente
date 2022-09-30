#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <pthread.h>

/* definindo o struct que vai ser usado para passar
todos os argumentos necessários à função thread */
typedef struct allargs {
    double* a;
    double* b;
    double* c;     
    int n_loops;          // a_size / n_threads = n_loops
    int* start;           // index do início do for na função thread
    int* remainder_loops; // a_size % n_threads = remainder_loops
} allargs;

// Lê o conteúdo do arquivo filename e retorna um vetor E o tamanho dele
// Se filename for da forma "gen:%d", gera um vetor aleatório com %d elementos
//
// +-------> retorno da função, ponteiro para vetor malloc()ado e preenchido
// | 
// |         tamanho do vetor (usado <-----+
// |         como 2o retorno)              |
// v                                       v
double* load_vector(const char* filename, int* out_size);

void* thread(void* arg);

// Avalia se o prod_escalar é o produto escalar dos vetores a e b. Assume-se
// que ambos a e b sejam vetores de tamanho size.
void avaliar(double* a, double* b, int size, double prod_escalar);

int main(int argc, char* argv[]) {
    srand(time(NULL));

    //Temos argumentos suficientes?
    if(argc < 4) {
        printf("Uso: %s n_threads a_file b_file\n"
               "    n_threads    número de threads a serem usadas na computação\n"
               "    *_file       caminho de arquivo ou uma expressão com a forma gen:N,\n"
               "                 representando um vetor aleatório de tamanho N\n", 
               argv[0]);
        return 1;
    }
  
    //Quantas threads?
    int n_threads = atoi(argv[1]);
    if (!n_threads) {
        printf("Número de threads deve ser > 0\n");
        return 1;
    }
    //Lê números de arquivos para vetores alocados com malloc
    int a_size = 0, b_size = 0;
    double* a = load_vector(argv[2], &a_size);
    if (!a) {
        //load_vector não conseguiu abrir o arquivo
        printf("Erro ao ler arquivo %s\n", argv[2]);
        return 1;
    }
    double* b = load_vector(argv[3], &b_size);
    if (!b) {
        printf("Erro ao ler arquivo %s\n", argv[3]);
        return 1;
    }
    
    //Garante que entradas são compatíveis
    if (a_size != b_size) {
        printf("Vetores a e b tem tamanhos diferentes! (%d != %d)\n", a_size, b_size);
        return 1;
    }

    //Calcula produto escalar. Paralelize essa parte
    if (n_threads > a_size) n_threads = a_size;

    int n_loops = a_size / n_threads;
    int remainder_loops = a_size % n_threads;
    int start = 0;
    pthread_t all_threads[n_threads];
    double result = 0;
    allargs std_args = { a, b, &result, n_loops, &start, &remainder_loops };

    for (int i = 0; i < n_threads; ++i)
        pthread_create(&all_threads[i], NULL, thread, (void*) &std_args);
    
    for (int j = 0; j < n_threads; j++)
        pthread_join(all_threads[j], NULL); 
    
    //    +---------------------------------+
    // ** | IMPORTANTE: avalia o resultado! | **
    //    +---------------------------------+
    avaliar(a, b, a_size, result);

    //Libera memória
    free(a);
    free(b);

    return 0;
}

void* thread(void* arg) {
    allargs* args = (allargs*)arg;

    /* essa variável armazena n_loops se não tiver remainder_loops,
    caso tenha, ela vai armazenar n_loops+1 para distribuir os 
    remainder_loops de forma igualitária para vários threads */
    int temp = args->n_loops;
    if (*(args->remainder_loops) > 0) {
        temp++;
        *(args->remainder_loops) -= 1;
    }
    
    for (int i = *(args->start); i < temp + *(args->start); i++)
        *(args->c) += args->a[i] * args->b[i];
    *(args->start) += temp;
    pthread_exit(NULL);
}
