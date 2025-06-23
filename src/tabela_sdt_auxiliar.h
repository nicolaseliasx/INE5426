#ifndef TABELA_SDT_AUXILIAR_H
#define TABELA_STD_AUXILIAR_H

#include "item_pilha_analise.h"
#include "gerenciador_escopo.h"
#include "acoes_semanticas.h"

typedef enum {
    ACAO,
    SIMBOLO
} TipoItemPilha;

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
} EntradaTabelaSDT;

typedef struct {
    EntradaTabelaSDT* entradas;
    int quantidade;
} TabelaSDT;

// Funções públicas
TabelaSDT* criar_tabela_sdt();
const ItemTabelaSDT* buscar_producao(const TabelaSDT* tabela, const char* nao_terminal, const char* token);
void liberar_tabela_sdt(TabelaSDT* tabela);

#endif