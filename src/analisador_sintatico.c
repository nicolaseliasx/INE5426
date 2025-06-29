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
    
    // Log detalhado para a busca
    printf("[DEBUG-BUSCA] Buscando na tabela por T['%s', '%s']\n", nao_terminal, terminal);

    // Verificações de segurança para impedir o crash
    if (!tabela) {
        printf("[DEBUG-BUSCA] ERRO FATAL: A 'tabela_analise' é NULA.\n");
        return NULL;
    }
    if (!nao_terminal || !terminal) {
        printf("[DEBUG-BUSCA] ERRO FATAL: Argumento para busca é NULO.\n");
        return NULL;
    }

    // Loop seguro pela tabela
    for (size_t i = 0; i < num_entradas; i++) {
        // Verifica se a entrada da tabela é válida
        if (!tabela[i].nao_terminal) {
            printf("[DEBUG-BUSCA] ALERTA: Entrada %zu da tabela tem um 'nao_terminal' NULO. Pulando.\n", i);
            continue; // Pula esta entrada para evitar crash
        }
        
        // Compara o não-terminal
        if (strcmp(tabela[i].nao_terminal, nao_terminal) == 0) {
            printf("[DEBUG-BUSCA] -> Encontrado bloco do não-terminal '%s'. Procurando pelo terminal...\n", nao_terminal);
            
            if (!tabela[i].mapeamentos) {
                printf("[DEBUG-BUSCA] ALERTA: Entrada %zu da tabela tem 'mapeamentos' NULO. Pulando.\n", i);
                continue;
            }

            for (size_t j = 0; j < tabela[i].num_mapeamentos; j++) {
                // Verifica se o terminal no mapeamento é válido
                if (!tabela[i].mapeamentos[j].terminal) {
                    printf("[DEBUG-BUSCA] ALERTA: Mapeamento %zu tem um 'terminal' NULO. Pulando.\n", j);
                    continue; // Pula este mapeamento para evitar crash
                }
                
                if (strcmp(tabela[i].mapeamentos[j].terminal, terminal) == 0) {
                    printf("[DEBUG-BUSCA] -> SUCESSO! Produção encontrada.\n");
                    return buscar_entrada_sdt(tabela[i].mapeamentos[j].chave_sdt);
                }
            }
        }
    }
    
    // Se o loop terminar, nenhuma produção foi encontrada
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
    
    // CORREÇÃO: Usa "PROGRAM" para ser idêntico ao C++
    analisador->raiz_ast = criar_no_ast("PROGRAM", &analisador->contador_rotulos);
    
    analisador->pilha = criar_pilha(100);
    inicializar_pilha(analisador);
    
    return analisador;
}

void inicializar_pilha(AnalisadorSintatico* analisador) {
    // Empilha o símbolo terminal de fim de entrada
    ItemPilha* fim = criar_item_simbolo("$");
    empilhar(analisador->pilha, fim);
    
    // Empilha o símbolo inicial
    // CORREÇÃO: Usa "PROGRAM" para ser idêntico ao C++
    ItemPilha* inicial = criar_item_simbolo("PROGRAM");
    definir_ancestralidade(inicial, analisador->raiz_ast, NULL);
    empilhar(analisador->pilha, inicial);
}

