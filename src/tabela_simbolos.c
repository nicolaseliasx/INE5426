#define _DEFAULT_SOURCE
#include "tabela_simbolos.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define CAPACIDADE_INICIAL 16

TabelaSimbolos* criar_tabela_simbolos() {
    TabelaSimbolos* tabela = malloc(sizeof(TabelaSimbolos));
    tabela->capacidade = CAPACIDADE_INICIAL;
    tabela->tamanho = 0;
    tabela->entradas = malloc(CAPACIDADE_INICIAL * sizeof(EntradaTabela*));
    tabela->chaves = malloc(CAPACIDADE_INICIAL * sizeof(char*));
    return tabela;
}

static int buscar_indice(TabelaSimbolos* tabela, const char* lexema) {
    for (int i = 0; i < tabela->tamanho; i++) {
        if (strcmp(tabela->chaves[i], lexema) == 0) {
            return i;
        }
    }
    return -1;
}

static void redimensionar(TabelaSimbolos* tabela) {
    int nova_capacidade = tabela->capacidade * 2;
    EntradaTabela** novas_entradas = malloc(nova_capacidade * sizeof(EntradaTabela*));
    char** novas_chaves = malloc(nova_capacidade * sizeof(char*));
    
    for (int i = 0; i < tabela->tamanho; i++) {
        novas_entradas[i] = tabela->entradas[i];
        novas_chaves[i] = tabela->chaves[i];
    }
    
    free(tabela->entradas);
    free(tabela->chaves);
    
    tabela->entradas = novas_entradas;
    tabela->chaves = novas_chaves;
    tabela->capacidade = nova_capacidade;
}

void tabela_simbolos_adicionar(TabelaSimbolos* tabela, Token* token) {
    int indice = buscar_indice(tabela, token->lexema);
    
    // Símbolo já existe, apenas adiciona a linha
    if (indice != -1) {
        EntradaTabela* entrada = tabela->entradas[indice];
        entrada->num_linhas++;
        entrada->linhas = realloc(entrada->linhas, entrada->num_linhas * sizeof(int));
        entrada->linhas[entrada->num_linhas - 1] = token->linha;
        return;
    }
    
    // Tabela cheia, redimensionar
    if (tabela->tamanho >= tabela->capacidade) {
        redimensionar(tabela);
    }
    
    // Criar nova entrada
    EntradaTabela* nova_entrada = malloc(sizeof(EntradaTabela));
    nova_entrada->tipo = strdup("");
    nova_entrada->num_linhas = 1;
    nova_entrada->linhas = malloc(sizeof(int));
    nova_entrada->linhas[0] = token->linha;
    
    // Adicionar à tabela
    tabela->chaves[tabela->tamanho] = strdup(token->lexema);
    tabela->entradas[tabela->tamanho] = nova_entrada;
    tabela->tamanho++;
}

void tabela_definir_tipo_simbolo(TabelaSimbolos* tabela, Token* token, const char* tipo) {
    int indice = buscar_indice(tabela, token->lexema);
    if (indice == -1) return;
    
    if (tabela->entradas[indice]->tipo) {
        free(tabela->entradas[indice]->tipo);
    }
    tabela->entradas[indice]->tipo = strdup(tipo);
}

bool simbolo_existe(TabelaSimbolos* tabela, const char* lexema) {
    return buscar_indice(tabela, lexema) != -1;
}

bool tabela_simbolo_e_tipo(TabelaSimbolos* tabela, Token* token, const char* tipo) {
    int indice = buscar_indice(tabela, token->lexema);
    if (indice == -1) return false;
    
    return strcmp(tabela->entradas[indice]->tipo, tipo) == 0;
}

char* tabela_obter_tipo_simbolo(TabelaSimbolos* tabela, Token* token) {
    int indice = buscar_indice(tabela, token->lexema);
    if (indice == -1) {
        return NULL;
    }
    return strdup(tabela->entradas[indice]->tipo);
}
void imprimir_tabela_simbolos(TabelaSimbolos* tabela, FILE* saida) {
    for (int i = 0; i < tabela->tamanho; i++) {
        fprintf(saida, "\tSímbolo: %s\n", tabela->chaves[i]);
        fprintf(saida, "\t\tTipo: %s\n", tabela->entradas[i]->tipo);
        
        fprintf(saida, "\t\tLinhas: [");
        for (int j = 0; j < tabela->entradas[i]->num_linhas; j++) {
            fprintf(saida, "%d", tabela->entradas[i]->linhas[j]);
            if (j < tabela->entradas[i]->num_linhas - 1) {
                fprintf(saida, ", ");
            }
        }
        fprintf(saida, "]\n");
    }
}

void liberar_entrada(EntradaTabela* entrada) {
    if (entrada) {
        free(entrada->tipo);
        free(entrada->linhas);
        free(entrada);
    }
}

void liberar_tabela_simbolos(TabelaSimbolos* tabela) {
    if (!tabela) return;
    
    for (int i = 0; i < tabela->tamanho; i++) {
        free(tabela->chaves[i]);
        liberar_entrada(tabela->entradas[i]);
    }
    
    free(tabela->entradas);
    free(tabela->chaves);
    free(tabela);
}