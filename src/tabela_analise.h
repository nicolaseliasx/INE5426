// Arquivo: tabela_analise.h (USA AS DEFINIÇÕES DO OUTRO HEADER)

#ifndef TABELA_ANALISE_H
#define TABELA_ANALISE_H

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "item_pilha_analise.h"

// Estrutura para as REGRAS DE PRODUÇÃO
typedef struct
{
    const char* chave;
    ItemPilha producao[15];
    size_t tamanho;
} EntradaTabelaSDT;

// Estrutura para o MAPA DA TABELA DE ANÁLISE
typedef struct
{
    const char* terminal;
    const char* chave_sdt;
} MapeamentoTerminal;

// Estrutura principal da TABELA DE ANÁLISE
typedef struct
{
    const char* nao_terminal;
    const char* terminal;
    const char* chave_sdt;
} EntradaTabelaAnalise;

// Declarações Globais
extern const EntradaTabelaAnalise tabela_analise[];
extern const size_t num_entradas_tabela;
extern const EntradaTabelaSDT tabela_sdt_entradas[];
extern const size_t num_entradas_sdt;

// Função para buscar na tabela de entradas SDT
const EntradaTabelaSDT* buscar_entrada_sdt(const char* chave);

#endif