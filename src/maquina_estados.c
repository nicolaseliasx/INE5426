#include "maquina_estados.h"
#include <stdlib.h>
#include <string.h>

MaquinaEstados* criar_maquina_estados(EntradaTransicao* transicoes, int num_trans,
                                      const char* estado_inicial, 
                                      char** estados_finais,
                                      char** estados_retrocesso) {
    MaquinaEstados* maq = (MaquinaEstados*)malloc(sizeof(MaquinaEstados));
    
    maq->transicoes = (EntradaTransicao*)malloc(sizeof(EntradaTransicao) * num_trans);
    for (int i = 0; i < num_trans; i++) {
        maq->transicoes[i].estado = strdup(transicoes[i].estado);
        maq->transicoes[i].funcao_transicao = transicoes[i].funcao_transicao;
    }
    maq->num_transicoes = num_trans;
    
    maq->estado_inicial = strdup(estado_inicial);
    
    // Contar estados finais
    int num_finais = 0;
    while (estados_finais[num_finais]) num_finais++;
    maq->estados_finais = (char**)malloc(sizeof(char*) * (num_finais + 1));
    for (int i = 0; i <= num_finais; i++) {
        maq->estados_finais[i] = estados_finais[i] ? strdup(estados_finais[i]) : NULL;
    }
    
    // Contar estados de retrocesso
    int num_retrocesso = 0;
    while (estados_retrocesso && estados_retrocesso[num_retrocesso]) num_retrocesso++;
    maq->estados_retrocesso = (char**)malloc(sizeof(char*) * (num_retrocesso + 1));
    for (int i = 0; i <= num_retrocesso; i++) {
        maq->estados_retrocesso[i] = estados_retrocesso && estados_retrocesso[i] ? 
            strdup(estados_retrocesso[i]) : NULL;
    }
    
    reiniciar_maquina(maq);
    return maq;
}

void reiniciar_maquina(MaquinaEstados* maq) {
    maq->estado_atual = strdup(maq->estado_inicial);
    
    if (maq->lexema) {
        free(maq->lexema);
    }
    maq->lexema = strdup("");
    
    maq->status = MAQ_OCIOSA;
}

EstadoMaquina transicionar_maquina(MaquinaEstados* maq, char c, int is_eof) {
    if (maq->status == MAQ_ERRO || maq->status == MAQ_SUCESSO) {
        return maq->status;
    }
    
    // Encontrar função de transição para o estado atual
    char* (*funcao_transicao)(char, int) = NULL;
    for (int i = 0; i < maq->num_transicoes; i++) {
        if (strcmp(maq->transicoes[i].estado, maq->estado_atual) == 0) {
            funcao_transicao = maq->transicoes[i].funcao_transicao;
            break;
        }
    }
    
    if (!funcao_transicao) {
        maq->status = MAQ_ERRO;
        return maq->status;
    }
    
    // Executar transição
    char* proximo_estado = funcao_transicao(c, is_eof);
    free(maq->estado_atual);
    maq->estado_atual = strdup(proximo_estado);
    maq->status = MAQ_EXECUTANDO;
    
    // Verificar estado terminal
    if (strcmp(proximo_estado, "dead") == 0) {
        maq->status = MAQ_ERRO;
        return maq->status;
    }
    
    // Verificar estado final
    for (int i = 0; maq->estados_finais[i]; i++) {
        if (strcmp(proximo_estado, maq->estados_finais[i]) == 0) {
            maq->status = MAQ_SUCESSO;
            break;
        }
    }
    
    // Atualizar lexema se não precisar retroceder
    if (!deve_retroceder_cursor(maq)) {
        char novo_char[2] = {c, '\0'};
        char* novo_lexema = (char*)malloc(strlen(maq->lexema) + 2);
        strcpy(novo_lexema, maq->lexema);
        strcat(novo_lexema, novo_char);
        free(maq->lexema);
        maq->lexema = novo_lexema;
    }
    
    return maq->status;
}

int deve_retroceder_cursor(MaquinaEstados* maq) {
    if (maq->status != MAQ_SUCESSO) return 0;
    
    // Verificar se é estado final
    int eh_estado_final = 0;
    for (int i = 0; maq->estados_finais[i]; i++) {
        if (strcmp(maq->estado_atual, maq->estados_finais[i]) == 0) {
            eh_estado_final = 1;
            break;
        }
    }
    if (!eh_estado_final) return 0;
    
    // Verificar se é estado de retrocesso
    for (int i = 0; maq->estados_retrocesso && maq->estados_retrocesso[i]; i++) {
        if (strcmp(maq->estado_atual, maq->estados_retrocesso[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

void liberar_maquina_estados(MaquinaEstados* maq) {
    if (!maq) return;
    
    // Liberar transições
    for (int i = 0; i < maq->num_transicoes; i++) {
        free(maq->transicoes[i].estado);
    }
    free(maq->transicoes);
    
    // Liberar estados
    free(maq->estado_inicial);
    free(maq->estado_atual);
    free(maq->lexema);
    
    // Liberar estados finais
    for (int i = 0; maq->estados_finais[i]; i++) {
        free(maq->estados_finais[i]);
    }
    free(maq->estados_finais);
    
    // Liberar estados de retrocesso
    if (maq->estados_retrocesso) {
        for (int i = 0; maq->estados_retrocesso[i]; i++) {
            free(maq->estados_retrocesso[i]);
        }
        free(maq->estados_retrocesso);
    }
    
    free(maq);
}

EstadoMaquina obter_status_maquina(MaquinaEstados* maq) {
    return maq->status;
}

const char* obter_lexema_maquina(MaquinaEstados* maq) {
    return maq->lexema;
}