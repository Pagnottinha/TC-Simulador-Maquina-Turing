#include "maquina.h"

#define MAX_SIMBOLOS 30
#define MAX_ESTADOS 50
#define MAX_FITA 100

typedef struct transicao Transicao;
typedef struct simulacao Simulacao;

struct maquina {
    // unsigned int qnt_alfabeto_entrada;
    // char alfabeto_entrada[MAX_SIMBOLOS];
    // unsigned int qnt_alfabeto_fita;
    // char alfabeto_fita[MAX_SIMBOLOS];
    char alfabeto[MAX_SIMBOLOS];
    unsigned int qnt_alfabeto;
    unsigned int qnt_estados;
    unsigned int qnt_transicoes;
    Transicao** transicoes;
    Simulacao* simulacao;
};

struct transicao
{
    unsigned int estado_inicial;
    unsigned int estado_final;
    char simbolo_transicao;
    char simbolo_gravar;
    Movimento movimento;
};

struct simulacao
{
    char fita[MAX_FITA];
    unsigned int cabeca;
    unsigned int estado_atual;
    unsigned int qnt_entradas;
    char** entradas; 
};

Transicao* pegarTransicao(FILE* fptr);
int transicaoValida(Transicao* transicao, Maquina* maquina, unsigned int linha, unsigned int mascara_alfabeto);
void printTransicoes(Maquina* maquina);

void printarFita(Maquina* maquina);
void limparFita(Simulacao* simulacao);

Maquina* pegarMaquina(char* caminho_arquivo) {
    FILE *fptr = fopen(caminho_arquivo, "r");

    if (fptr == NULL) {
        printf("Não é possível abrir o arquivo.\n");
        return NULL;
    }

    Maquina* maquina = (Maquina*) malloc(sizeof(Maquina));

    if (maquina == NULL) {
        printf("Erro ao alocar memória para maquina\n");
        fclose(fptr);
        return NULL;
    }

    maquina->qnt_alfabeto = 0;
    maquina->qnt_estados = 0;
    maquina->qnt_transicoes = 0;

    unsigned int qnt_alfabeto = 0;
    unsigned int mascara_alfabeto = 0;
    unsigned int qnt_lido = 0;
    char ch;
    while ((ch = fgetc(fptr)) != EOF) {
        if (ch == '\n') {
            break;
        }
        else if (ch < 97 || ch > 123) {
            printf("Caractere (%c) invalido.\n", ch);
            fclose(fptr);
            destruirMaquina(maquina);
            return NULL;
        }

        qnt_lido++;

        // pegar a posição do char na mascara
        unsigned int index = (ch - 1) % 32;
        unsigned int pos = 1 << index;

        // printf("%c: %d %d\n", ch, pos, mascara & pos);
        if ((mascara_alfabeto & pos) == 0) {
            mascara_alfabeto += pos;

            maquina->alfabeto[qnt_alfabeto] = ch;
            qnt_alfabeto++;
        }
        else {
            printf("AVISO: simbolo %c foi repetido no alfabeto.\n", ch);
        }


        if (qnt_lido > MAX_SIMBOLOS) {
            printf("Foi ultrapassado o limite de %d simbolos nos alfabetos.\n", MAX_SIMBOLOS);
            fclose(fptr);
            destruirMaquina(maquina);
            return NULL;
        }
    }

    maquina->alfabeto[qnt_alfabeto] = '\0';

    printf("Alfabeto (%d): %s\n", qnt_alfabeto, maquina->alfabeto);

    if (fscanf(fptr, "%d\n", &maquina->qnt_estados) != 1) {
        printf("Falha ao ler a quantidade de estados!\n");
        destruirMaquina(maquina);
        fclose(fptr);
        return NULL;
    }

    printf("Estados: %d\n", maquina->qnt_estados);

    if (fscanf(fptr, "%d\n", &maquina->qnt_transicoes) != 1) {
        printf("Falha ao ler a quantidade de transições!\n");
        destruirMaquina(maquina);
        fclose(fptr);
        return NULL;
    }

    maquina->transicoes = (Transicao**) malloc(sizeof(Transicao) * maquina->qnt_transicoes);

    for (unsigned int i = 0; i < maquina->qnt_transicoes; i++) {
        Transicao* transicao = pegarTransicao(fptr);
        if (transicao == NULL) {
            printf("Transicao %d no formato invalido\n", i + 1);
            destruirMaquina(maquina);
            fclose(fptr);
            return NULL;
        }

        if (transicaoValida(transicao, maquina, i, mascara_alfabeto) == 0) {
            destruirMaquina(maquina);
            fclose(fptr);
            return NULL;
        }

        maquina->transicoes[i] = transicao;

    }

    printTransicoes(maquina);

    Simulacao* simulacao = malloc(sizeof(Simulacao));
    simulacao->cabeca = 0;
    simulacao->estado_atual = 1;
    
    limparFita(simulacao);

    if (fscanf(fptr, "%d\n", &simulacao->qnt_entradas) != 1) {
        printf("Falha ao ler a quantidade de entradas!\n");
        destruirMaquina(maquina);
        fclose(fptr);
        return NULL;
    }

    simulacao->entradas = (char**) malloc(sizeof(char*) * simulacao->qnt_entradas);
    
    for (unsigned int i = 0; i < simulacao->qnt_entradas; i++) {
        simulacao->entradas[i] = calloc(MAX_FITA, sizeof(char));

        unsigned int qnt_entrada = 0;
        qnt_lido = 0;
        while ((ch = fgetc(fptr)) != EOF) {
            if (ch == '\n') {
                break;
            }

            qnt_lido++;

            // pegar a posição do char na mascara
            unsigned int index = (ch - 1) % 32;
            unsigned int pos = 1 << index;

            // printf("%c: %d %d\n", ch, pos, mascara & pos);
            if ((mascara_alfabeto & pos) == 0 || (ch < 97 || ch > 123)) {
                printf("Não tem esse caractere (%c) no alfabeto (%s).\n", ch, maquina->alfabeto);
                fclose(fptr);
                destruirMaquina(maquina);
                return NULL;
            }

            simulacao->entradas[i][qnt_entrada] = ch;
            qnt_entrada += 1;

            if (qnt_lido > MAX_FITA) {
                printf("Foi ultrapassado o limite de %d simbolos que cabem na fita.\n", MAX_FITA);
                fclose(fptr);
                destruirMaquina(maquina);
                return NULL;
            }
        }

        if (ch == EOF && i < simulacao->qnt_entradas - 1) {
            printf("Ainda falta %d entradas.\n", simulacao->qnt_entradas - i - 1);
            fclose(fptr);
            destruirMaquina(maquina);
            return NULL;
        }
        else if (ch != EOF && i == simulacao->qnt_entradas - 1) {
            printf("Leu todas as entradas e ainda tem texto no arquivo.\n");
            fclose(fptr);
            destruirMaquina(maquina);
            return NULL;
        }

        simulacao->entradas[qnt_entrada] = '\0';
    }

    maquina->simulacao = simulacao;

    fclose(fptr);
    return maquina;
}

