#define _DEFAULT_SOURCE
#include "item_pilha_analise.h"
#include <string.h>
#include <stdlib.h>

ItemPilha* criar_item_simbolo(const char* simbolo) {
    ItemPilha* item = (ItemPilha*)malloc(sizeof(ItemPilha));
    item->tipo = SIMBOLO;
    item->simbolo = strdup(simbolo);
    item->acao_semantica = NULL;
    item->no_ast = NULL;
    item->ancestral = NULL;
    return item;
}

ItemPilha* criar_item_acao(FuncaoAcaoSemantica acao) {
    ItemPilha* item = (ItemPilha*)malloc(sizeof(ItemPilha));
    item->tipo = ACAO;
    item->simbolo = NULL;
    item->acao_semantica = acao;
    item->no_ast = NULL;
    item->ancestral = NULL;
    return item;
}

void definir_ancestralidade(ItemPilha* item, NoAST* no, NoAST* ancestral) {
    item->no_ast = no;
    item->ancestral = ancestral;
}

void liberar_item_pilha(ItemPilha* item) {
    if (item) {
        if (item->simbolo) free(item->simbolo);
        free(item);
    }
}