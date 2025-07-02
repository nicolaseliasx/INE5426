#ifndef ANALISADOR_SINTATICO_H
#define ANALISADOR_SINTATICO_H

#include <stddef.h>

#include "gerenciador_escopo.h"
#include "item_pilha_analise.h"
#include "no_ast.h"
#include "pilha.h"
#include "tabela_analise.h"
#include "token.h"

typedef struct
{
    NoAST* raiz_ast;
    GerenciadorEscopo* gerenciador_escopo;
    Pilha* pilha;
    const EntradaTabelaAnalise* tabela_analise;
    size_t num_entradas_tabela;
    int contador_rotulos;

} AnalisadorSintatico;

// Protótipos das funções
AnalisadorSintatico* criar_analisador_sintatico(GerenciadorEscopo* gerenciador_escopo,
                                                const EntradaTabelaAnalise* tabela_analise,
                                                size_t num_entradas);

void inicializar_pilha(AnalisadorSintatico* analisador);
void analisar_token(AnalisadorSintatico* analisador, Token* token);
int analise_completa(AnalisadorSintatico* analisador);
void liberar_analisador_sintatico(AnalisadorSintatico* analisador);

#endif  // ANALISADOR_SINTATICO_H