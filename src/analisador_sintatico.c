#define _DEFAULT_SOURCE
#include "analisador_sintatico.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debug.h"
#include "erros.h"

// Função de comparação para bsearch. Compara primeiro por não-terminal, depois por terminal.
static int comparar_entradas_tabela(const void* a, const void* b)
{
    const EntradaTabelaAnalise* entradaA = (const EntradaTabelaAnalise*)a;
    const EntradaTabelaAnalise* entradaB = (const EntradaTabelaAnalise*)b;

    // 1. Compara o não-terminal (chave primária)
    int cmp_nao_terminal = strcmp(entradaA->nao_terminal, entradaB->nao_terminal);
    if (cmp_nao_terminal != 0)
    {
        return cmp_nao_terminal;
    }

    // 2. Se não-terminais são iguais, compara o terminal (chave secundária)
    return strcmp(entradaA->terminal, entradaB->terminal);
}

// Nova função de busca otimizada que encapsula a chamada bsearch.
const EntradaTabelaSDT* buscar_producao_na_tabela(const char* nao_terminal, const char* terminal)
{
    // Cria uma "chave" de busca com os dados que queremos encontrar.
    EntradaTabelaAnalise chave_busca = {nao_terminal, terminal, NULL};

    // Executa a busca binária
    EntradaTabelaAnalise* resultado =
        (EntradaTabelaAnalise*)bsearch(&chave_busca, tabela_analise, num_entradas_tabela,
                                       sizeof(EntradaTabelaAnalise), comparar_entradas_tabela);

    if (resultado)
    {
        // Encontrou! Usa a chave_sdt para buscar a produção real na outra tabela.
        return buscar_entrada_sdt(resultado->chave_sdt);
    }
    else
    {
        DEBUG_PRINT("[DEBUG] Produção não encontrada para '%s' e '%s'\n", nao_terminal, terminal);
    }

    // Se bsearch retornou NULL, a regra não existe.
    return NULL;
}

AnalisadorSintatico* criar_analisador_sintatico(GerenciadorEscopo* gerenciador_escopo,
                                                const EntradaTabelaAnalise* tabela_analise,
                                                size_t num_entradas)
{
    AnalisadorSintatico* analisador = malloc(sizeof(AnalisadorSintatico));
    analisador->gerenciador_escopo = gerenciador_escopo;
    analisador->tabela_analise = tabela_analise;
    analisador->num_entradas_tabela = num_entradas;
    analisador->contador_rotulos = 0;  // Inicializa o contador
    analisador->raiz_ast = criar_no_ast("PROGRAM", &analisador->contador_rotulos);
    analisador->pilha = criar_pilha(100);
    inicializar_pilha(analisador);

    return analisador;
}

void inicializar_pilha(AnalisadorSintatico* analisador)
{
    ItemPilha* fim = criar_item_simbolo("$");
    empilhar(analisador->pilha, fim);

    ItemPilha* inicial = criar_item_simbolo("PROGRAM");
    definir_ancestralidade(inicial, analisador->raiz_ast, NULL);
    empilhar(analisador->pilha, inicial);
}

void executar_acao(ItemPilha* item_acao, AnalisadorSintatico* analisador)
{
    DEBUG_PRINT("[DEBUG-ACAO] Executando %p no nó %s\n", item_acao->acao,
                item_acao->ancestral ? item_acao->ancestral->identificador : "NULO");
    if (!item_acao || item_acao->tipo != ACAO || !item_acao->acao)
    {
        return;
    }

    if (!item_acao->ancestral)
    {
        DEBUG_PRINT("[DEBUG] AVISO: Ação semântica '%p' sem ancestral, criando nó temporário\n",
                    (void*)item_acao->acao);

        NoAST* temp_node = criar_no_ast("TEMP_ACTION", &analisador->contador_rotulos);
        item_acao->ancestral = temp_node;
    }

    item_acao->acao(item_acao->ancestral, analisador->gerenciador_escopo);
}

