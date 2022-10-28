#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define N 16 // size of buffer

int i = -1, f = -1;
int buffer[N];
sem_t full, empty, lock_prod, lock_cons;

void* producer(void* args) {
    /**
     * @brief adds item to the front of the queue
     * if i == N, wraps around
     *  
     * @param value value to be added
     */

    while (1) {
        sem_wait(&empty);
        sem_wait(&lock_prod);
        i = (i + 1) % N;
        buffer[i] = rand() % 100;
        printf("Producer produced %d\n", buffer[i]);
        sem_post(&lock_prod);
        sem_post(&full);
    }

    pthread_exit(NULL);

}

void* consumer(void *arg) {
    /**
     * @brief removes item from the end of the queue 
     * if f == N, wraps around
     * removed values are represented by 0
     */

    while (1) {
        sem_wait(&full);
        sem_wait(&lock_cons); // mutex lock
        f = (f + 1) % N;
        printf("Consumer consumed %d\n", buffer[f]);
        buffer[f] = 0;
        sem_post(&lock_cons); // mutex unlock
        sem_post(&empty);
    }

    pthread_exit(NULL);

}

void initializeBuffer() {
    for (int i = 0; i < N; i++)
        buffer[i] = 0;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: ./program n_threads\n");
        return 1;
    }

    /* initializing thread vector and buffer*/
    int n_threads = atoi(argv[1]);
    pthread_t all_threads[n_threads];
    initializeBuffer();

    /* initializing semaphores */
    sem_init(&full, 0, 0);  
    sem_init(&empty, 0, N);
    sem_init(&lock_prod, 0, 1); // binary semaphor = mutex
    sem_init(&lock_cons, 0, 1); // binary semaphor = mutex

    /* creating threads on producer() and consumer() */
    for (int i = 0; i < n_threads; i++) {
        pthread_create(&all_threads[i], NULL, producer, NULL);
        pthread_create(&all_threads[i], NULL, consumer, NULL);
    }

    /* joining all threads */
    for (int j = 0; j < n_threads; j++)
        pthread_join(all_threads[j], NULL);

    /* destroying previously utilized semaphores */
    sem_destroy(&full);
    sem_destroy(&empty);
    sem_destroy(&lock_prod);
    sem_destroy(&lock_cons);

    return 0;
}




