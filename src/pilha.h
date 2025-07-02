#ifndef PILHA_H
#define PILHA_H

#include <stdlib.h>

#include "item_pilha_analise.h"  // A pilha precisa saber o que ela vai guardar

// Definição da estrutura da Pilha
typedef struct
{
    ItemPilha** itens;  // Um array de ponteiros para ItemPilha
    int topo;           // Índice do elemento no topo (-1 para pilha vazia)
    int capacidade;     // Tamanho atual do array 'itens'
} Pilha;

/**
 * @brief Cria e inicializa uma nova pilha.
 * @param capacidade_inicial O tamanho inicial do array interno.
 * @return Um ponteiro para a nova Pilha.
 */
Pilha* criar_pilha(int capacidade_inicial);

/**
 * @brief Libera toda a memória usada pela pilha.
 * @param p A pilha a ser liberada.
 */
void liberar_pilha(Pilha* p);

/**
 * @brief Adiciona um item ao topo da pilha (push).
 * @param p A pilha.
 * @param item O ponteiro para ItemPilha a ser adicionado.
 */
void empilhar(Pilha* p, ItemPilha* item);

/**
 * @brief Remove e retorna o item do topo da pilha (pop).
 * @param p A pilha.
 * @return O ponteiro para o ItemPilha que estava no topo.
 */
ItemPilha* desempilhar(Pilha* p);

/**
 * @brief Retorna o item do topo da pilha sem removê-lo (top/peek).
 * @param p A pilha.
 * @return O ponteiro para o ItemPilha no topo.
 */
ItemPilha* topo_pilha(Pilha* p);

/**
 * @brief Verifica se a pilha está vazia.
 * @param p A pilha.
 * @return 1 se estiver vazia, 0 caso contrário.
 */
int pilha_vazia(Pilha* p);

#endif  // PILHA_H