void analisar_token(AnalisadorSintatico* analisador, Token* token)
{
    DEBUG_PRINT(
        "\n[DEBUG] --- Nova chamada de analisar_token com Token: ID='%s', Lexema='%s' ---\n",
        token->id, token->lexema);

    while (1)
    {
        DEBUG_PRINT("[DEBUG] --- Início do loop de análise ---\n");

        if (pilha_vazia(analisador->pilha))
        {
            char erro_msg[100];
            snprintf(erro_msg, sizeof(erro_msg), "Token inesperado '%s' em %d:%d", token->lexema,
                     token->linha, token->coluna);
            LANCAR_ERRO_SINTATICO(erro_msg);
            return;
        }

        ItemPilha* topo = topo_pilha(analisador->pilha);

        DEBUG_PRINT("[DEBUG] Topo da pilha: Tipo=%d", topo->tipo);

        if (topo->tipo == SIMBOLO)
        {
            DEBUG_PRINT("[DEBUG] Símbolo='%s'\n", topo->simbolo);
        }
        else
        {
            DEBUG_PRINT("[DEBUG] Ação em %p\n", (void*)topo->acao);
        }

        // 1. Tratar ação semântica
        if (topo->tipo == ACAO)
        {
            DEBUG_PRINT("[DEBUG] Encontrada AÇÃO SEMÂNTICA. Executando e desempilhando...\n");
            executar_acao(topo, analisador);  // Usando a função auxiliar segura
            liberar_item_pilha(desempilhar(analisador->pilha));
            DEBUG_PRINT("[DEBUG] Ação executada. Continuando loop...\n");
            continue;
        }

        // 2. Caso terminal coincida
        if (strcmp(topo->simbolo, token->id) == 0)
        {
            DEBUG_PRINT("[DEBUG] Terminal '%s' coincide com token ID '%s'.\n", topo->simbolo,
                        token->id);
            if (strcmp(token->lexema, "$") != 0)
            {
                definir_token(topo->no_ast, token);
                DEBUG_PRINT("[DEBUG] Token associado ao nó AST '%s'.\n",
                            topo->no_ast->identificador);
            }
            liberar_item_pilha(desempilhar(analisador->pilha));
            DEBUG_PRINT("[DEBUG] Terminal desempilhado. Saindo de analisar_token.\n");
            return;
        }

        // 3. Buscar produção na tabela
        DEBUG_PRINT("[DEBUG] Buscando produção para T['%s', '%s']...\n", topo->simbolo, token->id);
        const EntradaTabelaSDT* producao = buscar_producao_na_tabela(topo->simbolo, token->id);

        if (!producao)
        {
            char erro_msg[100];
            snprintf(erro_msg, sizeof(erro_msg), "Produção não encontrada (linha %d)",
                     token->linha);
            LANCAR_ERRO_SINTATICO(erro_msg);
            return;
        }

        DEBUG_PRINT("[DEBUG] Produção encontrada. Expandindo '%s'.\n", topo->simbolo);

        NoAST* no_pai_da_regra = topo->no_ast;
        char topo_simbolo_salvo[100];
        strncpy(topo_simbolo_salvo, topo->simbolo, 99);
        topo_simbolo_salvo[99] = '\0';

        if (!no_pai_da_regra) exit(1);

        liberar_item_pilha(desempilhar(analisador->pilha));
        DEBUG_PRINT("[DEBUG] Item '%s' desempilhado.\n", topo_simbolo_salvo);

        ItemPilha** itens_para_empilhar = malloc(producao->tamanho * sizeof(ItemPilha*));
        if (!itens_para_empilhar)
        {
            LANCAR_ERRO_SINTATICO("Falha de alocacao de memoria no analisador.");
            return;
        }
        int num_itens_validos = 0;

        for (size_t i = 0; i < producao->tamanho; i++)
        {
            const ItemPilha* item_da_regra = &producao->producao[i];

            if (item_da_regra->tipo == SIMBOLO && strcmp(item_da_regra->simbolo, "ε") == 0)
            {
                continue;
            }

            ItemPilha* novo_item_para_pilha;

            if (item_da_regra->tipo == ACAO)
            {
                novo_item_para_pilha = criar_item_acao(item_da_regra->acao);
                definir_ancestralidade(novo_item_para_pilha, NULL, no_pai_da_regra);
                DEBUG_PRINT("[DEBUG] Inicializado item pilha para PROGRAM\n");
            }
            else
            {  // É um SÍMBOLO
                novo_item_para_pilha = criar_item_simbolo(item_da_regra->simbolo);
                NoAST* novo_no_ast =
                    criar_no_ast(item_da_regra->simbolo, &analisador->contador_rotulos);
                adicionar_filho(no_pai_da_regra, novo_no_ast);
                definir_ancestralidade(novo_item_para_pilha, novo_no_ast, no_pai_da_regra);
            }
            itens_para_empilhar[num_itens_validos++] = novo_item_para_pilha;
        }

        for (int i = num_itens_validos - 1; i >= 0; i--)
        {
            empilhar(analisador->pilha, itens_para_empilhar[i]);
        }

        free(itens_para_empilhar);
    }
}

int analise_completa(AnalisadorSintatico* analisador) { return pilha_vazia(analisador->pilha); }

void liberar_analisador_sintatico(AnalisadorSintatico* analisador)
{
    if (analisador)
    {
        liberar_pilha(analisador->pilha);
        liberar_no_ast(analisador->raiz_ast);
        free(analisador);
    }
}