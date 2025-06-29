#define _DEFAULT_SOURCE
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

#include "analisador_sintatico.h"
#include "erros.h"

const EntradaTabelaSDT* buscar_producao_sdt(
    const EntradaTabelaAnalise* tabela, 
    size_t num_entradas,
    const char* nao_terminal, 
    const char* terminal) {
    
    printf("[DEBUG-BUSCA] Buscando na tabela por T['%s', '%s']\n", nao_terminal, terminal);
    if (!tabela || !nao_terminal || !terminal) return NULL;

    for (size_t i = 0; i < num_entradas; i++) {
        if (tabela[i].nao_terminal && strcmp(tabela[i].nao_terminal, nao_terminal) == 0) {
            for (size_t j = 0; j < tabela[i].num_mapeamentos; j++) {
                if (tabela[i].mapeamentos[j].terminal && strcmp(tabela[i].mapeamentos[j].terminal, terminal) == 0) {
                    printf("[DEBUG-BUSCA] -> SUCESSO! Produção encontrada.\n");
                    return buscar_entrada_sdt(tabela[i].mapeamentos[j].chave_sdt);
                }
            }
        }
    }
    
    printf("[DEBUG-BUSCA] -> FALHA. Nenhuma produção encontrada para T['%s', '%s'].\n", nao_terminal, terminal);
    return NULL;
}


AnalisadorSintatico* criar_analisador_sintatico(
    GerenciadorEscopo* gerenciador_escopo,
    const EntradaTabelaAnalise* tabela_analise,
    size_t num_entradas) {
    
    AnalisadorSintatico* analisador = malloc(sizeof(AnalisadorSintatico));
    analisador->gerenciador_escopo = gerenciador_escopo;
    analisador->tabela_analise = tabela_analise;
    analisador->num_entradas_tabela = num_entradas;
    analisador->contador_rotulos = 0; // Inicializa o contador
    analisador->raiz_ast = criar_no_ast("PROGRAM", &analisador->contador_rotulos);
    analisador->pilha = criar_pilha(100);
    inicializar_pilha(analisador);
    
    return analisador;
}


void inicializar_pilha(AnalisadorSintatico* analisador) {
    ItemPilha* fim = criar_item_simbolo("$");
    empilhar(analisador->pilha, fim);
    
    ItemPilha* inicial = criar_item_simbolo("PROGRAM");
    definir_ancestralidade(inicial, analisador->raiz_ast, NULL);
    empilhar(analisador->pilha, inicial);
}

void executar_acao(ItemPilha* item_acao, AnalisadorSintatico* analisador) {
    if (!item_acao || item_acao->tipo != ACAO || !item_acao->acao) exit(1);
    if (!item_acao->ancestral) {
        printf("[DEBUG-CRASH] ERRO FATAL: Ação semântica com ancestral NULO!\n");
        exit(1);
    }
    item_acao->acao(item_acao->ancestral, analisador->gerenciador_escopo);
}

