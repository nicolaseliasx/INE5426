#ifndef NO_AST_H
#define NO_AST_H

#include "tokens.h"
#include "tipo_enum.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Estrutura para nó de expressão
typedef struct NoExpressao {
    char operacao;
    char* tipo;
    char* valor;
    struct NoExpressao* filhoA;
    struct NoExpressao* filhoB;
} NoExpressao;

// Estrutura para lista de strings
typedef struct ListaString {
    char** itens;
    int tamanho;
    int capacidade;
} ListaString;

// Estrutura para SDT matemática
typedef struct SDTMatematica {
    int contador_arrays;
    ListaString* variaveis_array;
    char operacao;
    NoExpressao* no;
} SDTMatematica;

// Estrutura para SDT de declaração
typedef struct SDTDec {
    char* tipo_inicial;
    char* tipo;
} SDTDec;

// Estrutura para código
typedef struct Codigo {
    char* variavel;
    char* inicio;
    char* retorno;
    char* valor_retorno;
    int contador_parametros;
} Codigo;

// Estrutura principal da AST
typedef struct NoAST {
    char* identificador;
    Token* token;
    
    // Filhos
    struct NoAST** filhos;
    int quantidade_filhos;
    int capacidade_filhos;
    
    // Código gerado
    ListaString* codigo;
    char* proximo;
    char* proximo_para_for;

    // SDTs
    SDTDec decSDT;
    Codigo codigo_res_var;
    SDTMatematica mathSDT;
    
    // Contador de rótulos (simulado como ponteiro para contador global)
    int* contador_rotulos;
} NoAST;

// Funções para manipulação de nós
NoAST* criar_no_ast(const char* identificador, int* contador_rotulos);
void adicionar_filho(NoAST* self, NoAST* filho);
void definir_filhos(NoAST* self, NoAST** novos_filhos, int quantidade);
void definir_token(NoAST* self, Token* token);
char* novo_rotulo(NoAST* self);
void liberar_no_ast(NoAST* no);

// Funções para nós de expressão
NoExpressao* criar_no_expressao_unario(char operacao, const char* valor, NoExpressao* filhoA);
NoExpressao* criar_no_expressao_binario(char operacao, const char* valor, NoExpressao* filhoA, NoExpressao* filhoB);
NoExpressao* criar_no_expressao_simples(char operacao, const char* tipo, const char* valor);
NoExpressao* criar_no_expressao_basico(char operacao, const char* valor);
void liberar_no_expressao(NoExpressao* no);

// Funções auxiliares
ListaString* criar_lista_string();
void adicionar_string(ListaString* lista, const char* str);
void liberar_lista_string(ListaString* lista);
void imprimir_erro_tipo(char operacao, NoExpressao* filhoA, NoExpressao* filhoB);

#endif