#include <stdio.h>
#include <stdlib.h>

#include "sushi_chef.h"
#include "globals.h"
#include "menu.h"


/* TODO:
em algum lugar aqui, o sushi chef prepara a comida e depois coloca na esteira. é aquele
problema do produtor e consumidor (circular buffer sincronization problem), onde se usam
semáforos para sincronizar e tals, minha pergunta aqui é: já que só tem um sushi chef, a 
gente deixa como um mutex (semáforo binário), ou coloca como semáforo inicializado com o
número de chefs?
*/

/* TODO:
outra coisa - acho que tem como mudar o mutex do sushi chef de globals para algum outro 
lugar ou então usar outro mutex.
*/

void* sushi_chef_run(void* arg) {
    /* 
        MODIFIQUE ESSA FUNÇÃO PARA GARANTIR O COMPORTAMENTO CORRETO E EFICAZ DO SUSHI CHEF.
        NOTAS:
        1.  ✅ O SUSHI CHEF SÓ PODE COMEÇAR A COZINHAR DEPOIS QUE ESTIVER POSICIONADO NA ESTEIRA.
        2.  ✅ ESSA FUNÇÃO JÁ POSSUI A LÓGICA PARA QUE O SUSHI CHEF COMECE A PREPARAR PRATOS ALEATÓRIOS.
        3.  ✅ VOCÊ DEVE ADICIONAR A LÓGICA PARA QUE O SUSHI CHEF PARE DE ADICIONAR PRATOS E SAIA DA 
            ESTEIRA QUANDO O SUSHI SHOP FECHAR (VEJA O ARQUIVO `virtual_clock.c`).
        4.  ❓ CUIDADO COM ERROS DE CONCORRÊNCIA.
            ~Arthur: acredito que isso tenha sido resolvido com o mutex antes de sushi_chef_prepare_food,
                     mas não tenho certeza. minha intuição me diz que está certo.
    */

    sushi_chef_t* self = (sushi_chef_t*) arg;
    virtual_clock_t* global_clock = globals_get_virtual_clock();

    /* ✅ 1 - O sushi chef senta antes de começar a cozinhar */
    sushi_chef_seat(self);
    /* Checa se o chef está posicionado no lugar correto da esteira */
    if (globals_get_conveyor_belt()->_seats[self->_seat_position] == 0) {
        /* ✅ 3 - O sushi chef prepara comida até o horário de fechamento do restaurante, depois sai */
        
        /* Prepara pratos até a hora de fechamento do restaurante */
        while (globals_get_virtual_clock()->current_time < globals_get_virtual_clock()->closing_time) {
            enum menu_item next_dish = rand() % 5;

            pthread_mutex_lock(globals_get_sushi_chef_mutex());
            sushi_chef_prepare_food(self, next_dish);
            sushi_chef_place_food(self, next_dish);
            pthread_mutex_unlock(globals_get_sushi_chef_mutex());
        }
        
        /* sushi chef sai da esteira */
        sushi_chef_leave(self);
    }

    pthread_exit(NULL);
}

void sushi_chef_seat(sushi_chef_t* self) {
    /* 
        MODIFIQUE ESSA FUNÇÃO PARA GARANTIR O COMPORTAMENTO CORRETO E EFICAZ DO SUSHI CHEF.
        NOTAS:
        1.  ✅ O CHEF DEVE SENTAR-SE NA "PONTA ESQUERDA" OU INÍCIO DA ESTEIRA.
        2.  ✅ NO ARRAY `conveyor_belt_t->_seats` UM SUSHI CHEF É REPRESENTADO POR 0.
        3.  ✅ NO ARRAY `conveyor_belt_t->_seats` UM ASSENTO VAZIO É REPRESENTADO POR -1.
        4.  ✅ CUIDADO COM ERROS DE CONCORRÊNCIA.
        5.  ✅ NÃO REMOVA OS PRINTS.
    */ 
    conveyor_belt_t* conveyor = globals_get_conveyor_belt();

    print_virtual_time(globals_get_virtual_clock());
    fprintf(stdout, GREEN "[INFO]" NO_COLOR " Sushi Chef %d arrived at the Sushi Shop and wants to seat!\n", self->_id);
    
    while (TRUE) {
        for (int i=0; i<conveyor->_size; i++) {
            pthread_mutex_lock(&conveyor->_seats_mutex);
            if (conveyor->_seats[i] == -1) {
                conveyor->_seats[i] = 0;
                self->_seat_position = i;
                print_virtual_time(globals_get_virtual_clock());
                fprintf(stdout, GREEN "[INFO]" NO_COLOR " Sushi Chef %d seated at conveyor->_seats[%d]!\n", self->_id, i);
                pthread_mutex_unlock(&conveyor->_seats_mutex);
                break;
            }
            pthread_mutex_unlock(&conveyor->_seats_mutex);
        }
        break;
    }
}

