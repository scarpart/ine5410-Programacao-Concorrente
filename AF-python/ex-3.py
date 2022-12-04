from multiprocessing import Process, Queue

def origem(fila1):
  for i in range(10):
    msg = 'Teste ' + str(i)
    # envia mensagem para a ponte pela fila 1
    print('Mensagem enviada:', msg)

def ponte(fila1, fila2):
  for i in range(10):
    # recebe mensagem na fila 1
    # reenvia mensagem na fila 2
    print('Mensagem encaminhada:', msg)

def destino(fila2):
  for i in range(10):
    # recebe mensagem na fila 2
    print('Mensagem recebida:', msg)

if __name__ == '__main__':
  fila1 = Queue()
  fila2 = Queue()
  o = Process(target=origem, args=(fila1,))
  p = Process(target=ponte, args=(fila1,fila2))
  d = Process(target=destino, args=(fila2,))
  # executa os processos
  # aguarda conclusão
  # libera as filas