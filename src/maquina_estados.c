// Arquivo: maquina_estados.c (VERSÃO REATORADA E CORRETA)

#define _DEFAULT_SOURCE
#include "maquina_estados.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Reinicia a máquina para um novo token
void reiniciar_maquina(MaquinaEstados* maq) {
    maq->estado_atual = maq->estado_inicial;
    
    // Limpa o lexema anterior, se existir
    if (maq->lexema) {
        maq->lexema[0] = '\0';
    } else {
        // Aloca um buffer inicial para o lexema
        maq->lexema = malloc(16 * sizeof(char));
        maq->lexema[0] = '\0';
    }
    maq->tamanho_lexema = 0;
    
    maq->status = MAQ_INICIAL;
}

// Processa um caractere e atualiza o estado
void transicionar_maquina(MaquinaEstados* maq, char c, int eof) {
    // A máquina para de processar se já atingiu um estado final (sucesso ou erro)
    if (maq->status == MAQ_SUCESSO || maq->status == MAQ_ERRO) {
        return;
    }

    // Se a máquina estava no estado inicial, agora está executando
    if (maq->status == MAQ_INICIAL) {
        maq->status = MAQ_EXECUTANDO;
    }

    // Encontra a função de transição para o estado atual
    FuncaoTransicao funcao_transicao = NULL;
    for (int i = 0; i < maq->num_transicoes; i++) {
        if (strcmp(maq->transicoes[i].estado, maq->estado_atual) == 0) {
            funcao_transicao = maq->transicoes[i].funcao_transicao;
            break;
        }
    }

    if (!funcao_transicao) {
        maq->status = MAQ_ERRO;
        return;
    }

    // Executa a transição para obter o próximo estado
    const char* proximo_estado = funcao_transicao(c, eof);
    maq->estado_atual = proximo_estado;
    
    // Anexa o caractere ao lexema ANTES de verificar o estado final
    // Apenas se não for fim de arquivo
    if (!eof) {
        int len = maq->tamanho_lexema;
        // Realoca o buffer do lexema se necessário
        if (len + 1 >= 15) { // Exemplo de tamanho de buffer
            maq->lexema = realloc(maq->lexema, (len + 16) * sizeof(char));
        }
        maq->lexema[len] = c;
        maq->lexema[len + 1] = '\0';
        maq->tamanho_lexema++;
    }

    // Verifica se o novo estado é morto
    for (int i = 0; i < maq->num_estados_mortos; i++) {
        if (strcmp(maq->estado_atual, maq->estados_mortos[i]) == 0) {
            maq->status = MAQ_ERRO;
            return;
        }
    }

    // Verifica se o novo estado é final
    for (int i = 0; i < maq->num_estados_finais; i++) {
        if (strcmp(maq->estado_atual, maq->estados_finais[i]) == 0) {
            maq->status = MAQ_SUCESSO;
            return; // Para de processar assim que atinge o sucesso
        }
    }
}

// Verifica se o estado de sucesso atual requer retrocesso
bool deve_retroceder_cursor(MaquinaEstados* maq) {
    if (maq->status != MAQ_SUCESSO) return false;
    
    for (int i = 0; i < maq->num_estados_retrocesso; i++) {
        if (strcmp(maq->estado_atual, maq->estados_retrocesso[i]) == 0) {
            return true;
        }
    }
    return false;
}

// Retorna o status atual da máquina
EstadoMaquina obter_status_maquina(MaquinaEstados* maq) {
    return maq->status;
}

// Retorna o lexema construído pela máquina
char* obter_lexema_maquina(MaquinaEstados* maq) {
    // Se o token terminou em um estado de retrocesso, removemos o último caractere do lexema
    if (deve_retroceder_cursor(maq)) {
        int len = strlen(maq->lexema);
        if (len > 0) {
            maq->lexema[len - 1] = '\0';
        }
    }
    return maq->lexema;
}