// Arquivo: item_pilha_analise.h (VERSÃO "OURO" - USE ESTA E NENHUMA OUTRA)
#ifndef ITEM_PILHA_ANALISE_H
#define ITEM_PILHA_ANALISE_H

#include "no_ast.h"
#include "gerenciador_escopo.h"

typedef enum {
    SIMBOLO,
    ACAO
} TipoItemPilha;

typedef void (*FuncaoAcaoSemantica)(NoAST* no_pai, GerenciadorEscopo* gerenciador_escopo);

typedef struct {
    TipoItemPilha tipo;
    union {
        char* simbolo;
        FuncaoAcaoSemantica acao;
    };
    NoAST* no_ast;
    NoAST* ancestral;
} ItemPilha;

ItemPilha* criar_item_simbolo(const char* simbolo);
ItemPilha* criar_item_acao(FuncaoAcaoSemantica acao);
void definir_ancestralidade(ItemPilha* item, NoAST* no, NoAST* ancestral);
void liberar_item_pilha(ItemPilha* item);

#endif