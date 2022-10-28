#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <pthread.h>

//                 (main)      
//                    |
//    +----------+----+------------+
//    |          |                 |   
// worker_1   worker_2   ....   worker_n


// ~~~ argumentos (argc, argv) ~~~
// ./program n_threads

// ~~~ printfs  ~~~
// main thread (após término das threads filhas): "Contador: %d\n"
// main thread (após término das threads filhas): "Esperado: %d\n"

// Obs:
// - pai deve criar n_threds (argv[1]) worker threads 
// - cada thread deve incrementar contador_global (operador ++) n_loops vezes
// - pai deve esperar pelas worker threads  antes de imprimir!


int contador_global = 0;
pthread_mutex_t mutex;

void *incrementor(void *arg) {
    int n_loops = *(int *)arg;

    pthread_mutex_lock(&mutex);
    for (int i = 0; i < n_loops; i++) 
        contador_global += 1;
    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL); // ou return NULL;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Uso: %s n_threads n_loops\n", argv[0]);
        return 1;
    }

    int n_threads = atoi(argv[1]);
    int n_loops = atoi(argv[2]);
    pthread_t threads[n_threads];

    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);

    for (int i = 0; i < n_threads; i++)
        pthread_create(&threads[i], NULL, incrementor, (void*)&n_loops);

    for (int i = 0; i < n_threads; i++)
        pthread_join(threads[i], NULL);

    pthread_mutex_destroy(&mutex);

    printf("Contador: %d\n", contador_global);
    printf("Esperado: %d\n", n_threads * n_loops);
    return 0;
}
