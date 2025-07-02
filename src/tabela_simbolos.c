#define _DEFAULT_SOURCE
#include "tabela_simbolos.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Capacidade inicial da tabela hash.
#define HASH_TABLE_CAPACIDADE_INICIAL 53

// FUNÇÃO DE HASH
// Esta função converte uma string em um número inteiro (o hash).
// Usamos o algoritmo "djb2", que é muito popular, simples e eficaz.
static unsigned long hash_djb2(const char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    return hash;
}

static ItemTabela* criar_item_tabela(const char* chave, Token* token) {
    ItemTabela* item = (ItemTabela*)malloc(sizeof(ItemTabela));
    item->chave = strdup(chave);
    item->proximo = NULL;

    EntradaTabela* entrada = (EntradaTabela*)malloc(sizeof(EntradaTabela));
    entrada->tipo = strdup(""); // Tipo inicial vazio
    entrada->capacidade_linhas = 4;
    entrada->num_linhas = 1;
    entrada->linhas = (int*)malloc(entrada->capacidade_linhas * sizeof(int));
    entrada->linhas[0] = token->linha;

    item->entrada = entrada;
    return item;
}

void tabela_redimensionar(TabelaSimbolos* tabela) {
    // 1. Calcula a nova capacidade (dobro da antiga é um bom começo)
    int capacidade_antiga = tabela->capacidade;
    int nova_capacidade = capacidade_antiga * 2;

    // Salva um ponteiro para a lista de buckets antiga
    ItemTabela** buckets_antigos = tabela->buckets;

    // 2. Cria a nova lista de buckets, maior e zerada
    tabela->capacidade = nova_capacidade;
    tabela->tamanho = 0; // Será re-incrementado
    tabela->buckets = (ItemTabela**)calloc(nova_capacidade, sizeof(ItemTabela*));

    // 3. Re-hash: Percorre a tabela antiga e insere cada item na nova
    for (int i = 0; i < capacidade_antiga; i++) {
        ItemTabela* item_atual = buckets_antigos[i];
        while (item_atual != NULL) {
            // Guarda o próximo item antes de modificar os ponteiros
            ItemTabela* proximo_item = item_atual->proximo;

            // Recalcula o índice para a nova capacidade
            unsigned long hash = hash_djb2(item_atual->chave);
            unsigned long novo_index = hash % nova_capacidade;

            // Insere o item na frente da lista do novo bucket
            item_atual->proximo = tabela->buckets[novo_index];
            tabela->buckets[novo_index] = item_atual;
            tabela->tamanho++;

            // Passa para o próximo item na lista ligada antiga
            item_atual = proximo_item;
        }
    }

    // 4. Libera a memória da lista de buckets antiga (NÃO dos itens, pois eles foram movidos)
    free(buckets_antigos);
}

TabelaSimbolos* criar_tabela_simbolos() {
    TabelaSimbolos* tabela = (TabelaSimbolos*)malloc(sizeof(TabelaSimbolos));
    tabela->capacidade = HASH_TABLE_CAPACIDADE_INICIAL;
    tabela->tamanho = 0;
    // Usa calloc para alocar e já inicializar todos os buckets com NULL
    tabela->buckets = (ItemTabela**)calloc(tabela->capacidade, sizeof(ItemTabela*));
    return tabela;
}

void liberar_tabela_simbolos(TabelaSimbolos* tabela) {
    if (!tabela) return;

    for (int i = 0; i < tabela->capacidade; i++) {
        ItemTabela* item = tabela->buckets[i];
        // Percorre a lista ligada de cada bucket, liberando tudo
        while (item != NULL) {
            ItemTabela* proximo = item->proximo;
            free(item->chave);
            free(item->entrada->tipo);
            free(item->entrada->linhas);
            free(item->entrada);
            free(item);
            item = proximo;
        }
    }
    free(tabela->buckets);
    free(tabela);
}

