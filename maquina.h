#ifndef __MAQUINA_H
#define __MAQUINA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Definindo constantes para limites do alfabeto, estados e tamanho da fita
#define MAX_SIMBOLOS 30
#define MAX_ESTADOS 50
#define MAX_FITA 100

// Declarações de estruturas que representam a máquina de Turing
typedef enum { ESQUERDA, DIREITA, INVALIDO } Movimento; // Enum para movimentos da cabeça

typedef struct transicao Transicao; // Estrutura para representar transições
typedef struct simulacao Simulacao;   // Estrutura para simulação
typedef struct maquina Maquina;         // Estrutura para a máquina

// Estrutura principal da máquina de Turing
struct maquina {
  char alfabeto[MAX_SIMBOLOS];          // Alfabeto da máquina
  unsigned int qnt_alfabeto;            // Quantidade de símbolos no alfabeto
  unsigned int qnt_estados;             // Quantidade de estados na máquina
  unsigned int qnt_transicoes;          // Quantidade de transições definidas
  Transicao **transicoes;               // Array de ponteiros para as transições
  Simulacao *simulacao;                 // Ponteiro para a estrutura de simulação
};

// Estrutura que representa a transição na máquina
struct transicao {
  unsigned int estado_inicial;          // Estado atual da máquina
  unsigned int estado_final;            // Estado que será alcançado após a transição
  char simbolo_transicao;               // Símbolo que deve ser lido
  char simbolo_gravar;                  // Símbolo que deve ser gravado na fita
  Movimento movimento;                   // Direção do movimento da cabeça (esquerda ou direita)
};

// Estrutura que representa o estado atual da simulação da máquina
struct simulacao {
  char fita[MAX_FITA];                  // Fita da máquina, onde os símbolos são armazenados
  unsigned int cabeca;                  // Posição da cabeça de leitura/escrita na fita
  unsigned int estado_atual;            // Estado atual da máquina durante a simulação
  unsigned int qnt_entradas;            // Quantidade de entradas a serem testadas
  char **entradas;                      // Array de entradas (palavras) a serem processadas
};

// Função para ler a máquina de um arquivo
Maquina *pegarMaquina(char *caminho_arquivo);

// Função para liberar a memória ocupada pela máquina
void destruirMaquina(Maquina *maquina);

// Função para ler a transição de um arquivo
Transicao *pegarTransicao(FILE *fptr);

// Função para validar a transição lida
int validarTransicao(Transicao *transicao, Maquina *maquina, unsigned int linha,
                    unsigned int mascara_alfabeto);

// Função para iniciar a simulação da máquina
void iniciarSimulacao(Maquina *maquina);

// Função para limpar a fita da máquina
void limparFita(Simulacao *simulacao);

#endif

