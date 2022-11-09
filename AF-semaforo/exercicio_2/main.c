#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>

/* ---------- Definições Globais. ---------- */
#define TEMPO_BASE 1000000

typedef enum { 
	CONTINENTE, 
	ILHA 
} cabeceira_t;

typedef struct {
	int id;
	cabeceira_t cabeceira;
	pthread_t thread;
} veiculo_t;
/* ---------------------------------------- */

/* ---------- Variáveis Globais. ---------- */
char cabeceiras[2][11] = { { "CONTINENTE" }, { "ILHA" } };
int total_veiculos;
int veiculos_turno;

// ToDo: Adicione aque quaisquer outras variávels globais necessárias.
sem_t continente_ilha, ilha_continente;
int carros_passados;
int cabeceira_atual;
/* ---------------------------------------- */


/* Inicializa a ponte. */
void ponte_inicializar(int total_veiculos, int veiculos_turno) {
	
	// ToDo: IMPLEMENTAR!
	sem_init(&continente_ilha, 0, veiculos_turno);
	sem_init(&ilha_continente, 0, 0);
	
	cabeceira_atual = 0;

	/* Imprime direção inicial da travessia. NÃO REMOVER! */
	printf("\n[PONTE] *** Novo sentido da travessia: CONTINENTE -> ILHA. ***\n\n");
	fflush(stdout);
}

/* Função executada pelo veículo para ENTRAR em uma cabeceira da ponte. */
void ponte_entrar(veiculo_t *v) {
	// ToDo: IMPLEMENTAR!

	if (v->cabeceira == 0)
		sem_wait(&continente_ilha);
	else if (v->cabeceira == 1)
		sem_wait(&ilha_continente);
}

/* Função executada pelo veículo para SAIR de uma cabeceira da ponte. */
void ponte_sair(veiculo_t *v) {
	
	carros_passados++;

	// ToDo: IMPLEMENTAR!
	if (carros_passados == veiculos_turno) {
		printf("\n[PONTE] *** Novo sentido da travessia: %s -> %s. ***\n\n", cabeceiras[v->cabeceira], cabeceiras[!v->cabeceira]);
		fflush(stdout);

		if (cabeceira_atual == 0)
			for (int i = 0; i < carros_passados; i++)
				sem_post(&ilha_continente);
		else
			for (int j = 0; j < carros_passados; j++)
				sem_post(&continente_ilha);	

		cabeceira_atual = !cabeceira_atual;
		carros_passados = 0;
	}
	
}

/* FINALIZA a ponte. */
void ponte_finalizar() {

	// ToDo: IMPLEMENTAR!
	sem_destroy(&continente_ilha);
	sem_destroy(&ilha_continente);

	/* Imprime fim da execução! */
	printf("[PONTE] FIM!\n\n");
	fflush(stdout);
}

/* Implementa o comportamento de um veículo. */
void * veiculo_executa(void *arg) {
	veiculo_t *v = (veiculo_t *) arg;
	
	printf("[Veiculo %3d] Aguardando para entrar na ponte pelo(a) %s.\n", v->id, cabeceiras[v->cabeceira]);
	fflush(stdout);

	/* Entra na ponte. */
	ponte_entrar(v);
	printf("[Veiculo %3d] ENTROU na ponte pelo(a) %s.\n", v->id, cabeceiras[v->cabeceira]);
	fflush(stdout);

	/* Faz a travessia. */
	usleep(TEMPO_BASE + rand() % 1000);
	
	/* Seta cabeceira oposta para sair. */
	v->cabeceira = !v->cabeceira;

	/* Sai da ponte na cabeceira oposta. */
	printf("[Veiculo %3d] SAIU da ponte pelo(a) %s.\n", v->id, cabeceiras[v->cabeceira]);
	fflush(stdout);
	ponte_sair(v);

	pthread_exit(NULL);
}

/* Função principal: NÃO PODE ALTERAR! */
int main(int argc, char **argv) {
 
	if (argc < 3) {
		printf("Indique a quantidade total de veiculos e o numero de veiculos que podem atravessar a ponte por turno:\n\n %s [QTD_TOTAL_VEICULOS] [VEICULOS_POR_TURNO]\n\n", argv[0]);
		return 1;
	}

	total_veiculos = atoi(argv[1]);
	veiculos_turno = atoi(argv[2]);

	if (total_veiculos % (veiculos_turno * 2)) {
		printf("ERRO: O valor [QTD_TOTAL_VEICULOS] deve ser divisivel por ([VEICULOS_POR_TURNO] * 2)!\n\n %s [QTD_TOTAL_VEICULOS] [VEICULOS_POR_TURNO]\n\n", argv[0]);
		return 1;
	}

	srand(time(NULL));

	/* Aloca os veículos. */
	veiculo_t veiculos[total_veiculos];

	ponte_inicializar(total_veiculos, veiculos_turno);

	/* Cria os veículos. */
	for (int i = 0; i < total_veiculos; i++) {
		/* Define o id do veículo. */
		veiculos[i].id = i;
		
		/* Escolhe aleatoreamente se o veículo entra pela ILHA ou CONTINENTE. */
		veiculos[i].cabeceira = i % 2; 
		
		/* Cria a thread veículo. */
		pthread_create(&veiculos[i].thread, NULL, veiculo_executa, (void *) &veiculos[i]);		
	}

	/* Aguarda o término da execução de todos os veículos antes de finalizar o programa. */
	for (int i = 0; i < total_veiculos; i++) {
		pthread_join(veiculos[i].thread, NULL);
	}

	ponte_finalizar();

	return 0;
}