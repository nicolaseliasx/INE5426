#ifndef NO_AST_H
#define NO_AST_H

#include "token.h"
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

// Estrutura para lista de strings (para código intermediário, etc.)
typedef struct ListaString {
    char** itens;
    int tamanho;
    int capacidade;
} ListaString;

// Estrutura para SDT de declaração
typedef struct SDTDec {
    char* tipo_inicial;
    char* tipo;
} SDTDec;

// Estrutura para SDT de resultado de código
typedef struct Codigo {
    char* var;
    char* inicio;
    char* ret;
    char* valor_retorno;
    int contador_param;
} Codigo;

// Estrutura para SDT de matemática
typedef struct SDTMatematica {
    int contador_vetor;
    ListaString* vetor_array_var;
    char operacao;
    NoExpressao* no;
} SDTMatematica;

// --- ESTRUTURA PRINCIPAL DA AST (COM NOMES CORRIGIDOS) ---
typedef struct NoAST {
    char* identificador;
    Token* token;
    
    // Filhos
    struct NoAST** filhos;
    int quantidade_filhos;
    int capacidade_filhos;
    
    // Atributos para Geração de Código
    ListaString* codigo;
    char* proximo;
    char* para_proximo; // MUDANÇA: de 'proximo_para_for' para 'para_proximo'

    // SDTs (Dados Sintetizados/Herdados) com nomes padronizados
    SDTDec sdt_dec;         // MUDANÇA: de 'decSDT' para 'sdt_dec'
    Codigo res_var_codigo;  // MUDANÇA: de 'codigo_res_var' para 'res_var_codigo'
    SDTMatematica sdt_mat;    // MUDANÇA: de 'mathSDT' para 'sdt_mat'
    
    // Ponteiro para o contador de rótulos global
    int* contador_rotulos;
} NoAST;


// --- Protótipos das Funções ---

// Funções para manipulação de nós da AST
NoAST* criar_no_ast(const char* identificador, int* contador_rotulos);
void adicionar_filho(NoAST* self, NoAST* filho);
void definir_filhos(NoAST* self, NoAST** novos_filhos, int quantidade);
void definir_token(NoAST* self, Token* token);
char* no_ast_gerar_novo_rotulo(NoAST* self);
void liberar_no_ast(NoAST* no);

// Funções para nós de expressão
NoExpressao* criar_no_expressao_unario(char operacao, const char* valor, NoExpressao* filhoA);
NoExpressao* criar_no_expressao_binario(char operacao, const char* valor, NoExpressao* filhoA, NoExpressao* filhoB);
NoExpressao* criar_no_expressao_simples(char operacao, const char* tipo, const char* valor);
NoExpressao* criar_no_expressao_basico(char operacao, const char* valor);
void liberar_no_expressao(NoExpressao* no);
void imprimir_erro_tipo(char operacao, NoExpressao* filhoA, NoExpressao* filhoB);

// Funções auxiliares para lista de strings
ListaString* criar_lista_string();
void adicionar_string(ListaString* lista, const char* str);
void liberar_lista_string(ListaString* lista);
void lista_codigo_adicionar_lista(ListaString* destino, ListaString* fonte);
void lista_codigo_imprimir_tudo(ListaString* lista);


#endif // NO_AST_H