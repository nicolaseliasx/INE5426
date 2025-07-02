#ifndef TABELA_SIMBOLOS_H
#define TABELA_SIMBOLOS_H

#include "token.h"
#include <stdbool.h>
#include <stdio.h>

// A "entrada" da tabela, contendo os dados do símbolo.
// Esta estrutura permanece a mesma.
typedef struct {
    char* tipo;
    int* linhas;
    int num_linhas;
    int capacidade_linhas; // Adicionado para otimizar realocações
} EntradaTabela;

// O "item" da tabela hash. Contém a chave (nome do símbolo),
// o valor (dados do símbolo) e um ponteiro para o próximo item
// em caso de colisão de hash (lista ligada).
typedef struct ItemTabela {
    char* chave;
    EntradaTabela* entrada;
    struct ItemTabela* proximo;
} ItemTabela;

// A estrutura principal da Tabela de Símbolos, agora como Tabela Hash.
typedef struct {
    ItemTabela** buckets; // O array de "buckets". Cada bucket é um ponteiro para o início de uma lista ligada.
    int capacidade;     // O número de buckets no array.
    int tamanho;          // O número total de símbolos na tabela.
} TabelaSimbolos;

// Cria uma nova tabela de símbolos
TabelaSimbolos* criar_tabela_simbolos();
// Adiciona um novo símbolo à tabela
void tabela_simbolos_adicionar(TabelaSimbolos* tabela, Token* token);
// Define o tipo de um símbolo
void tabela_definir_tipo_simbolo(TabelaSimbolos* tabela, Token* token, const char* tipo);
// Verifica se um lexema está na tabela
bool simbolo_existe(TabelaSimbolos* tabela, const char* lexema);
// Verifica se um símbolo é de um tipo específico
bool tabela_simbolo_e_tipo(TabelaSimbolos* tabela, Token* token, const char* tipo);
// Obtém o tipo de um símbolo
char* tabela_obter_tipo_simbolo(TabelaSimbolos* tabela, Token* token);
// Imprime o conteúdo da tabela
void imprimir_tabela_simbolos(TabelaSimbolos* tabela, FILE* saida);
// Libera a memória da tabela
void liberar_tabela_simbolos(TabelaSimbolos* tabela);

#endif // TABELA_SIMBOLOS_H