void analisar_token(AnalisadorSintatico* analisador, Token* token) {
    printf("\n--- Nova chamada de analisar_token com Token: ID='%s', Lexema='%s' ---\n", token->id, token->lexema);

    while (1) {
        printf("--- Início do loop de análise ---\n");

        if (pilha_vazia(analisador->pilha)) {
            printf("[DEBUG-CRASH] ERRO FATAL: A pilha está vazia, mas ainda há tokens. Token inesperado.\n");
            char erro_msg[100];
            snprintf(erro_msg, sizeof(erro_msg), 
                     "Token inesperado '%s' em %d:%d", token->lexema, token->linha, token->coluna);
            criar_erro_sintatico(erro_msg);
            return;
        }
        
        ItemPilha* topo = topo_pilha(analisador->pilha);
        printf("[DEBUG] Topo da pilha: Símbolo='%s', Tipo=%d\n", topo->simbolo, topo->tipo);

        // Verifica se os ponteiros no topo da pilha são válidos
        if (topo->no_ast) {
             printf("[DEBUG] topo->no_ast aponta para um nó '%s'\n", topo->no_ast->identificador);
        } else {
             printf("[DEBUG] ALERTA: topo->no_ast é NULO.\n");
        }
        if (topo->ancestral) {
             printf("[DEBUG] topo->ancestral aponta para um nó '%s'\n", topo->ancestral->identificador);
        } else {
             printf("[DEBUG] topo->ancestral é NULO.\n");
        }


        // 1. Tratar ação semântica
        if (topo->tipo == ACAO) {
            printf("[DEBUG] Encontrada AÇÃO SEMÂNTICA. Executando e desempilhando...\n");
            if (!topo->ancestral) {
                printf("[DEBUG-CRASH] ERRO FATAL: Ação semântica com ancestral NULO!\n");
                exit(1); // Força a saída para indicar o ponto exato do erro.
            }
            topo->acao(topo->ancestral, analisador->gerenciador_escopo);
            liberar_item_pilha(desempilhar(analisador->pilha));
            printf("[DEBUG] Ação executada. Continuando loop...\n");
            continue;
        }
        
        // 2. Caso terminal coincida
        if (strcmp(topo->simbolo, token->id) == 0) {
            printf("[DEBUG] Terminal '%s' coincide com token ID '%s'.\n", topo->simbolo, token->id);
            if (strcmp(token->lexema, "$") != 0) {
                if (!topo->no_ast) {
                    printf("[DEBUG-CRASH] ERRO FATAL: Tentando definir token em um nó AST NULO!\n");
                    exit(1);
                }
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
            printf("[DEBUG-CRASH] ERRO FATAL: Nenhuma produção encontrada na tabela de análise.\n");
            char erro_msg[100];
            snprintf(erro_msg, sizeof(erro_msg), 
                     "Token inválido '%s' para o estado '%s' em %d:%d", 
                     token->lexema, topo->simbolo, token->linha, token->coluna);
            criar_erro_sintatico(erro_msg);
            return;
        }
        
        printf("[DEBUG] Produção encontrada. Expandindo '%s'.\n", topo->simbolo);

        // Salva o ponteiro para o nó pai ANTES de desempilhar.
        NoAST* no_pai_da_regra = topo->no_ast;
        // Salva também o símbolo para o log
        char topo_simbolo_salvo[100];
        strncpy(topo_simbolo_salvo, topo->simbolo, 99);
        topo_simbolo_salvo[99] = '\0';

        if (!no_pai_da_regra) {
            printf("[DEBUG-CRASH] ERRO FATAL: O nó pai para a regra ('%s') é NULO!\n", topo_simbolo_salvo);
            exit(1);
        }
        printf("[DEBUG] Nó pai da regra ('%s') salvo. Endereço do nó: %p\n", no_pai_da_regra->identificador, (void*)no_pai_da_regra);
        
        liberar_item_pilha(desempilhar(analisador->pilha));
        printf("[DEBUG] Item '%s' desempilhado.\n", topo_simbolo_salvo);
        
        ItemPilha** itens_para_empilhar = malloc(producao->tamanho * sizeof(ItemPilha*));
        if (!itens_para_empilhar) {
             criar_erro_sintatico("Falha de alocacao de memoria no analisador.");
             return;
        }
        int num_itens_validos = 0;
        
        printf("[DEBUG] Processando %zu itens da produção...\n", producao->tamanho);
        for (size_t i = 0; i < producao->tamanho; i++) {
            ItemPilha* item_da_regra = &producao->producao[i];
            
            if (item_da_regra->tipo == SIMBOLO && strcmp(item_da_regra->simbolo, "ε") == 0) {
                printf("[DEBUG]   Item %zu: 'ε', ignorando.\n", i);
                continue;
            }
            
            printf("[DEBUG]   Processando Item %zu: Símbolo='%s', Tipo=%d\n", i, item_da_regra->simbolo, item_da_regra->tipo);

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
        
        printf("[DEBUG] Empilhando %d novos itens na ordem reversa...\n", num_itens_validos);
        for (int i = num_itens_validos - 1; i >= 0; i--) {
            printf("[DEBUG]   Empilhando '%s'\n", itens_para_empilhar[i]->simbolo);
            empilhar(analisador->pilha, itens_para_empilhar[i]);
        }
        
        free(itens_para_empilhar);
        printf("[DEBUG] Itens empilhados. Retornando ao topo do loop while.\n");
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