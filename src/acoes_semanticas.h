#ifndef ACOES_SEMANTICAS_H
#define ACOES_SEMANTICAS_H

#include "no_ast.h"
#include "gerenciador_escopo.h"
#include <stddef.h>
#include <stdbool.h>

void inicializar_resolvedor_global(); 

// --- Ações de Geração de Código ---
void CODIGO_relop_action(NoAST* no_pai, GerenciadorEscopo* gerenciador);
void CODIGO_lidar_break(NoAST* no_pai, GerenciadorEscopo* gerenciador);
void CODIGO_lidar_leitura(NoAST* no_pai, GerenciadorEscopo* gerenciador);
void CODIGO_lidar_impressao(NoAST* no_pai, GerenciadorEscopo* gerenciador);
void CODIGO_obter_variavel_lvalue(NoAST* no_pai, GerenciadorEscopo* gerenciador);
void CODIGO_obter_valor_retorno(NoAST* no_pai, GerenciadorEscopo* gerenciador);
void CODIGO_lidar_retorno(NoAST* no_pai, GerenciadorEscopo* gerenciador);
void CODIGO_lidar_retorno2(NoAST* no_pai, GerenciadorEscopo* gerenciador);
void CODIGO_obter_parametros(NoAST* no_pai, GerenciadorEscopo* gerenciador);
void CODIGO_chamada_paramlist(NoAST* no_pai, GerenciadorEscopo* gerenciador);
void CODIGO_chamada_funcao(NoAST* no_pai, GerenciadorEscopo* gerenciador);
void CODIGO_ir_para_principal(NoAST* no_pai, GerenciadorEscopo* gerenciador);
void CODIGO_funcdef_h(NoAST* no_pai, GerenciadorEscopo* gerenciador);
void CODIGO_funcdef_s(NoAST* no_pai, GerenciadorEscopo* gerenciador);
void CODIGO_acao_else_vazio(NoAST* no_pai, GerenciadorEscopo* gerenciador);
void CODIGO_acao_else_h(NoAST* no_pai, GerenciadorEscopo* gerenciador);
void CODIGO_acao_else_s(NoAST* no_pai, GerenciadorEscopo* gerenciador);
void CODIGO_acao_if_s(NoAST* no_pai, GerenciadorEscopo* gerenciador);
void CODIGO_acao_for(NoAST* no_pai, GerenciadorEscopo* gerenciador);
void CODIGO_herdar_proximo_for(NoAST* no_pai, GerenciadorEscopo* gerenciador);
void CODIGO_atribuicao(NoAST* no_pai, GerenciadorEscopo* gerenciador);
void CODIGO_definir_valor_expressao(NoAST* no_pai, GerenciadorEscopo* gerenciador);
void CODIGO_obter_codigo_filhos(NoAST* no_pai, GerenciadorEscopo* gerenciador);
void CODIGO_montar_lista_de_statements(NoAST* no_pai, GerenciadorEscopo* gerenciador);
void CODIGO_herdar_proximo(NoAST* no_pai, GerenciadorEscopo* gerenciador);
void CODIGO_definir_rotulo_final(NoAST* no_pai, GerenciadorEscopo* gerenciador);
void CODIGO_gerar_codigo_final(NoAST* no_pai, GerenciadorEscopo* gerenciador);
void CODIGO_imprimir_codigo(NoAST* no_pai, GerenciadorEscopo* gerenciador);

// --- Ações de Gerenciamento de Escopo ---
void ESCOPO_acao1(NoAST* no_pai, GerenciadorEscopo* gerenciador);
void ESCOPO_acao2(NoAST* no_pai, GerenciadorEscopo* gerenciador);
void ESCOPO_acao3(NoAST* no_pai, GerenciadorEscopo* gerenciador);
void ESCOPO_acao4(NoAST* no_pai, GerenciadorEscopo* gerenciador);
void ESCOPO_acao5(NoAST* no_pai, GerenciadorEscopo* gerenciador);
void ESCOPO_acao6(NoAST* no_pai, GerenciadorEscopo* gerenciador);
void ESCOPO_acao7(NoAST* no_pai, GerenciadorEscopo* gerenciador);

// --- Ações de Declaração de Tipos ---
void DEC_acao1(NoAST* no_pai, GerenciadorEscopo* gerenciador);
void DEC_acao2(NoAST* no_pai, GerenciadorEscopo* gerenciador);
void DEC_acao3(NoAST* no_pai, GerenciadorEscopo* gerenciador);
void DEC_acao4(NoAST* no_pai, GerenciadorEscopo* gerenciador);
void DEC_acao5(NoAST* no_pai, GerenciadorEscopo* gerenciador);
void DEC_acao6(NoAST* no_pai, GerenciadorEscopo* gerenciador);
void DEC_acao7(NoAST* no_pai, GerenciadorEscopo* gerenciador);
void DEC_acao8(NoAST* no_pai, GerenciadorEscopo* gerenciador);
void DEC_acao9(NoAST* no_pai, GerenciadorEscopo* gerenciador);
void DEC_acao10(NoAST* no_pai, GerenciadorEscopo* gerenciador);
void DEC_acao11(NoAST* no_pai, GerenciadorEscopo* gerenciador);

// --- Ações de Verificação de Declarações ---
void DECLARAR_VERIFICAR_acao1(NoAST* no_pai, GerenciadorEscopo* gerenciador);
void DECLARAR_VERIFICAR_acao2(NoAST* no_pai, GerenciadorEscopo* gerenciador);

// --- Ações de Verificação de Break ---
void BREAK_acao1(NoAST* no_pai, GerenciadorEscopo* gerenciador);

// --- Funções Auxiliares ---
char* AUXILIAR_obter_tipo(const char* tipo, int contador_vetor);
int* AUXILIAR_obter_tamanhos_vetor(const char* tipo, int* num_elementos);

// --- Ações para Expressões ---
void EXPA_contador_vetor(NoAST* no_pai, GerenciadorEscopo* gerenciador);
void EXPA_passar_numero(NoAST* no_pai, GerenciadorEscopo* gerenciador);
void EXPA_imprimir_expressao0_h(NoAST* no_pai, GerenciadorEscopo* gerenciador);
void EXPA_imprimir_expressao0(NoAST* no_pai, GerenciadorEscopo* gerenciador);
void EXPA_imprimir_expressao1(NoAST* no_pai, GerenciadorEscopo* gerenciador);
void EXPA_imprimir_expressao2(NoAST* no_pai, GerenciadorEscopo* gerenciador);
void EXPA_lexema_para_valor(NoAST* no_pai, GerenciadorEscopo* gerenciador);
void EXPA_definir_operacao(NoAST* no_pai, GerenciadorEscopo* gerenciador);
void EXPA_ident_para_cima(NoAST* no_pai, GerenciadorEscopo* gerenciador);
void EXPA_valor_para_cima(NoAST* no_pai, GerenciadorEscopo* gerenciador);
void EXPA_valor_segundo_filho_para_cima(NoAST* no_pai, GerenciadorEscopo* gerenciador);
void EXPA_gerar_no(NoAST* no_pai, GerenciadorEscopo* gerenciador);
void EXPA_definir_operacao2(NoAST* no_pai, GerenciadorEscopo* gerenciador);
void EXPA_termo(NoAST* no_pai, GerenciadorEscopo* gerenciador);
void EXPA_inicializar_contador_vetor(NoAST* no_pai, GerenciadorEscopo* gerenciador);

#endif // ACOES_SEMANTICAS_H