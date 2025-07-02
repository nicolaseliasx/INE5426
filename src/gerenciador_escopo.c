#define _DEFAULT_SOURCE
#include "gerenciador_escopo.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "erros.h"
#include "tabela_simbolos.h"

#define CAPACIDADE_INICIAL_FILHOS 10

GerenciadorEscopo* criar_gerenciador_escopo()
{
    GerenciadorEscopo* gerenciador = malloc(sizeof(GerenciadorEscopo));
    gerenciador->escopo_raiz = NULL;
    gerenciador->escopo_atual = NULL;
    return gerenciador;
}

void gerenciador_abrir_escopo(GerenciadorEscopo* gerenciador, const char* nome_escopo)
{
    // Criar novo item de escopo
    ItemEscopo* novo_escopo = malloc(sizeof(ItemEscopo));
    novo_escopo->nome = strdup(nome_escopo);
    novo_escopo->tabela_simbolos = criar_tabela_simbolos();
    novo_escopo->escopo_ancestral = gerenciador->escopo_atual;

    // Inicializar lista de filhos
    novo_escopo->escopos_filhos = malloc(CAPACIDADE_INICIAL_FILHOS * sizeof(ItemEscopo*));
    novo_escopo->num_escopos_filhos = 0;
    novo_escopo->capacidade_filhos = CAPACIDADE_INICIAL_FILHOS;

    // Configurar hierarquia
    if (!gerenciador->escopo_raiz)
    {
        gerenciador->escopo_raiz = novo_escopo;
    }
    else if (gerenciador->escopo_atual)
    {
        // Adicionar como filho do escopo atual
        if (gerenciador->escopo_atual->num_escopos_filhos >=
            gerenciador->escopo_atual->capacidade_filhos)
        {
            // Redimensionar array se necessário
            int nova_capacidade = gerenciador->escopo_atual->capacidade_filhos * 2;
            gerenciador->escopo_atual->escopos_filhos = realloc(
                gerenciador->escopo_atual->escopos_filhos, nova_capacidade * sizeof(ItemEscopo*));
            gerenciador->escopo_atual->capacidade_filhos = nova_capacidade;
        }
        gerenciador->escopo_atual->escopos_filhos[gerenciador->escopo_atual->num_escopos_filhos++] =
            novo_escopo;
    }

    gerenciador->escopo_atual = novo_escopo;
}

void gerenciador_fechar_escopo(GerenciadorEscopo* gerenciador)
{
    if (gerenciador->escopo_atual && gerenciador->escopo_atual->escopo_ancestral)
    {
        gerenciador->escopo_atual = gerenciador->escopo_atual->escopo_ancestral;
    }
}

void gerenciador_escopo_adicionar_simbolo(GerenciadorEscopo* gerenciador, Token* token)
{
    if (!gerenciador->escopo_atual) return;

    if (simbolo_existe(gerenciador->escopo_atual->tabela_simbolos, token->lexema))
    {
        char erro_msg[100];
        snprintf(erro_msg, sizeof(erro_msg), "%s redeclarado em %d:%d", token->lexema, token->linha,
                 token->coluna);
        LANCAR_ERRO_SEMANTICO(erro_msg);
        return;
    }

    tabela_simbolos_adicionar(gerenciador->escopo_atual->tabela_simbolos, token);
}

void gerenciador_definir_tipo_simbolo(GerenciadorEscopo* gerenciador, Token* token,
                                      const char* tipo)
{
    ItemEscopo* escopo = gerenciador->escopo_atual;

    while (escopo)
    {
        if (simbolo_existe(escopo->tabela_simbolos, token->lexema))
        {
            tabela_definir_tipo_simbolo(escopo->tabela_simbolos, token, tipo);
            return;
        }
        escopo = escopo->escopo_ancestral;
    }

    char erro_msg[100];
    snprintf(erro_msg, sizeof(erro_msg), "Impossível atribuir tipo a %s em %d:%d", token->lexema,
             token->linha, token->coluna);
    LANCAR_ERRO_SEMANTICO(erro_msg);
}

