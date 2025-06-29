#ifndef ANALISADOR_LEXICO_H
#define ANALISADOR_LEXICO_H

#define _DEFAULT_SOURCE

#include <string.h>
#include "token.h"
#include "tabela_simbolos.h"
#include "identificadores_tokens.h"
#include "maquina_estados.h"
#include "erros.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Estrutura para token possível
typedef struct {
    Token* token;
    int inicio;
} PossivelToken;

// Estrutura principal do analisador léxico
typedef struct {
    FILE* arquivo;
    char** palavras_chave;
    int num_palavras_chave;
    
    IdentificadorToken** identificadores_token;
    int num_identificadores;
    
    PossivelToken** possiveis_tokens;
    int num_possiveis_tokens;
    
    Token** tokens;
    int num_tokens;
    
    // Mapeamento de lexema para ID
    struct {
        char* lexema;
        char* id;
    } *mapeamento_tokens;
    int num_mapeamentos;
    
    char caractere_atual;
    int linha;
    int coluna;
    int linhas_durante_token;
    int lendo_token;
} AnalisadorLexico;

// Funções públicas
AnalisadorLexico* criar_analisador_lexico(FILE* arquivo, char** palavras_chave, int num_palavras);
Token* proximo_token(AnalisadorLexico* analisador);
void destruir_analisador_lexico(AnalisadorLexico* analisador);

#endif