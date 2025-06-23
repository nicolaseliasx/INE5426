#ifndef TABELA_ANALISE_H
#define TABELA_ANALISE_H

#include <string.h>
#include "item_pilha_parser.h"

// Estrutura para uma entrada na tabela SDT
typedef struct {
    const char* chave;
    ItemPilha* producao;
    size_t tamanho;
} EntradaTabelaSDT;

// Estrutura para um mapeamento terminal -> produção
typedef struct {
    const char* terminal;
    const char* chave_sdt;
} MapeamentoTerminal;

// Estrutura principal da tabela de análise
typedef struct {
    const char* nao_terminal;
    MapeamentoTerminal* mapeamentos;
    size_t num_mapeamentos;
} EntradaTabelaAnalise;

extern const EntradaTabelaAnalise tabela_analise[];
extern const size_t num_entradas_tabela;

// Protótipos das funções auxiliares
const EntradaTabelaSDT* buscar_entrada_sdt(const char* chave);

#endif