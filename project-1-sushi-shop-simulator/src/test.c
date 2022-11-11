#include <stdlib.h>
#include <stdio.h>

int* mutex;

void globals_set_mutex_array(int size) {
    mutex = realloc(mutex, size*sizeof(int));
}

int main() {
    printf("Size of mutex: %ld\n", sizeof(mutex));
    globals_set_mutex_array(6);
    printf("Size of mutex: %ld\n", (sizeof(mutex)));

    return 0;    
}