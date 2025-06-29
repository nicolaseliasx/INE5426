#define _DEFAULT_SOURCE // Para strdup
#include "maquina_estados.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// CORREÇÃO: A assinatura agora bate com o .h (usa const)
MaquinaEstados* criar_maquina_estados(TransicaoEstado* transicoes, int num_trans,
                                      const char* estado_inicial, 
                                      const char** estados_finais,
                                      const char** estados_retrocesso) {
    
    MaquinaEstados* maq = (MaquinaEstados*)malloc(sizeof(MaquinaEstados));
    
    maq->transicoes = (TransicaoEstado*)malloc(sizeof(TransicaoEstado) * num_trans);
    for (int i = 0; i < num_trans; i++) {
        maq->transicoes[i].estado = transicoes[i].estado; // Não precisa de strdup, já são const
        maq->transicoes[i].funcao_transicao = transicoes[i].funcao_transicao;
    }
    maq->num_transicoes = num_trans;
    
    maq->estado_inicial = estado_inicial;
    
    int num_finais = 0;
    while (estados_finais && estados_finais[num_finais]) num_finais++;
    maq->estados_finais = estados_finais;
    maq->num_estados_finais = num_finais;

    int num_retrocesso = 0;
    while (estados_retrocesso && estados_retrocesso[num_retrocesso]) num_retrocesso++;
    maq->estados_retrocesso = estados_retrocesso;
    maq->num_estados_retrocesso = num_retrocesso;

    maq->lexema = NULL;
    maq->estado_atual = NULL;
    
    reiniciar_maquina(maq);
    return maq;
}

void reiniciar_maquina(MaquinaEstados* maq) {
    free((void*)maq->estado_atual);
    maq->estado_atual = strdup(maq->estado_inicial);
    
    free(maq->lexema);
    maq->lexema = strdup("");
    
    maq->status = MAQ_OCIOSA;
}

EstadoMaquina transicionar_maquina(MaquinaEstados* maq, char c, int is_eof) {
    // CORREÇÃO 1: Para a máquina se já estiver em um estado final (SUCESSO ou ERRO)
    if (maq->status == MAQ_ERRO || maq->status == MAQ_SUCESSO) {
        return maq->status;
    }
    
    FuncaoTransicao funcao_transicao = NULL;
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
    
    const char* proximo_estado = funcao_transicao(c, is_eof);
    free((void*)maq->estado_atual);
    maq->estado_atual = strdup(proximo_estado);
    maq->status = MAQ_EXECUTANDO;
    
    if (strcmp(maq->estado_atual, "morto") == 0) {
        maq->status = MAQ_ERRO;
        return maq->status;
    }

    // Verifica se o novo estado é final
    for (int i = 0; i < maq->num_estados_finais; i++) {
        if (strcmp(maq->estado_atual, maq->estados_finais[i]) == 0) {
            maq->status = MAQ_SUCESSO;
            break; // Sai do loop assim que encontra um estado final
        }
    }

    // Adiciona ao lexema se não precisar retroceder
    if (!deve_retroceder_cursor(maq)) {
        size_t len = strlen(maq->lexema);
        char* novo_lexema = (char*)realloc(maq->lexema, len + 2);
        novo_lexema[len] = c;
        novo_lexema[len + 1] = '\0';
        maq->lexema = novo_lexema;
    }
    
    return maq->status;
}

int deve_retroceder_cursor(MaquinaEstados* maq) {
    if (maq->status != MAQ_SUCESSO) return 0;
    
    for (int i = 0; i < maq->num_estados_retrocesso; i++) {
        if (strcmp(maq->estado_atual, maq->estados_retrocesso[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

void liberar_maquina_estados(MaquinaEstados* maq) {
    if (!maq) return;
    
    free(maq->transicoes);
    
    // CORREÇÃO: Não libere 'estado_inicial', 'estados_finais' ou 'estados_retrocesso',
    // pois eles são ponteiros para dados externos.
    // free((void*)maq->estado_inicial); // REMOVA ESTA LINHA

    // Libere apenas o que foi alocado dinamicamente pela máquina
    free((void*)maq->estado_atual);
    free(maq->lexema);
    
    free(maq);
}
EstadoMaquina obter_status_maquina(MaquinaEstados* maq) {
    return maq->status;
}

const char* obter_lexema_maquina(MaquinaEstados* maq) {
    return maq->lexema;
}