void destruirMaquina(Maquina* maquina) {
    if (maquina == NULL) return;

    if (maquina->transicoes != NULL) {
        for (unsigned int i = 0; i < maquina->qnt_transicoes; i++) {
            if (maquina->transicoes[i] != NULL)
                free(maquina->transicoes[i]);
        }

        free(maquina->transicoes);
    }

    if (maquina->simulacao->entradas != NULL) {
        for (unsigned int i = 0; i < maquina->simulacao->qnt_entradas; i++) {
            if (maquina->simulacao->entradas[i] != NULL)
                free(maquina->simulacao->entradas[i]);
        }

        free(maquina->simulacao->entradas);
    }
        
    
    free(maquina);
}

Transicao* pegarTransicao(FILE* fptr) {
    Transicao* transicao = (Transicao*) malloc(sizeof(Transicao));
    char movimentacao = '\0';

    if (transicao == NULL) {
        printf("Erro ao alocar memória para transicao\n");
        return NULL;
    }

    if (fscanf(fptr, "%d %c %c %c %d\n", &transicao->estado_inicial, &transicao->simbolo_transicao, &transicao->simbolo_gravar, &movimentacao, &transicao->estado_final) != 5) {
        free(transicao);
        return NULL;
    }

    if (movimentacao == 'D') {
        transicao->movimento = DIREITA;
    }
    else if (movimentacao == 'E') {
        transicao->movimento = ESQUERDA;
    }
    else {
        transicao->movimento = INVALIDO;
    }

    return transicao;
}

