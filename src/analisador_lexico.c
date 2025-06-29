#define _DEFAULT_SOURCE
#include "analisador_lexico.h"
#include "identificadores_tokens.h"
#include "maquina_estados.h"
#include "erros.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static char* obter_id_token_final(AnalisadorLexico* analisador, const char* id, const char* lexema) {
    if (strcmp(id, "IDENT") == 0) {
        for (int i = 0; i < analisador->num_palavras_chave; i++) {
            if (strcmp(lexema, analisador->palavras_chave[i]) == 0) {
                char* kw_id = strdup(lexema);
                for (char* p = kw_id; *p; p++) *p = toupper(*p);
                return kw_id;
            }
        }
    }
    return strdup(id);
}

// Cria o analisador léxico
AnalisadorLexico* criar_analisador_lexico(FILE* arquivo, char** palavras_chave, int num_palavras) {
    AnalisadorLexico* analisador = malloc(sizeof(AnalisadorLexico));
    analisador->arquivo = arquivo;
    analisador->linha = 1;
    analisador->coluna = 0;
    
    // Copia palavras-chave
    analisador->num_palavras_chave = num_palavras;
    analisador->palavras_chave = malloc(num_palavras * sizeof(char*));
    for (int i = 0; i < num_palavras; i++) {
        analisador->palavras_chave[i] = strdup(palavras_chave[i]);
    }
    
    // Aponta para as máquinas de estado globais
    analisador->num_identificadores = 7;
    analisador->identificadores_token = malloc(analisador->num_identificadores * sizeof(IdentificadorToken*));
    analisador->identificadores_token[0] = &identTokenIdentifier;
    analisador->identificadores_token[1] = &integerTokenIdentifier;
    analisador->identificadores_token[2] = &floatTokenIdentifier;
    analisador->identificadores_token[3] = &mathOperatorTokenIdentifier;
    analisador->identificadores_token[4] = &relopTokenIdentifier;
    analisador->identificadores_token[5] = &oneCharTokenIdentifier;
    analisador->identificadores_token[6] = &stringConstCharTokenIdentifier;
    
    return analisador;
}

// Libera a memória do analisador
void destruir_analisador_lexico(AnalisadorLexico* analisador) {
    if (!analisador) return;

    for (int i = 0; i < analisador->num_palavras_chave; i++) {
        free(analisador->palavras_chave[i]);
    }
    free(analisador->palavras_chave);

    // As máquinas e identificadores são globais, não os liberamos aqui.
    // Apenas o array de ponteiros.
    free(analisador->identificadores_token);
    free(analisador);
}


// A função principal que lê o arquivo e retorna o próximo token
Token* proximo_token(AnalisadorLexico* analisador) {
    if (!analisador || !analisador->arquivo || feof(analisador->arquivo)) {
        return NULL;
    }

    // 1. Pular espaços em branco
    int c = ' ';
    while (eh_espaco_branco(c) && !feof(analisador->arquivo)) {
        c = fgetc(analisador->arquivo);
        if (c == '\n') {
            analisador->linha++;
            analisador->coluna = 0;
        } else {
            analisador->coluna++;
        }
    }
    if (feof(analisador->arquivo)) return NULL;
    ungetc(c, analisador->arquivo);
    analisador->coluna--;

    // 2. Marcar posição inicial e reiniciar as máquinas
    int linha_inicio = analisador->linha;
    int coluna_inicio = analisador->coluna + 1;
    long posicao_inicio = ftell(analisador->arquivo);

    for (int i = 0; i < analisador->num_identificadores; i++) {
        reiniciar_maquina(analisador->identificadores_token[i]->maquina_estado);
    }

    // 3. Encontrar o maior casamento (longest match)
    char* lexema_maior_token = NULL;
    IdentificadorToken* id_maior_token = NULL;
    long posicao_final = posicao_inicio;

    while (1) {
        long pos_leitura_atual = ftell(analisador->arquivo);
        c = fgetc(analisador->arquivo);
        bool eof = (c == EOF);

        int maquinas_ativas = 0;
        for (int i = 0; i < analisador->num_identificadores; i++) {
            IdentificadorToken* id_token = analisador->identificadores_token[i];
            
            if (obter_status_maquina(id_token->maquina_estado) == MAQ_ERRO) continue;

            transicionar_maquina(id_token->maquina_estado, c, eof);
            
            if (obter_status_maquina(id_token->maquina_estado) != MAQ_ERRO) {
                maquinas_ativas++;
                if (obter_status_maquina(id_token->maquina_estado) == MAQ_SUCESSO) {
                    free(lexema_maior_token);
                    lexema_maior_token = strdup(obter_lexema_maquina(id_token->maquina_estado));
                    id_maior_token = id_token;
                    posicao_final = pos_leitura_atual;
                }
            }
        }
        if (maquinas_ativas == 0 || eof) break;
    }

    // 4. Criar o token ou reportar erro
    if (id_maior_token) {
        fseek(analisador->arquivo, posicao_final, SEEK_SET);
        if(!deve_retroceder_cursor(id_maior_token->maquina_estado)) {
             fgetc(analisador->arquivo);
        }
        analisador->coluna = coluna_inicio + strlen(lexema_maior_token) - 1;
        
        char* id_final = obter_id_token_final(analisador, id_maior_token->id_token, lexema_maior_token);
        Token* token_final = criar_token(lexema_maior_token, id_final, linha_inicio, coluna_inicio);
        
        free(lexema_maior_token);
        free(id_final);
        return token_final;
    } else {
        c = fgetc(analisador->arquivo);
        char erro_msg[100];
        snprintf(erro_msg, sizeof(erro_msg), "Caractere inesperado '%c' na linha %d:%d", c, linha_inicio, coluna_inicio);
        criar_erro_lexico(erro_msg);
        return NULL;
    }
}