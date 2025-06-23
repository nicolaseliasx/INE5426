#ifndef TABELA_SIMBOLOS_H
#define TABELA_SIMBOLOS_H

#include "token.h"
#include <stdbool.h>

// Estrutura para uma entrada na tabela de símbolos
typedef struct {
    char* tipo;
    int* linhas;
    int num_linhas;
} EntradaTabela;

// Estrutura principal da tabela de símbolos
typedef struct {
    EntradaTabela** entradas;
    char** chaves;
    int capacidade;
    int tamanho;
} TabelaSimbolos;

// Cria uma nova tabela de símbolos
TabelaSimbolos* criar_tabela_simbolos();

// Adiciona um novo símbolo à tabela
void tabela_simbolos_adicionar(TabelaSimbolos* tabela, Token* token);

// Define o tipo de um símbolo
void definir_tipo_simbolo(TabelaSimbolos* tabela, Token* token, const char* tipo);

// Verifica se um lexema está na tabela
bool simbolo_existe(TabelaSimbolos* tabela, const char* lexema);

// Verifica se um símbolo é de um tipo específico
bool simbolo_e_tipo(TabelaSimbolos* tabela, Token* token, const char* tipo);

// Obtém o tipo de um símbolo
char* obter_tipo_simbolo(TabelaSimbolos* tabela, Token* token);

// Imprime o conteúdo da tabela
void imprimir_tabela(TabelaSimbolos* tabela, FILE* saida);

// Libera a memória da tabela
void liberar_tabela_simbolos(TabelaSimbolos* tabela);

#endif