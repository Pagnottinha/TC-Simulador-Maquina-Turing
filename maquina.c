#include "maquina.h"

// Função para carregar a máquina de Turing a partir de um arquivo
Maquina* pegarMaquina(char* caminho_arquivo) {
    // Abre o arquivo no caminho fornecido em modo de leitura
    FILE *fptr = fopen(caminho_arquivo, "r");

    // Verifica se o arquivo foi aberto corretamente
    if (fptr == NULL) {
        printf("Não é possível abrir o arquivo.\n");
        return NULL; // Retorna NULL se o arquivo não puder ser aberto
    }

    // Aloca memória para a estrutura da máquina
    Maquina* maquina = (Maquina*) malloc(sizeof(Maquina));

    // Verifica se a alocação de memória foi bem-sucedida
    if (maquina == NULL) {
        printf("Erro ao alocar memória para maquina\n");
        fclose(fptr); // Fecha o arquivo se a alocação falhar
        return NULL;
    }

    // Inicializa os contadores da máquina
    maquina->qnt_alfabeto = 0; // Contador para o alfabeto
    maquina->qnt_estados = 0;  // Contador para os estados
    maquina->qnt_transicoes = 0; // Contador para as transições

    // Variáveis auxiliares
    unsigned int qnt_alfabeto = 0; // Contador local para o alfabeto
    unsigned int mascara_alfabeto = 0; // Máscara para verificar símbolos únicos
    unsigned int qnt_lido = 0; // Contador de caracteres lidos
    char ch; // Variável para armazenar o caractere lido

    // Lê o alfabeto do arquivo
    while ((ch = fgetc(fptr)) != EOF) { // Continua até o final do arquivo
        // Quebra o loop se uma nova linha for encontrada
        if (ch == '\n' || ch == '\r') {
            break; 
        }
        // Verifica se o caractere está fora do intervalo permitido (a-z)
        else if (ch < 97 || ch > 122) {
            printf("Caractere (%c) invalido.\n", ch);
            fclose(fptr); // Fecha o arquivo
            destruirMaquina(maquina); // Libera a memória da máquina
            return NULL; // Retorna NULL
        }

        qnt_lido++; // Incrementa o contador de caracteres lidos

        // Pega a posição do caractere na máscara
        unsigned int index = (ch - 1) % 32; // Índice para a máscara
        unsigned int pos = 1 << index; // Calcula a posição correspondente

        // Verifica se o caractere já foi lido
        if ((mascara_alfabeto & pos) == 0) {
            mascara_alfabeto += pos; // Adiciona o símbolo à máscara

            maquina->alfabeto[qnt_alfabeto] = ch; // Armazena o símbolo no alfabeto
            qnt_alfabeto++; // Incrementa o contador do alfabeto
        }
        else {
            printf("AVISO: simbolo %c foi repetido no alfabeto.\n", ch);
        }

        // Verifica se o limite de símbolos foi ultrapassado
        if (qnt_lido > MAX_SIMBOLOS) {
            printf("Foi ultrapassado o limite de %d simbolos nos alfabetos.\n", MAX_SIMBOLOS);
            fclose(fptr); // Fecha o arquivo
            destruirMaquina(maquina); // Libera a memória da máquina
            return NULL; // Retorna NULL
        }
    }

    // Finaliza a string do alfabeto
    maquina->alfabeto[qnt_alfabeto] = '\0';

    // Lê a quantidade de estados do arquivo
    if (fscanf(fptr, "%d\n", &maquina->qnt_estados) != 1) {
        printf("Falha ao ler a quantidade de estados!\n");
        destruirMaquina(maquina); // Libera a memória da máquina
        fclose(fptr); // Fecha o arquivo
        return NULL; // Retorna NULL
    }

    // Lê a quantidade de transições do arquivo
    if (fscanf(fptr, "%d\n", &maquina->qnt_transicoes) != 1) {
        printf("Falha ao ler a quantidade de transições!\n");
        destruirMaquina(maquina); // Libera a memória da máquina
        fclose(fptr); // Fecha o arquivo
        return NULL; // Retorna NULL
    }

    // Aloca memória para as transições da máquina
    maquina->transicoes = (Transicao**) malloc(sizeof(Transicao) * maquina->qnt_transicoes);

    // Lê as transições do arquivo
    for (unsigned int i = 0; i < maquina->qnt_transicoes; i++) {
        Transicao* transicao = pegarTransicao(fptr); // Pega a transição
        if (transicao == NULL) {
            printf("Transicao %d no formato invalido\n", i + 1);
            destruirMaquina(maquina); // Libera a memória da máquina
            fclose(fptr); // Fecha o arquivo
            return NULL; // Retorna NULL
        }

        // Valida a transição lida
        if (validarTransicao(transicao, maquina, i, mascara_alfabeto) == 0) {
            destruirMaquina(maquina); // Libera a memória da máquina
            fclose(fptr); // Fecha o arquivo
            return NULL; // Retorna NULL
        }

        maquina->transicoes[i] = transicao; // Armazena a transição na máquina
    }

    // Aloca memória para a simulação
    Simulacao* simulacao = malloc(sizeof(Simulacao));
    simulacao->cabeca = 0; // Inicializa a posição da cabeça da fita
    simulacao->estado_atual = 1; // Inicializa o estado atual

    limparFita(simulacao); // Limpa a fita inicial

    // Lê a quantidade de entradas do arquivo
    if (fscanf(fptr, "%d\n", &simulacao->qnt_entradas) != 1) {
        printf("Falha ao ler a quantidade de entradas!\n");
        destruirMaquina(maquina); // Libera a memória da máquina
        fclose(fptr); // Fecha o arquivo
        return NULL; // Retorna NULL
    }

    // Aloca memória para as entradas da simulação
    simulacao->entradas = (char**) malloc(sizeof(char*) * simulacao->qnt_entradas);

    // Lê cada entrada do arquivo
    for (unsigned int i = 0; i < simulacao->qnt_entradas; i++) {
        simulacao->entradas[i] = calloc(MAX_FITA, sizeof(char)); // Aloca memória para a entrada

        unsigned int qnt_entrada = 0; // Contador de caracteres da entrada

        while ((ch = fgetc(fptr)) != EOF) { // Lê caracteres até o final do arquivo
            if (ch == '\n' || ch == '\r') {
                break; // Quebra o loop em nova linha
            }

            // Pega a posição do caractere na máscara
            unsigned int index = (ch - 1) % 32;
            unsigned int pos = 1 << index; // Calcula a posição correspondente

            // Verifica se o caractere está no alfabeto ou se é inválido
            if ((mascara_alfabeto & pos) == 0 || (ch < 97 || ch > 123)) {
                printf("Não tem esse caractere (%c) no alfabeto (%s).\n", ch, maquina->alfabeto);
                fclose(fptr); // Fecha o arquivo
                destruirMaquina(maquina); // Libera a memória da máquina
                return NULL; // Retorna NULL
            }

            simulacao->entradas[i][qnt_entrada] = ch; // Armazena o caractere na entrada
            qnt_entrada += 1; // Incrementa o contador de caracteres da entrada

            // Verifica se o limite de caracteres da fita foi ultrapassado
            if (qnt_entrada > MAX_FITA) {
                printf("Foi ultrapassado o limite de %d simbolos que cabem na fita.\n", MAX_FITA);
                fclose(fptr); // Fecha o arquivo
                destruirMaquina(maquina); // Libera a memória da máquina
                return NULL; // Retorna NULL
            }
        }

        // Verifica se o final do arquivo foi alcançado e se ainda faltam entradas
        if (ch == EOF && i < simulacao->qnt_entradas - 1) {
            printf("Ainda falta %d entradas.\n", simulacao->qnt_entradas - i - 1);
            fclose(fptr); // Fecha o arquivo
            destruirMaquina(maquina); // Libera a memória da máquina
            return NULL; // Retorna NULL
        }

        // Finaliza a string da entrada
        simulacao->entradas[i][qnt_entrada] = '\0';
    }

    // Verifica se há texto adicional no arquivo após ler todas as entradas
    if (fgetc(fptr) != EOF) {
        printf("Leu todas as entradas e ainda tem texto no arquivo.\n");
        fclose(fptr); // Fecha o arquivo
        destruirMaquina(maquina); // Libera a memória da máquina
        return NULL; // Retorna NULL
    }

    // Atribui a simulação à máquina
    maquina->simulacao = simulacao;

    fclose(fptr); // Fecha o arquivo
    return maquina; // Retorna a máquina carregada
}


