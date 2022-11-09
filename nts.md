### Processos

A CPU executa vários processos ao longo de 1 segundo, mas sempre apenas 1 por vez.
Todos os softwares que podem executar em um computador são organizados em processos. 

Um processo é um programa em execução acompanhado dos valores atuais do contador de programa, dos registradores e das variáveis.
Cada processor tem sua CPU virtual. 

Quando cada processo executa, seu contador de programa lógico é carregado no contador de programa real. 
Quando acaba o tempo de CPU alocado para um processo, o contador de programa físico é salvo no contador de programa lógico do processo de memória.

Processos são criados e destruídos através de chamadas de sistema, depende do SO.

Perfis de processos:
	CPU-bound: tempo de execução do processo é definido pelos ciclos de processador.
	Memory-bound: tempo de execução do processo é definido principalmente pelo tempo de acesso à memória.
	I/O-bound: tempo de execução é definido por operações I/O

Estados de um processo:
1. Criação: Quando um usuário executa um programa o SO cria um processos através de chamadas de sistema. O processo recebe uma identificação única PID.
2. Pronto: Após ser criado um processo passa para esse estado e pode ser executado pela CPU. Podem existir muitos processos em "Pronto", então são organizados em uma fila. O Escalonador de Processos vai selecionar um deles para ser exutado na CPU. A escolha depende do algoritmo de escalonamento. Cada um tem seus critérios.
3. Em Execução: quando um processo é escalonado, ele vem para esse estado. Permanece em execução até que termine ou ocorra uma preempção (SO pausa temporariamente o processo, coloca em "Pronto" e seleciona outro processo para executar). A preempção depende do algoritmo de escalonamento. Geralmente ocorre quando o tempo de CPU alocado acaba, ou quando um processo de maior prioridade fica disponível em pronto. 
4. Bloqueado: O processo passa para esse estado ao usar uma chamada de sistema. Processos bloqueados são armazenados em filas e cada dispositivo de I/O pode possuir uma fila diferente. Chamada de Sistema: quando um processo requisita alguma operação de I/O. Ao final da execução do serviço é gerada uma interrupção, informando ao SO que acabou. 
5. Destruição: Quando um processo está em execução termina, ou por intervenção de um outro processo. Nesse estado a memória e outros recursos alocados para o processo são desalocados. 

Implementação de Processos:
O SO mantém uma tabela de processos. Uma entrada na tabela é chamada de descritor de processo. Contém informações sobre o contexto do processo. Contexto do processo: valores de registradores, program counter, estado do processo, etc.

Processos no Linux:
São criados através do `fork()`. Um processo é criado quando o programa é executado (processo pai), que pode criar processos filhos. O processo filho é uma cópia do pai e ambos vão executar o mesmo código após a chamada fork(). 
A syscall `wait()`, quando for -1, o último filho do processo terminou. Quando for > 0, um processo terminou mas ainda existem outros em execução e então o PID do filho que terminou é retornado. Para esperar que todos terminem: `while(wait(NULL) > 0);`

### COMPARTILHAMENTO DE DADOS
Processos não compartilham o mesmo espaço de endereçamento, logo, processos pai e filho terão cópias idênticas dos dados após um fork() mas não há compartilhamento de variáveis globais entre processos pai/filho criados a partir de uma chamada fork(): cada processo terá uma cópia das variáveis globais no seu próprio espaço de endereçamento. 

## Threads
Permitem que múltiplas linhas de execução no mesmo ambiente de um processo, com certo grau de independência. A criação de threads é mais fácil que a de processos.  
Threads de um mesmo processo compartilham variáveis globais. 

Processos alocam:
	- Espaço de endereçamento. Todas as threads de um processo utilizam esse mesmo espaço.
	- Variáveis globais, contexto de processo.
	- Arquivos abertos, contexto de processo.

Threads alocam:
	- Contador de programa (PC)
	- Registradores
	- Pilha
	- Estado

Existem 3 formas de implementar threads:

