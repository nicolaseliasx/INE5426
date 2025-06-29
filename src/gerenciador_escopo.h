#ifndef GERENCIADOR_ESCOPO_H
#define GERENCIADOR_ESCOPO_H

#include "tabela_simbolos.h"
#include <stdbool.h>

// Estrutura de um item de escopo
typedef struct ItemEscopo {
    char* nome;
    TabelaSimbolos* tabela_simbolos;
    struct ItemEscopo* escopo_ancestral;
    struct ItemEscopo** escopos_filhos;
    int num_escopos_filhos;
    int capacidade_filhos;
} ItemEscopo;

// Gerenciador principal de escopos
typedef struct {
    ItemEscopo* escopo_raiz;
    ItemEscopo* escopo_atual;
} GerenciadorEscopo;

// Cria um novo gerenciador de escopos
GerenciadorEscopo* criar_gerenciador_escopo();

// Abre um novo escopo
void gerenciador_abrir_escopo(GerenciadorEscopo* gerenciador, const char* nome_escopo);

// Fecha o escopo atual
void gerenciador_fechar_escopo(GerenciadorEscopo* gerenciador);

// Adiciona um novo símbolo ao escopo atual
void gerenciador_escopo_adicionar_simbolo(GerenciadorEscopo* gerenciador, Token* token);

// Define o tipo de um símbolo
void gerenciador_definir_tipo_simbolo(GerenciadorEscopo* gerenciador, Token* token, const char* tipo);

// Verifica se um símbolo está declarado
bool gerenciador_simbolo_declarado(GerenciadorEscopo* gerenciador, Token* token);

// Verifica se um símbolo é de um tipo específico
bool gerenciador_simbolo_e_tipo(GerenciadorEscopo* gerenciador, Token* token, const char* tipo);

// Obtém o tipo de um símbolo
char* gerenciador_obter_tipo_simbolo(GerenciadorEscopo* gerenciador, Token* token);

// Verifica se está em um tipo específico de escopo
bool gerenciador_esta_no_escopo(GerenciadorEscopo* gerenciador, const char* tipo_escopo);

// Imprime todas as tabelas de símbolos
void imprimir_tabelas_gerenciador(GerenciadorEscopo* gerenciador, FILE* saida);

// Libera a memória do gerenciador de escopos
void liberar_gerenciador_escopo(GerenciadorEscopo* gerenciador);

#endif