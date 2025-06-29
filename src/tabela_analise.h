// Arquivo: tabela_analise.h (USA AS DEFINIÇÕES DO OUTRO HEADER)

#ifndef TABELA_ANALISE_H
#define TABELA_ANALISE_H

#include <stddef.h>
#include <string.h>
#include "item_pilha_analise.h"

// Estrutura para as REGRAS DE PRODUÇÃO
typedef struct {
    const char* chave;
    ItemPilha producao[15]; 
    size_t tamanho;
} EntradaTabelaSDT;

// Estrutura para o MAPA DA TABELA DE ANÁLISE
typedef struct {
    const char* terminal;
    const char* chave_sdt;
} MapeamentoTerminal;

// Estrutura principal da TABELA DE ANÁLISE
typedef struct {
    const char* nao_terminal;
    MapeamentoTerminal* mapeamentos;
    size_t num_mapeamentos;
} EntradaTabelaAnalise;

// Declarações Globais
extern const EntradaTabelaAnalise tabela_analise[];
extern const size_t num_entradas_tabela;
extern const EntradaTabelaSDT tabela_sdt_entries[];
extern const size_t num_entradas_sdt;

// Funções
const EntradaTabelaSDT* buscar_entrada_sdt(const char* chave);

#endif