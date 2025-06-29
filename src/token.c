#define _DEFAULT_SOURCE
#include "token.h"
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

// Cria um novo token
Token* criar_token(const char* lexema, const char* id, int linha, int coluna) {
    Token* token = (Token*)malloc(sizeof(Token));
    token->lexema = strdup(lexema);
    token->id = strdup(id);
    token->linha = linha;
    token->coluna = coluna;
    return token;
}

// Libera a memória de um token
void liberar_token(Token* token) {
    if (token) {
        free(token->lexema);
        free(token->id);
        free(token);
    }
}

// Verifica se um caractere é uma letra
bool eh_letra(char c) {
    return isalpha((unsigned char)c) || c == '_';
}

// Verifica se um caractere é um dígito
bool eh_digito(char c) {
    return isdigit((unsigned char)c);
}

// Verifica se um caractere é espaço em branco
bool eh_espaco_branco(char c) {
    return isspace((unsigned char)c);
}