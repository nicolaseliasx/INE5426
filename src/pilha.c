#include "pilha.h"

#include <stdio.h>
#include <stdlib.h>

Pilha* criar_pilha(int capacidade_inicial)
{
    Pilha* p = (Pilha*)malloc(sizeof(Pilha));
    if (!p)
    {
        // Falha na alocação da estrutura da pilha
        return NULL;
    }
    p->capacidade = capacidade_inicial;
    p->topo = -1;  // Pilha começa vazia
    p->itens = (ItemPilha**)malloc(sizeof(ItemPilha*) * p->capacidade);
    if (!p->itens)
    {
        // Falha na alocação do array de itens
        free(p);
        return NULL;
    }
    return p;
}

void liberar_pilha(Pilha* p)
{
    if (p)
    {
        // IMPORTANTE: Esta função NÃO libera os itens que estão na pilha,
        // pois a posse deles pertence ao analisador sintático, que os
        // desempilha e libera um por um.
        free(p->itens);
        free(p);
    }
}

int pilha_vazia(Pilha* p) { return p->topo == -1; }

void empilhar(Pilha* p, ItemPilha* item)
{
    // Verifica se a pilha está cheia e precisa de mais espaço
    if (p->topo == p->capacidade - 1)
    {
        int nova_capacidade = p->capacidade * 2;  // Dobra a capacidade
        ItemPilha** novos_itens =
            (ItemPilha**)realloc(p->itens, sizeof(ItemPilha*) * nova_capacidade);
        if (!novos_itens)
        {
            fprintf(stderr, "Erro: Falha ao realocar a pilha.\n");
            exit(EXIT_FAILURE);
        }
        p->itens = novos_itens;
        p->capacidade = nova_capacidade;
    }

    // Adiciona o item no topo
    p->topo++;
    p->itens[p->topo] = item;
}

ItemPilha* desempilhar(Pilha* p)
{
    if (pilha_vazia(p))
    {
        return NULL;  // Não há nada para desempilhar
    }
    ItemPilha* item = p->itens[p->topo];
    p->topo--;
    return item;
}

ItemPilha* topo_pilha(Pilha* p)
{
    if (pilha_vazia(p))
    {
        return NULL;
    }
    return p->itens[p->topo];
}