// Função para liberar a memória alocada na máquina de Turing
void destruirMaquina(Maquina* maquina) {
    // Verifica se a máquina é NULL; se for, não faz nada
    if (maquina == NULL) return;

    // Verifica se há transições alocadas
    if (maquina->transicoes != NULL) {
        // Libera cada transição alocada
        for (unsigned int i = 0; i < maquina->qnt_transicoes; i++) {
            // Verifica se a transição não é NULL antes de liberar
            if (maquina->transicoes[i] != NULL)
                free(maquina->transicoes[i]); // Libera a memória da transição
        }

        // Libera a memória alocada para o array de transições
        free(maquina->transicoes);
    }

    // Verifica se há entradas alocadas na simulação
    if (maquina->simulacao->entradas != NULL) {
        // Libera cada entrada alocada
        for (unsigned int i = 0; i < maquina->simulacao->qnt_entradas; i++) {
            // Verifica se a entrada não é NULL antes de liberar
            if (maquina->simulacao->entradas[i] != NULL)
                free(maquina->simulacao->entradas[i]); // Libera a memória da entrada
        }

        // Libera a memória alocada para o array de entradas
        free(maquina->simulacao->entradas);
    }

    // Libera a memória da estrutura da máquina
    free(maquina);
}

// Função para ler uma transição a partir de um arquivo e alocar memória para ela
Transicao* pegarTransicao(FILE* fptr) {
    // Aloca memória para a estrutura Transicao
    Transicao* transicao = (Transicao*) malloc(sizeof(Transicao));
    char movimentacao = '\0'; // Variável para armazenar o movimento (D ou E)

    // Verifica se a alocação de memória foi bem-sucedida
    if (transicao == NULL) {
        printf("Erro ao alocar memória para transicao\n");
        return NULL; // Retorna NULL se a alocação falhar
    }

    // Lê uma linha do arquivo no formato: estado_inicial simbolo_transicao simbolo_gravar movimentacao estado_final
    // Se a leitura não conseguir extrair 5 valores, libera a memória e retorna NULL
    if (fscanf(fptr, "%d %c %c %c %d\n", &transicao->estado_inicial, 
            &transicao->simbolo_transicao, &transicao->simbolo_gravar, 
            &movimentacao, &transicao->estado_final) != 5) {
        free(transicao); // Libera a memória alocada se a leitura falhar
        return NULL;
    }

    // Define o movimento da transição com base no caractere lido (D para direita, E para esquerda)
    if (movimentacao == 'D') {
        transicao->movimento = DIREITA;
    } else if (movimentacao == 'E') {
        transicao->movimento = ESQUERDA;
    } else {
        transicao->movimento = INVALIDO; // Define como inválido se o movimento for diferente de D ou E
    }

    // Retorna a transição lida e alocada
    return transicao;
}