bool gerenciador_simbolo_declarado(GerenciadorEscopo* gerenciador, Token* token)
{
    ItemEscopo* escopo = gerenciador->escopo_atual;
    while (escopo)
    {
        if (simbolo_existe(escopo->tabela_simbolos, token->lexema))
        {
            return true;
        }
        escopo = escopo->escopo_ancestral;
    }
    return false;
}

bool gerenciador_simbolo_e_tipo(GerenciadorEscopo* gerenciador, Token* token, const char* tipo)
{
    ItemEscopo* escopo = gerenciador->escopo_atual;

    while (escopo)
    {
        if (simbolo_existe(escopo->tabela_simbolos, token->lexema))
        {
            return tabela_simbolo_e_tipo(escopo->tabela_simbolos, token, tipo);
        }
        escopo = escopo->escopo_ancestral;
    }
    return false;
}

char* gerenciador_obter_tipo_simbolo(GerenciadorEscopo* gerenciador, Token* token)
{
    ItemEscopo* escopo = gerenciador->escopo_atual;

    while (escopo)
    {
        if (simbolo_existe(escopo->tabela_simbolos, token->lexema))
        {
            return tabela_obter_tipo_simbolo(escopo->tabela_simbolos, token);
        }
        escopo = escopo->escopo_ancestral;
    }
    return "";
}

bool gerenciador_esta_no_escopo(GerenciadorEscopo* gerenciador, const char* tipo_escopo)
{
    ItemEscopo* escopo = gerenciador->escopo_atual;

    while (escopo)
    {
        if (strcmp(escopo->nome, tipo_escopo) == 0)
        {
            return true;
        }
        escopo = escopo->escopo_ancestral;
    }
    return false;
}

void imprimir_tabelas_gerenciador(GerenciadorEscopo* gerenciador, FILE* saida)
{
    if (!gerenciador->escopo_raiz) return;

    // Pilha para percurso em profundidade
    ItemEscopo** pilha = malloc(100 * sizeof(ItemEscopo*));
    int topo = -1;
    pilha[++topo] = gerenciador->escopo_raiz;

    while (topo >= 0)
    {
        ItemEscopo* atual = pilha[topo--];

        fprintf(saida, "Tabela de Símbolos");
        if (strlen(atual->nome) > 0)
        {
            fprintf(saida, " tipo: %s", atual->nome);
        }
        fprintf(saida, "\n");
        imprimir_tabela_simbolos(atual->tabela_simbolos, saida);

        // Empilhar filhos
        for (int i = atual->num_escopos_filhos - 1; i >= 0; i--)
        {
            if (topo < 99)
            {
                pilha[++topo] = atual->escopos_filhos[i];
            }
        }
    }

    free(pilha);
}

void liberar_gerenciador_escopo(GerenciadorEscopo* gerenciador)
{
    if (!gerenciador) return;

    // Liberar escopos recursivamente
    ItemEscopo** pilha = malloc(100 * sizeof(ItemEscopo*));
    int topo = -1;
    pilha[++topo] = gerenciador->escopo_raiz;

    while (topo >= 0)
    {
        ItemEscopo* atual = pilha[topo--];

        // Empilhar filhos
        for (int i = 0; i < atual->num_escopos_filhos; i++)
        {
            if (topo < 99)
            {
                pilha[++topo] = atual->escopos_filhos[i];
            }
        }

        // Liberar memória do escopo atual
        free(atual->nome);
        liberar_tabela_simbolos(atual->tabela_simbolos);
        free(atual->escopos_filhos);
        free(atual);
    }

    free(pilha);
    free(gerenciador);
}

void gerenciador_registrar_uso_simbolo(GerenciadorEscopo* gerenciador, Token* token)
{
    ItemEscopo* escopo = gerenciador->escopo_atual;
    while (escopo)
    {
        if (simbolo_existe(escopo->tabela_simbolos, token->lexema))
        {
            tabela_simbolos_adicionar(escopo->tabela_simbolos, token);
            return;
        }
        escopo = escopo->escopo_ancestral;
    }
}