void sushi_chef_leave(sushi_chef_t* self) {
    /* 
        MODIFIQUE ESSA FUNÇÃO PARA GARANTIR O COMPORTAMENTO CORRETO E EFICAZ DO SUSHI CHEF.
        NOTAS:
        1.  ❓ O CHEF DEVE PARAR DE COZINHAR E SAIR DA ESTEIRA SOMENTE APÓS O HORÁRIO DE FECHAMENTO DA LOJA.
            ~Arthur: implementei isso no sushi_chef_run, será que tem que fazer alguma lógica aqui também?
                     Acredito que não, pq fica bem claro no sushi_chef_run que a lógica tem q ficar lá,
                     acho que seria redundante colocar aqui. 
        2.  ✅ CUIDADO COM ERROS DE CONCORRÊNCIA.
        3.  ✅ NÃO REMOVA OS PRINTS.
    */
    conveyor_belt_t* conveyor = globals_get_conveyor_belt();

    /* INSIRA SUA LÓGICA AQUI */
    
    /* ✅ 2 - Cuidado com erros de concorrência:
               mutex garante que apenas uma thread pode esvaziar o assento */
    pthread_mutex_lock(&conveyor->_seats_mutex);
    conveyor->_seats[self->_seat_position] = -1;
    
    print_virtual_time(globals_get_virtual_clock());
    /* ✅ 3 - Prints não são removidos */
    fprintf(stdout, GREEN "[INFO]" NO_COLOR " Sushi Chef %d seated at conveyor->_seats[%d] stopped cooking and left the shop!\n", self->_id, self->_seat_position);    
    
    pthread_mutex_unlock(&conveyor->_seats_mutex);
}

void sushi_chef_place_food(sushi_chef_t* self, enum menu_item dish) {
    /* 
        MODIFIQUE ESSA FUNÇÃO PARA GARANTIR O COMPORTAMENTO CORRETO E EFICAZ DO SUSHI CHEF.
        NOTAS:
        1.  ✅ O CHEF SÓ PODE ADICIONAR COMIDA NO SLOT À SUA FRENTE
        2.  ✅ O CHEF SÓ PODE ADICIONAR COMIDA EM UM SLOT VAGO
        3.  ✅ O CHEF DEVE ESPERAR ATÉ QUE UM SLOT VAGO APAREÇA PARA POSICIONAR O PRATO NA ESTEIRA
        4.  ❓ CUIDADO COM ERROS DE CONCORRÊNCIA
            ~Arthur: bloqueei o mutex dos assentos, acredito que seja o caminho certo, mas não tenho certeza.
        5.  ✅ NÃO REMOVA OS PRINTS
    */ 

    conveyor_belt_t* conveyor_belt = globals_get_conveyor_belt();
    print_virtual_time(globals_get_virtual_clock());
    fprintf(stdout, GREEN "[INFO]" NO_COLOR " Sushi Chef %d wants to place %u at conveyor->_food_slot[%d]!\n", self->_id, dish, self->_seat_position);

    /* INSIRA SUA LÓGICA AQUI */

    /* ✅ 3 - Chef espera até que um slot vago apareça */ 
    while (TRUE) {
        /* ✅ 2 - Chef adiciona comida somente em um slot vago */
        if (conveyor_belt->_food_slots[self->_seat_position] == -1) {
            /* ❓ 4 - Tomando cuidado com os erros de concorrência */ 
            pthread_mutex_lock(&conveyor_belt->_seats_mutex);
            /* ✅ 1 - Chef só pode adicionar comida no slot à sua frente */
            conveyor_belt->_food_slots[self->_seat_position] = dish;
            print_virtual_time(globals_get_virtual_clock());
            /* ✅ 5 - Prints não foram removidos */
            fprintf(stdout, GREEN "[INFO]" NO_COLOR " Sushi Chef %d placed %u at conveyor->_food_slot[%d]!\n", self->_id, dish, self->_seat_position);
            pthread_mutex_unlock(&conveyor_belt->_seats_mutex);
            break;
        }
    }

    /* INSIRA SUA LÓGICA AQUI */

}