// Função para validar uma transição em uma máquina
int validarTransicao(Transicao* transicao, Maquina* maquina, unsigned int linha, unsigned int mascara_alfabeto) {
    // Verifica se o estado inicial da transição é válido
    if (transicao->estado_inicial > maquina->qnt_estados) {
        printf("Estado inicial (%d) invalido na transicao %d\n", transicao->estado_inicial, linha + 1);
        return 0; // Retorna 0 se o estado inicial for inválido
    }

    // Verifica se o símbolo de transição é válido, considerando a máscara do alfabeto
    if (((mascara_alfabeto & (1 << (transicao->simbolo_transicao - 1) % 32)) == 0) && transicao->simbolo_transicao != '-') {
        printf("Simbolo de transicao (%c) invalido na transicao %d\n", transicao->simbolo_transicao, linha + 1);
        return 0; // Retorna 0 se o símbolo de transição for inválido
    }

    // Verifica se o símbolo a ser gravado é válido, considerando a máscara do alfabeto
    if (((mascara_alfabeto & (1 << (transicao->simbolo_gravar - 1) % 32)) == 0) && transicao->simbolo_transicao != '-') {
        printf("Simbolo de gravacao (%c) invalido na transicao %d\n", transicao->simbolo_gravar, linha + 1);
        return 0; // Retorna 0 se o símbolo de gravação for inválido
    }

    // Verifica se a movimentação é válida
    if (transicao->movimento == INVALIDO) {
        printf("Movimentacao invalida na linha %d\n", linha + 1);
        return 0; // Retorna 0 se a movimentação for inválida
    }

    // Verifica se o estado final da transição é válido
    if (transicao->estado_final > maquina->qnt_estados) {
        printf("Estado final (%d) invalido na transicao %d\n", transicao->estado_final, linha + 1);
        return 0; // Retorna 0 se o estado final for inválido
    }

    return 1; // Retorna 1 se todas as validações estiverem corretas
}

