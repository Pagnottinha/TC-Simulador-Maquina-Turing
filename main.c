#include <stdio.h> 
#include "maquina.h" // Inclui o cabeçalho que contém a definição da estrutura Maquina e suas funções associadas.
 
int main(void) {
    // Chama a função pegarMaquina para ler a máquina de Turing a partir de um arquivo.
    Maquina *m1 = pegarMaquina("entrada.txt");

    // Inicia a simulação da máquina de Turing.
    iniciarSimulacao(m1);

    if (m1 != NULL)
        destruirMaquina(m1); // Se a máquina foi criada, libera a memória alocada para a mesma após a finalização da simulação.

    return 0; // Retorna 0 para finalizar o programa
}
