#ifndef __MAQUINA_H
#define __MAQUINA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    ESQUERDA,
    DIREITA,
    INVALIDO
} Movimento;

typedef struct maquina Maquina;

Maquina* pegarMaquina(char* caminho_arquivo);
void destruirMaquina(Maquina* maquina);
void iniciarSimulacao(Maquina* maquina);

#endif