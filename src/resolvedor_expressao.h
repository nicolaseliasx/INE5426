#ifndef RESOLVEDOR_EXPRESSAO_H
#define RESOLVEDOR_EXPRESSAO_H

#include <stdio.h>

#include "no_ast.h"

// Estrutura para resolver árvores de expressão
typedef struct
{
    int contador_temp;
} ResolvedorExpressao;

// Funções públicas
void imprimir_arvore_prefixo(const char* prefixo, NoExpressao* no, int eh_esquerda);
void imprimir_arvore(NoExpressao* no);
char* gerar_variavel_temporaria(ResolvedorExpressao* resolvedor);

// Função para criar um resolvedor
ResolvedorExpressao* criar_resolvedor_expressao();

// Função para liberar memória
void liberar_resolvedor_expressao(ResolvedorExpressao* resolvedor);

#endif