// Arquivo: item_pilha_analise.h (VERSÃO FINAL E DEFINITIVA)
#ifndef ITEM_PILHA_ANALISE_H
#define ITEM_PILHA_ANALISE_H

#include "no_ast.h"
#include "gerenciador_escopo.h"

// Enum para o tipo de item
typedef enum {
    SIMBOLO,
    ACAO
} TipoItemPilha;

// Ponteiro de função para a ação semântica
typedef void (*FuncaoAcaoSemantica)(NoAST* no_pai, GerenciadorEscopo* gerenciador_escopo);

// A ESTRUTURA DEFINITIVA COM UNION ANÔNIMA
typedef struct {
    TipoItemPilha tipo;
    union { // Union anônima (sem nome)
        char* simbolo;
        FuncaoAcaoSemantica acao;
    };

    NoAST* no_ast;
    NoAST* ancestral;
} ItemPilha;

// As declarações das funções permanecem as mesmas
ItemPilha* criar_item_simbolo(const char* simbolo);
ItemPilha* criar_item_acao(FuncaoAcaoSemantica acao);
void definir_ancestralidade(ItemPilha* item, NoAST* no, NoAST* ancestral);
void liberar_item_pilha(ItemPilha* item);

#endif // ITEM_PILHA_ANALISE_H