// Função auxiliar para buscar um item. Retorna o ponteiro para o item ou NULL.
static ItemTabela* tabela_buscar_item(TabelaSimbolos* tabela, const char* lexema) {
    unsigned long hash = hash_djb2(lexema);
    unsigned long index = hash % tabela->capacidade;

    ItemTabela* item = tabela->buckets[index];
    while (item != NULL) {
        if (strcmp(item->chave, lexema) == 0) {
            return item; // Encontrado!
        }
        item = item->proximo;
    }
    return NULL; // Não encontrado
}


void tabela_simbolos_adicionar(TabelaSimbolos* tabela, Token* token) {
    ItemTabela* item = tabela_buscar_item(tabela, token->lexema);

    // Se o símbolo já existe, apenas atualiza a lista de linhas.
    if (item != NULL) {
        EntradaTabela* entrada = item->entrada;
        // Otimização: dobra a capacidade do array de linhas se estiver cheio
        if (entrada->num_linhas >= entrada->capacidade_linhas) {
            entrada->capacidade_linhas *= 2;
            int* novas_linhas = realloc(entrada->linhas, entrada->capacidade_linhas * sizeof(int));
            if (!novas_linhas) { // Verificação de segurança para realloc
                fprintf(stderr, "Falha ao realocar memória para as linhas do símbolo.\n");
                return;
            }
            entrada->linhas = novas_linhas;
        }
        entrada->linhas[entrada->num_linhas] = token->linha;
        entrada->num_linhas++;
    } else {
        // Se o símbolo é novo, cria um novo item e o insere na frente da lista do bucket.
        unsigned long hash = hash_djb2(token->lexema);
        unsigned long index = hash % tabela->capacidade;

        ItemTabela* novo_item = criar_item_tabela(token->lexema, token);
        novo_item->proximo = tabela->buckets[index]; // O antigo "primeiro" se torna o segundo
        tabela->buckets[index] = novo_item;       // O novo item se torna o primeiro
        tabela->tamanho++;

        // Verifica se o fator de carga (tamanho/capacidade) ultrapassou o limite de 75%.
        if ((float)tabela->tamanho / (float)tabela->capacidade > 0.75f) {
            tabela_redimensionar(tabela);
        }
    }
}

void tabela_definir_tipo_simbolo(TabelaSimbolos* tabela, Token* token, const char* tipo) {
    ItemTabela* item = tabela_buscar_item(tabela, token->lexema);
    if (item != NULL) {
        free(item->entrada->tipo); // Libera o tipo antigo
        item->entrada->tipo = strdup(tipo); // Atribui o novo
    }
}

bool simbolo_existe(TabelaSimbolos* tabela, const char* lexema) {
    return tabela_buscar_item(tabela, lexema) != NULL;
}

bool tabela_simbolo_e_tipo(TabelaSimbolos* tabela, Token* token, const char* tipo) {
    ItemTabela* item = tabela_buscar_item(tabela, token->lexema);
    if (item != NULL) {
        return strcmp(item->entrada->tipo, tipo) == 0;
    }
    return false;
}

char* tabela_obter_tipo_simbolo(TabelaSimbolos* tabela, Token* token) {
    ItemTabela* item = tabela_buscar_item(tabela, token->lexema);
    if (item != NULL) {
        return strdup(item->entrada->tipo);
    }
    return NULL;
}

void imprimir_tabela_simbolos(TabelaSimbolos* tabela, FILE* saida) {
    fprintf(saida, "-------------------------------------------------\n");
    for (int i = 0; i < tabela->capacidade; i++) {
        ItemTabela* item = tabela->buckets[i];
        if (item == NULL) {
            continue;
        }
        // Itera pela lista ligada em cada bucket que não esteja vazio
        while (item != NULL) {
            fprintf(saida, "\tSímbolo: %s\n", item->chave);
            fprintf(saida, "\t\tTipo: %s\n", item->entrada->tipo);
            fprintf(saida, "\t\tLinhas: [");
            for (int j = 0; j < item->entrada->num_linhas; j++) {
                fprintf(saida, "%d%s", item->entrada->linhas[j], (j == item->entrada->num_linhas - 1) ? "" : ", ");
            }
            fprintf(saida, "]\n");
            item = item->proximo;
        }
    }
    fprintf(saida, "-------------------------------------------------\n");
}