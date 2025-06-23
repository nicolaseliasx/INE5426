#ifndef TOKEN_H
#define TOKEN_H

#include <stdbool.h>

// Estrutura para representar um token
typedef struct {
    char* lexema;
    char* id;
    int linha;
    int coluna;
} Token;

// Protótipos das funções
Token* criar_token(const char* lexema, const char* id, int linha, int coluna);
void liberar_token(Token* token);
bool eh_letra(char c);
bool eh_digito(char c);
bool eh_espaco_branco(char c);

#endif // TOKEN_H