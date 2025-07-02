#ifndef ERROS_H
#define ERROS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Tipos de erro
typedef enum
{
    ERRO_LEXICO,
    ERRO_SINTATICO,
    ERRO_SEMANTICO
} TipoErro;

typedef struct
{
    TipoErro tipo;
    char mensagem[512];
} Erro;

Erro criar_erro_lexico(const char* mensagem);
Erro criar_erro_sintatico(const char* mensagem);
Erro criar_erro_semantico(const char* mensagem);
void tratar_erro(const Erro* erro);
void imprimir_erro(const Erro* erro);

#define LANCAR_ERRO_LEXICO(msg)              \
    do                                       \
    {                                        \
        Erro _erro = criar_erro_lexico(msg); \
        imprimir_erro(&_erro);               \
        exit(EXIT_FAILURE);                  \
    } while (0)

#define LANCAR_ERRO_SINTATICO(msg)              \
    do                                          \
    {                                           \
        Erro _erro = criar_erro_sintatico(msg); \
        imprimir_erro(&_erro);                  \
        exit(EXIT_FAILURE);                     \
    } while (0)

#define LANCAR_ERRO_SEMANTICO(msg)              \
    do                                          \
    {                                           \
        Erro _erro = criar_erro_semantico(msg); \
        imprimir_erro(&_erro);                  \
        exit(EXIT_FAILURE);                     \
    } while (0)

#endif