int transicaoValida(Transicao* transicao, Maquina* maquina, unsigned int linha, unsigned int mascara_alfabeto) {
    if (transicao->estado_inicial > maquina->qnt_estados) {
        printf("Estado inicial (%d) invalido na transicao %d\n", transicao->estado_inicial, linha + 1);
        return 0;
    }

    if ((mascara_alfabeto & (transicao->simbolo_transicao % 32) == 0) && transicao->simbolo_transicao != '-') {
        printf("Simbolo de transicao (%c) invalido na transicao %d\n", transicao->simbolo_transicao, linha + 1);
        return 0;
    }

    if ((mascara_alfabeto & (transicao->simbolo_gravar % 32) == 0) && transicao->simbolo_transicao != '-') {
        printf("Simbolo de gravacao (%c) invalido na transicao %d\n", transicao->simbolo_gravar, linha + 1);
        return 0;
    }

    if (transicao->movimento == INVALIDO) {
        printf("Movimentacao invalida na linha %d\n", linha + 1);
        return 0;
    }

    if (transicao->estado_final > maquina->qnt_estados) {
        printf("Estado final (%d) invalido na transicao %d\n", transicao->estado_final, linha + 1);
        return 0;
    }
    return 1;
}

void printTransicoes(Maquina* maquina) {
    printf("Transicoes:\n");

    for (unsigned int i = 0; i < maquina->qnt_transicoes; i++) {
        Transicao* transicao = maquina->transicoes[i];

        if (transicao != NULL)
            printf("f(q_%d, %c) = (%c, %d, q_%d)\n", transicao->estado_inicial, transicao->simbolo_transicao, transicao->simbolo_gravar, transicao->movimento, transicao->estado_final);
    }
}

void limparFita(Simulacao* simulacao) {
    for (int i = 0; i < MAX_FITA; i++) {
        simulacao->fita[i] = '-';
    }
}

void printarFita(Maquina* maquina) {
    Simulacao* simulacao = maquina->simulacao;

    printf("Fita: ");
    for(int i = 0; i < MAX_FITA; i++) {
        printf("%c", simulacao->fita[i]);
    }

    printf("\n");
}

void iniciarSimulacao(Maquina* maquina) {
    
    // Percorre todas as entradas
    for (unsigned int i = 0; i < maquina->simulacao->qnt_entradas; i++) {
        limparFita(maquina->simulacao); // Realiza a limpeza da fita para iniciar a cada entrada
        printf("Entrada: %s\n", maquina->simulacao->entradas[i]);

        printf("Antes ");
        printarFita(maquina);

        // Adiciona na fita cada caractere de cada entrada        
        for (unsigned int j = 0; maquina->simulacao->entradas[i][j] != '\0'; j++) {
            maquina->simulacao->fita[maquina->simulacao->cabeca + j] = maquina->simulacao->entradas[i][j];
        }

        printf("Depois ");
        printarFita(maquina);

        unsigned int fim = 0;        
        maquina->simulacao->estado_atual = 1; // Ao iniciar define sempre o primeiro estado

        while (!fim) {
            char simbolo_lido = maquina->simulacao->fita[maquina->simulacao->cabeca];

            // Verifica se a máquina está no estado de parada (estado 5)
            if (maquina->simulacao->estado_atual == 5) {
                printf("Máquina atingiu o estado de parada (5).\n");
                fim = 1;
                continue;  // Finaliza a simulação para essa entrada
            }

            Transicao* transicao_aplicavel = NULL;

            // Encontra a transição apropriada com base no estado atual e no símbolo lido
            for (unsigned int k = 0; k < maquina->qnt_transicoes; k++) {
                Transicao* transicao = maquina->transicoes[k];
                if (transicao->estado_inicial == maquina->simulacao->estado_atual &&
                    transicao->simbolo_transicao == simbolo_lido) {
                    transicao_aplicavel = transicao;
                    break;
                }
            }

            if (transicao_aplicavel != NULL) {
                // Grava o novo símbolo na fita
                maquina->simulacao->fita[maquina->simulacao->cabeca] = transicao_aplicavel->simbolo_gravar;

                // Move a cabeça da máquina
                if (transicao_aplicavel->movimento == DIREITA) {
                    maquina->simulacao->cabeca++;
                } else if (transicao_aplicavel->movimento == ESQUERDA) {
                    maquina->simulacao->cabeca--;
                }

                // Atualiza o estado da máquina
                maquina->simulacao->estado_atual = transicao_aplicavel->estado_final;

            } else {
                printf("Erro: Nenhuma transição aplicável no estado %d com o símbolo '%c'\n", 
                        maquina->simulacao->estado_atual, simbolo_lido);
                fim = 1;  // Termina a simulação em caso de erro
            }

        }
        
        printarFita(maquina);  // Mostra o estado final da fita após todas as transições
    }
}