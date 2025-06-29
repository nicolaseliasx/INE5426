#ifndef ITEM_PILHA_ANALISE_H
#define ITEM_PILHA_ANALISE_H

#include "no_ast.h"
#include "gerenciador_escopo.h"

typedef enum {
    SIMBOLO,
    ACAO
} TipoItemPilha;

typedef void (*FuncaoAcaoSemantica)(NoAST* no_pai, GerenciadorEscopo* gerenciador_escopo);

typedef struct ItemPilha {
    TipoItemPilha tipo;
    char* simbolo;
    FuncaoAcaoSemantica acao_semantica;
    NoAST* no_ast;
    NoAST* ancestral;
} ItemPilha;

// Cria um item de pilha do tipo símbolo
ItemPilha* criar_item_simbolo(const char* simbolo);

// Cria um item de pilha do tipo ação semântica
ItemPilha* criar_item_acao(FuncaoAcaoSemantica acao);

// Define a relação de ancestralidade na AST
void definir_ancestralidade(ItemPilha* item, NoAST* no, NoAST* ancestral);

// Libera a memória de um item da pilha
void liberar_item_pilha(ItemPilha* item);

#endif