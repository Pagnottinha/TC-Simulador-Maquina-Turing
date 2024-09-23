#include <stdio.h>
#include "maquina.h"

int main(void) {
    Maquina* m1 = pegarMaquina("entrada.txt");

    iniciarSimulacao(m1);

    if (m1 != NULL)
        destruirMaquina(m1);
    return 0;
}