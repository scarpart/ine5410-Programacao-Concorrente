1. Multiprocessadores

Todos os processadores acessam uma memória compartilhada e a comunicação é feita via compartilhamento de memória. Exemplo: multicores.

Duas formas de organizar a memória:

		- UMA - Uniform Memory Acess
		- NUMA - Non Uniform Memory Acess

**UMA**

A memória é centralizada e o acesso à memória leva tempo constante. O problema é o limite físico da interconexão.

**NUMA**

A memória é distribuída entre os processadores. O tempo de acesso depende se a memória é local ou remota. Processadores podem acessar outros blocos de memória de outros processadores. Causa menor contenção.

2. Aceleradores

Arquiteturas usadas em conjuntos com CPU para acelerar computação. 

3. Multicomputadores (NO Remote Memory Acess (NORMA))

Computadores inteiros replicados: cada um com a própria memória. Comunicação é feita via rede.

Subclasses de NORMA:
		- NOW: Network of Workstations. Desktops interligados, rede tradicional de baixo custo, ambientes oportunistas. 
		- COW: Clusters of Workstations. Como COW para muito alto desempenho. Redes de baixa latência, componentes redundantes.
		- Grid: máquina cmoposta por componentes distribuídos geograficamente. Controle descentralizado. Interfaces e protocolos padronnizados. 
		- Cloud/Nuvem: similar à computação em grade. Recursos são utilizados sob demanda, pago de acordo com uso. Recursos virtualizados. 
