#ifndef TABELA_SDT_AUXILIAR_H
#define TABELA_SDT_AUXILIAR_H

#include "item_pilha_analise.h"

typedef struct {
    TipoItemPilha tipo;
    union {
        void (*acao)(NoAST*, GerenciadorEscopo*);
        char* simbolo;
    } valor;
} ItemTabelaSDT;

typedef struct {
    char* chave;
    ItemTabelaSDT* itens;
    int quantidade;
} EntradaTabelaAuxSDT;

extern EntradaTabelaAuxSDT tabela_sdt_entries[];
extern const size_t num_entradas_sdt;

void inicializar_tabela_sdt_entries();

#endif // TABELA_SDT_AUXILIAR_H