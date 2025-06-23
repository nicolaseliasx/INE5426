#ifndef ERROS_H
#define ERROS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Tipos de erro
typedef enum {
    ERRO_LEXICO,
    ERRO_SINTATICO,
    ERRO_SEMANTICO
} TipoErro;

// Estrutura base para erros
typedef struct {
    TipoErro tipo;
    char mensagem[512];
} Erro;

// Função para criar erro léxico
Erro criar_erro_lexico(const char* mensagem) {
    Erro erro;
    erro.tipo = ERRO_LEXICO;
    snprintf(erro.mensagem, sizeof(erro.mensagem), "Erro Léxico: %s", mensagem);
    return erro;
}

// Função para criar erro sintático
Erro criar_erro_sintatico(const char* mensagem) {
    Erro erro;
    erro.tipo = ERRO_SINTATICO;
    snprintf(erro.mensagem, sizeof(erro.mensagem), "Erro Sintático: %s", mensagem);
    return erro;
}

// Função para criar erro semântico
Erro criar_erro_semantico(const char* mensagem) {
    Erro erro;
    erro.tipo = ERRO_SEMANTICO;
    snprintf(erro.mensagem, sizeof(erro.mensagem), "Erro Semântico: %s", mensagem);
    return erro;
}

// Macros para lançar erros
#define LANCAR_ERRO_LEXICO(msg) do { \
    Erro _erro = criar_erro_lexico(msg); \
    fprintf(stderr, "%s\n", _erro.mensagem); \
    exit(EXIT_FAILURE); \
} while(0)

#define LANCAR_ERRO_SINTATICO(msg) do { \
    Erro _erro = criar_erro_sintatico(msg); \
    fprintf(stderr, "%s\n", _erro.mensagem); \
    exit(EXIT_FAILURE); \
} while(0)

#define LANCAR_ERRO_SEMANTICO(msg) do { \
    Erro _erro = criar_erro_semantico(msg); \
    fprintf(stderr, "%s\n", _erro.mensagem); \
    exit(EXIT_FAILURE); \
} while(0)

// Funções para tratamento de erros
void tratar_erro(const Erro* erro) {
    fprintf(stderr, "%s\n", erro->mensagem);
    exit(EXIT_FAILURE);
}

void imprimir_erro(const Erro* erro) {
    const char* tipo_str = "";
    switch(erro->tipo) {
        case ERRO_LEXICO: tipo_str = "Léxico"; break;
        case ERRO_SINTATICO: tipo_str = "Sintático"; break;
        case ERRO_SEMANTICO: tipo_str = "Semântico"; break;
    }
    fprintf(stderr, "[ERRO %s] %s\n", tipo_str, erro->mensagem);
}

#endif