#include "analisador_sintatico.h"
#include "pilha.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Função auxiliar para buscar produção na tabela
const EntradaTabelaSDT* buscar_producao(
    const EntradaTabelaAnalise* tabela, 
    size_t num_entradas,
    const char* nao_terminal, 
    const char* terminal) {
    
    for (size_t i = 0; i < num_entradas; i++) {
        if (strcmp(tabela[i].nao_terminal, nao_terminal) == 0) {
            for (size_t j = 0; j < tabela[i].num_mapeamentos; j++) {
                if (strcmp(tabela[i].mapeamentos[j].terminal, terminal) == 0) {
                    return buscar_entrada_sdt(tabela[i].mapeamentos[j].chave_sdt);
                }
            }
        }
    }
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
    analisador->raiz_ast = criar_no_ast("PROGRAMA");
    
    // Inicializa a pilha com capacidade inicial
    analisador->pilha = criar_pilha(100);
    inicializar_pilha(analisador);
    
    return analisador;
}

void inicializar_pilha(AnalisadorSintatico* analisador) {
    // Empilha o símbolo terminal de fim de entrada
    ItemPilha* fim = criar_item_simbolo("$");
    empilhar(analisador->pilha, fim);
    
    // Empilha o símbolo inicial
    ItemPilha* inicial = criar_item_simbolo("PROGRAMA");
    definir_ancestralidade(inicial, analisador->raiz_ast, NULL);
    empilhar(analisador->pilha, inicial);
}

void analisar_token(AnalisadorSintatico* analisador, Token* token) {
    while (1) {
        if (pilha_vazia(analisador->pilha)) {
            char erro_msg[100];
            snprintf(erro_msg, sizeof(erro_msg), 
                     "Token inesperado em %d:%d", token->linha, token->coluna);
            erro_sintatico(erro_msg); // Função de tratamento de erro
            return;
        }
        
        ItemPilha* topo = topo_pilha(analisador->pilha);
        
        // Tratar ação semântica
        if (topo->tipo == TIPO_ACAO) {
            topo->acao_semantica(topo->ancestral, analisador->gerenciador_escopo);
            liberar_item_pilha(desempilhar(analisador->pilha));
            continue;
        }
        
        // Caso terminal coincida
        if (strcmp(topo->simbolo, token->id) == 0) {
            if (strcmp(token->lexema, "$") != 0) {
                associar_token_no(topo->no_ast, token);
            }
            liberar_item_pilha(desempilhar(analisador->pilha));
            return;
        }
        
        // Buscar produção na tabela
        const EntradaTabelaSDT* producao = buscar_producao(
            analisador->tabela_analise,
            analisador->num_entradas_tabela,
            topo->simbolo,
            token->id
        );
        
        if (!producao) {
            char erro_msg[100];
            snprintf(erro_msg, sizeof(erro_msg), 
                     "Token inválido '%s' em %d:%d", 
                     token->lexema, token->linha, token->coluna);
            erro_sintatico(erro_msg);
            return;
        }
        
        // Remove o item do topo
        liberar_item_pilha(desempilhar(analisador->pilha));
        
        // Prepara itens para empilhar
        ItemPilha** itens_empilhar = malloc(producao->tamanho * sizeof(ItemPilha*));
        int num_itens = 0;
        
        for (size_t i = 0; i < producao->tamanho; i++) {
            ItemPilha* item = &producao->producao[i];
            
            // Ignorar produções vazias
            if (item->tipo == TIPO_SIMBOLO && strcmp(item->simbolo, "ε") == 0) {
                continue;
            }
            
            ItemPilha* novo_item;
            if (item->tipo == TIPO_ACAO) {
                novo_item = criar_item_acao(item->acao_semantica);
                definir_ancestralidade(novo_item, NULL, topo->no_ast);
            } else {
                novo_item = criar_item_simbolo(item->simbolo);
                
                // Criar nó AST apenas para não-terminais
                if (isupper(item->simbolo[0])) { // Heurística para identificar não-terminais
                    NoAST* novo_no = criar_no_ast(item->simbolo);
                    definir_ancestralidade(novo_item, novo_no, topo->no_ast);
                    adicionar_filho(topo->no_ast, novo_no);
                }
            }
            
            itens_empilhar[num_itens++] = novo_item;
        }
        
        // Empilha na ordem reversa
        for (int i = num_itens - 1; i >= 0; i--) {
            empilhar(analisador->pilha, itens_empilhar[i]);
        }
        
        free(itens_empilhar);
    }
}

int analise_completa(AnalisadorSintatico* analisador) {
    return pilha_vazia(analisador->pilha);
}

void liberar_analisador_sintatico(AnalisadorSintatico* analisador) {
    if (analisador) {
        liberar_pilha(analisador->pilha);
        liberar_no_ast(analisador->raiz_ast); // Implementar liberação recursiva da AST
        free(analisador);
    }
}