#ifndef ANALISADOR_SINTATICO_H
#define ANALISADOR_SINTATICO_H

#include "no_ast.h"             // Antigo ast_node.h
#include "gerenciador_escopo.h"  // Antigo scope_manager.h
#include "item_pilha_parser.h"   // Antigo parser_stack_item.h
#include "tabela_analise.h"      // Antigo parse_table.h
#include "token.h"               // Antigo tokens.h

typedef struct {
    NoAST* raiz_ast;
    GerenciadorEscopo* gerenciador_escopo;
    Pilha* pilha;
    const EntradaTabelaAnalise* tabela_analise;
    size_t num_entradas_tabela;
} AnalisadorSintatico;

AnalisadorSintatico* criar_analisador_sintatico(
    GerenciadorEscopo* gerenciador_escopo,
    const EntradaTabelaAnalise* tabela_analise,
    size_t num_entradas);

void inicializar_pilha(AnalisadorSintatico* analisador);
void analisar_token(AnalisadorSintatico* analisador, Token* token);
int analise_completa(AnalisadorSintatico* analisador);
void liberar_analisador_sintatico(AnalisadorSintatico* analisador);

#endif