1. Implementação em espaço de usuário
O núcleo do SO não fornece abstração de threads. A implementação é feita através de bibliotecas de usuário. Funções da API de threads são invocadas em espaço de usuário ( ao invés de syscalls ). O núcleo gerencia processos singlethread. Tabela que gerencia threads -> espaço de usuário. Tabela que genrencia processos -> espaço de kernel.
Vantagens:
	- Pode ser implementado em SOs sem suporte a threads.
	- Chaveamento entre threads é mais rápido e não é necessário passar para o modo núcleo.
	- Cada processo pode ter seu próprio algoritmo de escalonamento.
Desvantagens:
	- Chamadas de sistema por threads bloqueiam o processo como um todo. 

2. Implementação em espaço de núcleo
O núcleo do SO fornece uma abstração de threads. O código e estruturas de dados da impelmentação de threads residem no espaço de núcleo. Funções da API de threds são invocadas através de chamadas de sistema. 

Vantagens:
	- SO pode escalonar threads individualmente, como processos.
	- Melhor eficiência de hardware.
	- Uma syscall realiazada por thread não bloqueia as demais.
Desvantagens:
	- Maior custo para chaveamento entre threds.

Implementação híbrida:
Diversas threads de usuário podem ser multiplexadas em uma thread de núcleo. SO realiza escalonamento de threads de núcleo. Quando a thread de núcleo é multiplexada somente uma thread de usuário pode estar em execução nela.

Vantagens:
	- Maior flexibilidade.
Desvantagens:
	- Maior complexidade.
	- Sobrecusto maior.

### POSIX Threads
Portable Operating System Interface. Define uma API para compatibilidade de software com variantes Unix. 
O SO cria um processo quando um programa é iniciado. Esse processo contém uma thread principal que poderá criar threads filhas ou trabalhadoras. Threads trabalhadoras podem criar outras threads. 
O linux usa threads em espaço de núcleo, que são escalonadas individualmente. 

Threads podem compartilhar dados na memória. Isso pode causar problemas (condições de corrida).
Um programa possui uma condição de corrida quando duas ou mais threads alteram um mesmo conjunto de dados concorrentemente. O resultado depende da ordem de acesso. 
Mecanismos de exclusão mútua são usados para proteger regiões críticas.

Mecanismos de exclusão mútua:
	- Spin-locks
	- Mutexes
	- Semáforos

1. Spin-lock
Mecanismo de sincronização baseado em espera ocupada (busy waiting). O processo ou thread que quer acessar a região crítica fica em loop enquanto a região crítica está sendo acessada por outro. Utiliza instrução específica de hardware. A região crítica é protegida por uma variável em memória (LOCK). Inicialmente LOCK = 0. Se LOCK = 0, região crítica = livre. Se for 1, está ocupada a região crítica. O problema é a espera ocupada, que usa desnecessáriamente o processador.

2. Mutex
lock(m): solicita acesso à região crítica.
unlcok(m): libera a região crítica.

Lock: Se o mutex está livre, então marca-se este como ocupado. Caso contrário, bloqueia a thread e a insere no fim da fila do mutex m. 
Unlock: Se a fila de m está vazia, então marca m como livre. Senão, libera a thread do início da fila do mutex m.

Para que o mutex funcione, lock() e unlock() precisam ser atômicas, ou seja, não pode ser permitido que mais de uma thread utilize lock() no mesmo mutex de maneira simultânea. Então os mutexes usam spin-locks para garantir atomicidade na execução dessas funções. Spin-locks são para garantir a atomicidade, não para esperar acesso à região crítica. 

3. Semáforo
P(S): solicita acesso à região protegida por semáforo.
V(S): libera o semáforo.

P(S): Se o valor de S > 0, então S--, caso contrário bloqueia a thread e a insere no fim da fila do semáforo S.
V(S): Se a fila de S for vazia, então S++, caso contrário libera a thread do início da fila do semáforo S.

Uma diferença entre semáforos e mutex é que mutex só permite 2 estados. 

Semáforos podem ser:
- Binários: Funciona da mesma forma que um mutex, com valores somente 0 e 1. Usado para implementar exclusão mútua. 
- Contador: Assume valores maiores ou iguais a 0. N indica a quantidade de threads que podem "passar" pelo semáforo em um dado instante. Usado para a sincronização entre threads.

P(S) e V(S) precisam ser atômicas e utilizam spin-locks para garantir isso.