void sushi_chef_prepare_food(sushi_chef_t* self, enum menu_item menu_item) {
    /* NÃO EDITE O CONTEÚDO DESSA FUNÇÃO */
    virtual_clock_t* global_clock = globals_get_virtual_clock();
    switch (menu_item) {
        case Sushi:
            print_virtual_time(globals_get_virtual_clock());
            fprintf(stdout, GREEN "[INFO]" NO_COLOR " Sushi Chef %d started preparing Sushi!\n", self->_id);
            msleep(SUSHI_PREP_TIME/global_clock->clock_speed_multiplier);
            print_virtual_time(globals_get_virtual_clock());
            fprintf(stdout, GREEN "[INFO]" NO_COLOR " Sushi Chef %d finished preparing Sushi!\n", self->_id);
            break;
        case Dango:
            print_virtual_time(globals_get_virtual_clock());
            fprintf(stdout, GREEN "[INFO]" NO_COLOR " Sushi Chef %d started preparing Dango!\n", self->_id);
            msleep(DANGO_PREP_TIME/global_clock->clock_speed_multiplier);
            print_virtual_time(globals_get_virtual_clock());
            fprintf(stdout, GREEN "[INFO]" NO_COLOR " Sushi Chef %d finished preparing Dango!\n", self->_id);
            break;
        case Ramen:
            print_virtual_time(globals_get_virtual_clock());
            fprintf(stdout, GREEN "[INFO]" NO_COLOR " Sushi Chef %d started preparing Ramen!\n", self->_id);
            msleep(RAMEN_PREP_TIME/global_clock->clock_speed_multiplier);
            print_virtual_time(globals_get_virtual_clock());
            fprintf(stdout, GREEN "[INFO]" NO_COLOR " Sushi Chef %d finished preparing Ramen!\n", self->_id);
            break;
        case Onigiri:
            print_virtual_time(globals_get_virtual_clock());
            fprintf(stdout, GREEN "[INFO]" NO_COLOR " Sushi Chef %d started preparing Onigiri!\n", self->_id);
            msleep(ONIGIRI_PREP_TIME/global_clock->clock_speed_multiplier);
            print_virtual_time(globals_get_virtual_clock());
            fprintf(stdout, GREEN "[INFO]" NO_COLOR " Sushi Chef %d finished preparing Onigiri!\n", self->_id);
            break;
        case Tofu:
            print_virtual_time(globals_get_virtual_clock());
            fprintf(stdout, GREEN "[INFO]" NO_COLOR " Sushi Chef %d started preparing Tofu!\n", self->_id);
            msleep(TOFU_PREP_TIME/global_clock->clock_speed_multiplier);
            print_virtual_time(globals_get_virtual_clock());
            fprintf(stdout, GREEN "[INFO]" NO_COLOR " Sushi Chef %d finished preparing Tofu!\n", self->_id);
            break; 
        default:
            fprintf(stdout, RED "[ERROR] Invalid menu_item variant passed to `sushi_chef_prepare_food()`.\n" NO_COLOR);
            exit(EXIT_FAILURE);
    }
}

sushi_chef_t* sushi_chef_init() {
    /* NÃO PRECISA ALTERAR ESSA FUNÇÃO */
    sushi_chef_t* self = malloc(sizeof(sushi_chef_t));
    if (self == NULL) {
        fprintf(stdout, RED "[ERROR] Bad malloc() at `sushi_chef_t* sushi_chef_init()`.\n" NO_COLOR);
        exit(EXIT_FAILURE);
    }
    self->_id = rand() % 1000;
    self->_seat_position = -1;
    pthread_create(&self->thread, NULL, sushi_chef_run, (void *) self);
    return self;
}

void sushi_chef_finalize(sushi_chef_t* self) {
    /* NÃO PRECISA ALTERAR ESSA FUNÇÃO */
    pthread_join(self->thread, NULL);
    free(self);
}

void print_sushi_chef(sushi_chef_t* self) {
    /* NÃO PRECISA ALTERAR ESSA FUNÇÃO */
    print_virtual_time(globals_get_virtual_clock());
    fprintf(stdout, BROWN "[DEBUG] Sushi Chef " NO_COLOR "{\n");
    fprintf(stdout, BROWN "    _id" NO_COLOR ": %d\n", self->_id);
    fprintf(stdout, BROWN "    _seat_position" NO_COLOR ": %d\n", self->_seat_position);
    fprintf(stdout, NO_COLOR "}\n" NO_COLOR);
}