// Função para limpar a fita na simulação
void limparFita(Simulacao* simulacao) {
    simulacao->cabeca = 0; // Reseta a posição da cabeça da fita

    // Preenche a fita com o caractere de vazio ('-')
    for (int i = 0; i < MAX_FITA; i++) {
        simulacao->fita[i] = '-';
    }
}

// Função que inicia a simulação de uma máquina com múltiplas entradas
void iniciarSimulacao(Maquina* maquina) {
    // Percorre todas as entradas
    for (unsigned int i = 0; i < maquina->simulacao->qnt_entradas; i++) {
        limparFita(maquina->simulacao); // Realiza a limpeza da fita para iniciar a cada entrada

        // Adiciona na fita cada caractere de cada entrada        
        for (unsigned int j = 0; maquina->simulacao->entradas[i][j] != '\0'; j++) {
            maquina->simulacao->fita[maquina->simulacao->cabeca + j] = maquina->simulacao->entradas[i][j];
        }

        unsigned int fim = 0;        
        maquina->simulacao->estado_atual = 1; // Ao iniciar define sempre o primeiro estado

        while (!fim) {
            char simbolo_lido = maquina->simulacao->fita[maquina->simulacao->cabeca];

            // Verifica se a máquina está no estado de parada (estado 5)
            if (maquina->simulacao->estado_atual == 5) {
                printf("%d: %s OK\n", i + 1, maquina->simulacao->entradas[i]);
                fim = 1; // Finaliza a simulação para essa entrada
                continue;  
            }

            Transicao* transicao_aplicavel = NULL;

            // Encontra a transição apropriada com base no estado atual e no símbolo lido
            for (unsigned int k = 0; k < maquina->qnt_transicoes; k++) {
                Transicao* transicao = maquina->transicoes[k];
                // Verifica se a transição se aplica ao estado atual e ao símbolo lido
                if (transicao->estado_inicial == maquina->simulacao->estado_atual &&
                    transicao->simbolo_transicao == simbolo_lido) {
                    transicao_aplicavel = transicao; // Armazena a transição aplicável
                    break;
                }
            }

            if (transicao_aplicavel != NULL) {
                // Grava o novo símbolo na fita
                maquina->simulacao->fita[maquina->simulacao->cabeca] = transicao_aplicavel->simbolo_gravar;

                // Move a cabeça da máquina
                if (transicao_aplicavel->movimento == DIREITA) {
                    maquina->simulacao->cabeca++; // Move para a direita
                } else if (transicao_aplicavel->movimento == ESQUERDA) {
                    maquina->simulacao->cabeca--; // Move para a esquerda
                }

                // Atualiza o estado da máquina
                maquina->simulacao->estado_atual = transicao_aplicavel->estado_final;

            } else {
                // Caso não encontre uma transição aplicável, finaliza a simulação com erro
                printf("%d: %s not OK\n", i + 1, maquina->simulacao->entradas[i]);
                fim = 1;  // Termina a simulação em caso de erro
            }
        }
    }
}
