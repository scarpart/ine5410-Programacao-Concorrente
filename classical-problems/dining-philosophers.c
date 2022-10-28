#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>

#define N 5 // number of forks

sem_t forks[N];

void takes(int i) {
    sem_wait(&forks[i]);
}

void thinks() {
    printf("thinking...\n");
    sleep(2);
}

void eats() {
    printf("nom nom nom\n");
    sleep(2);
}

void returns(int i) {
    sem_post(&forks[i]);
}

void* philosopher(void* args) {
    
    int i = 0;

    while (1) {
        thinks();
        if (i < N - 1) {
            takes(i);
            takes((i + 1) % N);
            printf("Philosopher took forks %d and %d\n", i, (i + 1) % N);
        } else {
            takes((i + 1) % N);
            takes(i);
            printf("Philosopher took forks %d and %d\n", (i + 1) % N, i);
        }
        eats();
        returns(i);
        returns((i + 1) % N);
        printf("Philosopher returned forks %d and %d\n", i, (i + 1) % N);
        i = (i + 1) % N;
    }

}

int main(int argc, char** argv) {
    
    /* initializing semaphores */
    for (int i = 0; i < N; i++)
        sem_init(&forks[i], 0, 1);

    /* initializing and creating threads */
    pthread_t all_threads[atoi(argv[1])];
    for (int i = 0; i < N; i++)
        pthread_create(&all_threads[i], NULL, philosopher, NULL);

    for (int i = 0; i < N; i++)
        pthread_join(all_threads[i], NULL);

}