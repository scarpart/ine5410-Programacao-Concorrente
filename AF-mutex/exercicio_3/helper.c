#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

/*******************************
 *          ATENÇÃO            *
 *   NÃO EDITAR ESSE ARQUIVO   *
 *          ATENÇÃO            *
 *******************************/

void imprimir_resultados(int n, int** results) {
    for (int i = 0; i < n; ++i) 
        printf("results[%d]: %d\n", i, *results[i]);
    
}

/*******************************
 *          ATENÇÃO            *
 *   NÃO EDITAR ESSE ARQUIVO   *
 *          ATENÇÃO            *
 *******************************/