void analisar_token(AnalisadorSintatico* analisador, Token* token) {
    printf("\n--- Nova chamada de analisar_token com Token: ID='%s', Lexema='%s' ---\n", token->id, token->lexema);

    while (1) {
        printf("--- Início do loop de análise ---\n");

        if (pilha_vazia(analisador->pilha)) {
            char erro_msg[100];
            snprintf(erro_msg, sizeof(erro_msg), "Token inesperado '%s' em %d:%d", token->lexema, token->linha, token->coluna);
            criar_erro_sintatico(erro_msg);
            return;
        }
        
        ItemPilha* topo = topo_pilha(analisador->pilha);

        printf("[DEBUG] Topo da pilha: Tipo=%d", topo->tipo);
        if (topo->tipo == SIMBOLO) {
            printf(", Símbolo='%s'\n", topo->simbolo);
        } else {
            printf(", Ação em %p\n", (void*)topo->acao);
        }

        // 1. Tratar ação semântica
        if (topo->tipo == ACAO) {
            printf("[DEBUG] Encontrada AÇÃO SEMÂNTICA. Executando e desempilhando...\n");
            executar_acao(topo, analisador); // Usando a função auxiliar segura
            liberar_item_pilha(desempilhar(analisador->pilha));
            printf("[DEBUG] Ação executada. Continuando loop...\n");
            continue;
        }
        
        // 2. Caso terminal coincida
        if (strcmp(topo->simbolo, token->id) == 0) {
            printf("[DEBUG] Terminal '%s' coincide com token ID '%s'.\n", topo->simbolo, token->id);
            if (strcmp(token->lexema, "$") != 0) {
                definir_token(topo->no_ast, token);
                printf("[DEBUG] Token associado ao nó AST '%s'.\n", topo->no_ast->identificador);
            }
            liberar_item_pilha(desempilhar(analisador->pilha));
            printf("[DEBUG] Terminal desempilhado. Saindo de analisar_token.\n");
            return;
        }
        
        // 3. Buscar produção na tabela
        printf("[DEBUG] Buscando produção para T['%s', '%s']...\n", topo->simbolo, token->id);
        const EntradaTabelaSDT* producao = buscar_producao_sdt(
            analisador->tabela_analise,
            analisador->num_entradas_tabela,
            topo->simbolo,
            token->id
        );
        
        if (!producao) {
            char erro_msg[100];
            snprintf(erro_msg, sizeof(erro_msg), "Token inválido '%s' para o estado '%s' em %d:%d", 
                     token->lexema, topo->simbolo, token->linha, token->coluna);
            criar_erro_sintatico(erro_msg);
            return;
        }
        
        printf("[DEBUG] Produção encontrada. Expandindo '%s'.\n", topo->simbolo);

        NoAST* no_pai_da_regra = topo->no_ast;
        char topo_simbolo_salvo[100];
        strncpy(topo_simbolo_salvo, topo->simbolo, 99);
        topo_simbolo_salvo[99] = '\0';

        if (!no_pai_da_regra) exit(1);
        
        liberar_item_pilha(desempilhar(analisador->pilha));
        printf("[DEBUG] Item '%s' desempilhado.\n", topo_simbolo_salvo);
        
        ItemPilha** itens_para_empilhar = malloc(producao->tamanho * sizeof(ItemPilha*));
        if (!itens_para_empilhar) {
             criar_erro_sintatico("Falha de alocacao de memoria no analisador.");
             return;
        }
        int num_itens_validos = 0;
        
        for (size_t i = 0; i < producao->tamanho; i++) {
            const ItemPilha* item_da_regra = &producao->producao[i];
            
            if (item_da_regra->tipo == SIMBOLO && strcmp(item_da_regra->simbolo, "ε") == 0) {
                continue;
            }
            
            ItemPilha* novo_item_para_pilha;
            
            if (item_da_regra->tipo == ACAO) {
                novo_item_para_pilha = criar_item_acao(item_da_regra->acao);
                definir_ancestralidade(novo_item_para_pilha, NULL, no_pai_da_regra);
            } else { // É um SÍMBOLO
                novo_item_para_pilha = criar_item_simbolo(item_da_regra->simbolo);
                NoAST* novo_no_ast = criar_no_ast(item_da_regra->simbolo, &analisador->contador_rotulos);
                adicionar_filho(no_pai_da_regra, novo_no_ast);
                definir_ancestralidade(novo_item_para_pilha, novo_no_ast, no_pai_da_regra);
            }
            itens_para_empilhar[num_itens_validos++] = novo_item_para_pilha;
        }
        
        for (int i = num_itens_validos - 1; i >= 0; i--) {
            empilhar(analisador->pilha, itens_para_empilhar[i]);
        }
        
        free(itens_para_empilhar);
    }
}


int analise_completa(AnalisadorSintatico* analisador) {
    return pilha_vazia(analisador->pilha);
}


void liberar_analisador_sintatico(AnalisadorSintatico* analisador) {
    if (analisador) {
        liberar_pilha(analisador->pilha);
        liberar_no_ast(analisador->raiz_ast);
        free(analisador);
    }
}