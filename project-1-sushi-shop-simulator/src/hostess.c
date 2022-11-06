#include <stdlib.h>

#include "hostess.h"
#include "globals.h"
#include "args.h"


int hostess_check_for_a_free_conveyor_seat() {
    /* 
        MODIFIQUE ESSA FUNÇÃO PARA GARANTIR O COMPORTAMENTO CORRETO E EFICAZ DO HOSTESS.
        NOTAS:
        1.  ✅ O HOSTESS DEVE FICAR EM ESPERA ATÉ QUE UMA VAGA SEJA LIBERADA NA ESTEIRA.
        2.  ✅ A VAGA INICIAL (conveyor->_seats[0]) É RESERVADA PARA O SUSHI CHEF.
        3.  ✅ NÃO REMOVA OS PRINTS, NEM O `msleep()` DE DENTRO DO WHILE LOOP.
        4.  ✅ O RETORNO DESSA FUNÇÃO É O ÍNDICE DO CONVEYOR INDICANDO UM ASSENTO LIVRE.
        5.  ❓ CUIDADO COM PROBLEMAS DE SINCRONIZAÇÃO!
    */
    conveyor_belt_t* conveyor = globals_get_conveyor_belt();
    virtual_clock_t* virtual_clock = globals_get_virtual_clock();
    
    print_virtual_time(globals_get_virtual_clock());
    fprintf(stdout, GREEN "[INFO]" NO_COLOR " O Hostess está procurando por um assento livre...\n");
    print_conveyor_belt(conveyor);

    while (TRUE) {
        for (int i=0; i<conveyor->_size; i++) {
            /* ✅ 1 e 2 - Fica em espera até que a vaga seja liberada, descontando o assento 0 */
            /* ❓ - não sei se o problema da sinc é devido ao acesso à conveyor->_seats[i] */
            if (conveyor->_seats[i] == -1 && i != 0) {  // Atenção à regra! (-1 = livre, 0 = sushi_chef, 1 = customer)
                print_virtual_time(globals_get_virtual_clock());
                fprintf(stdout, GREEN "[INFO]" NO_COLOR " O Hostess encontrou o assento %d livre para o próximo cliente!\n", i);
                /* ✅ 4 - O retorno é o indice do acento livre */
                return i;
            }
        }
        msleep(120000/virtual_clock->clock_speed_multiplier);  // Não remova esse sleep!
    }
}

void hostess_guide_first_in_line_customer_to_conveyor_seat(int seat) {
    /* 
        MODIFIQUE ESSA FUNÇÃO PARA GARANTIR O COMPORTAMENTO CORRETO E EFICAZ DO HOSTESS.
        NOTAS:
        1.  ✅ NESSA FUNÇÃO É PAPEL DO HOSTESS GARANTIR 3 ATUALIZAÇÕES:
            a. ✅ O valor _seat_position do cliente em questão deve ser atualizado
            b. ✅ O valor do assento em questão no conveyor_belt global deve ser atualizado 
               (-1 = vazio, 0 = sushi_chef, 1 = cliente)
            c. ✅ O cliente em questão deve ser removido da fila global
        2.  ❓ CUIDADO COM PROBLEMAS DE SINCRONIZAÇÃO!
        3.  ✅ NÃO REMOVA OS PRINTS!
    */
    conveyor_belt_t* conveyor = globals_get_conveyor_belt();
    queue_t* queue = globals_get_queue();

    /* ❓ 2 - acredito que esse mutex já resolva o problema */
    pthread_mutex_lock(&conveyor->_seats_mutex);
    /* ✅ 1.c - cliente removido da fila global */
    customer_t* customer = queue_remove(queue);
    /* ✅ 1.b - assento do conveyor_belt atualizado */
    conveyor->_seats[seat] = 1;
    /* ✅ 1.a - assento do cliente atualizado */
    customer->_seat_position=seat;
    pthread_mutex_unlock(&conveyor->_seats_mutex);

    print_virtual_time(globals_get_virtual_clock());
    fprintf(stdout, GREEN "[INFO]" NO_COLOR " O Hostess levou o cliente %d para o assento %d!\n", customer->_id, seat);
    print_conveyor_belt(conveyor);    
}

void* hostess_run() {
    /* 
        MODIFIQUE ESSA FUNÇÃO PARA GARANTIR O COMPORTAMENTO CORRETO E EFICAZ DO HOSTESS.
        NOTAS:
        1.  ✅ O HOSTESS DEVE FUNCIONAR EM LOOP, RETIRANDO CLIENTES DA FILA GLOBAL E ADICIONANDO-OS NA
            ESTEIRA GLOBAL CONFORME VAGAS SÃO LIBERADAS.
        2.  ✅ QUANDO O SUSHI SHOP FECHAR, O HOSTESS DEVE PARAR DE GUIAR NOVOS CLIENTES DA FILA PARA 
            A ESTEIRA, E ESVAZIAR A FILA GLOBAL, FINALIZANDO OS CLIENTES EM ESPERA.
        3.  ❓ CUIDADO COM PROBLEMAS DE SINCRONIZAÇÃO!
            ~Arthur: acho que isso já é resolvido dentro daquelas 2 funçoes no if
        4.  ✅ NÃO REMOVA OS PRINTS!
    */
    virtual_clock_t* virtual_clock = globals_get_virtual_clock();
    queue_t* queue = globals_get_queue();

    /* ✅ 2 - Quando o shop fecha, hostess para de guiar clientes */
    while (globals_get_virtual_clock()->current_time < globals_get_virtual_clock()->closing_time) {  // Adicione a lógica para que o Hostess realize o fechamento do Sushi Shop!
        /* ✅ 1 - loop que tira pessoas da fila e coloca no conveyor */
        if (queue->_length > 0) {
            int seat = hostess_check_for_a_free_conveyor_seat();
            hostess_guide_first_in_line_customer_to_conveyor_seat(seat);
        }
        msleep(3000/virtual_clock->clock_speed_multiplier);  // Não remova esse sleep!
    }

    /* ✅ 2 - Hostess limpa a fila */
    queue_finalize(queue);
    pthread_exit(NULL);
}

hostess_t* hostess_init() {
    /* NÃO PRECISA ALTERAR ESSA FUNÇÃO */
    hostess_t* self = malloc(sizeof(hostess_t));
    if (self == NULL) {
        fprintf(stdout, RED "[ERROR] Bad malloc() at `hostess_t* hostess_init()`.\n" NO_COLOR);
        exit(EXIT_FAILURE);
    }
    pthread_create(&self->thread, NULL, hostess_run, NULL);
    return self;
}

void hostess_finalize(hostess_t* self) {
    /* NÃO PRECISA ALTERAR ESSA FUNÇÃO */
    pthread_join(self->thread, NULL);
    free(self);
}
