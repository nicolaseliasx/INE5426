#ifndef ACOES_SEMANTICAS_H
#define ACOES_SEMANTICAS_H

#include "no_ast.h"
#include "gerenciador_escopo.h"
#include <stddef.h> // Para size_t
#include <stdbool.h> // Para bool, se necessário

typedef struct {
    char* var;
    int contador_vetor;
    char* tipo;
    char* inicio;
    char* ret;
    int contador_param;
    char operacao;
} DadosSemanticos;

// Namespace CODE (Ações de Geração de Código)
void CODIGO_relop_action(NoAST* no_pai, GerenciadorEscopo* gerenciador);
void CODIGO_lidar_break(NoAST* no_pai, GerenciadorEscopo* gerenciador);
void CODIGO_lidar_leitura(NoAST* no_pai, GerenciadorEscopo* gerenciador);
void CODIGO_lidar_impressao(NoAST* no_pai, GerenciadorEscopo* gerenciador);
void CODIGO_obter_variavel_lvalue(NoAST* no_pai, GerenciadorEscopo* gerenciador); // get_lvalue_var
void CODIGO_obter_valor_retorno(NoAST* no_pai, GerenciadorEscopo* gerenciador); // get_return_val
void CODIGO_lidar_retorno(NoAST* no_pai, GerenciadorEscopo* gerenciador); // handle_return
void CODIGO_lidar_retorno2(NoAST* no_pai, GerenciadorEscopo* gerenciador); // handle_return2

void CODIGO_obter_parametros(NoAST* no_pai, GerenciadorEscopo* gerenciador); // get_params
void CODIGO_chamada_paramlist(NoAST* no_pai, GerenciadorEscopo* gerenciador); // paramlistcall
void CODIGO_chamada_funcao(NoAST* no_pai, GerenciadorEscopo* gerenciador); // funccall

void CODIGO_ir_para_principal(NoAST* no_pai, GerenciadorEscopo* gerenciador); // goto_main

void CODIGO_funcdef_h(NoAST* no_pai, GerenciadorEscopo* gerenciador); // funcdef_h
void CODIGO_funcdef_s(NoAST* no_pai, GerenciadorEscopo* gerenciador); // funcdef_s

void CODIGO_acao_else_vazio(NoAST* no_pai, GerenciadorEscopo* gerenciador); // empty_else_action
void CODIGO_acao_else_h(NoAST* no_pai, GerenciadorEscopo* gerenciador); // else_action_h
void CODIGO_acao_else_s(NoAST* no_pai, GerenciadorEscopo* gerenciador); // else_action_s
void CODIGO_acao_if_s(NoAST* no_pai, GerenciadorEscopo* gerenciador); // if_action_s

void CODIGO_acao_for(NoAST* no_pai, GerenciadorEscopo* gerenciador); // for_action
void CODIGO_herdar_proximo_for(NoAST* no_pai, GerenciadorEscopo* gerenciador); // inherit_for_next

void CODIGO_atribuicao(NoAST* no_pai, GerenciadorEscopo* gerenciador); // assign

void CODIGO_definir_valor_expressao(NoAST* no_pai, GerenciadorEscopo* gerenciador); // set_expression_val

void CODIGO_obter_codigo_filhos(NoAST* no_pai, GerenciadorEscopo* gerenciador); // get_code_from_childs
void CODIGO_obter_codigo_filhos_2(NoAST* no_pai, GerenciadorEscopo* gerenciador); // get_code_from_childs_2

void CODIGO_herdar_proximo(NoAST* no_pai, GerenciadorEscopo* gerenciador); // inherit_next
void CODIGO_definir_rotulo_final(NoAST* no_pai, GerenciadorEscopo* gerenciador); // set_final_label
void CODIGO_definir_rotulo(NoAST* no_pai, GerenciadorEscopo* gerenciador); // set_label
void CODIGO_gerar_codigo_final(NoAST* no_pai, GerenciadorEscopo* gerenciador); // gen_final_code
void CODIGO_imprimir_codigo(NoAST* no_pai, GerenciadorEscopo* gerenciador); // print_code

// Namespace SCOPE (Gerência de Escopos)
void ESCOPO_acao1(NoAST* no_pai, GerenciadorEscopo* gerenciador);
void ESCOPO_acao2(NoAST* no_pai, GerenciadorEscopo* gerenciador);
void ESCOPO_acao3(NoAST* no_pai, GerenciadorEscopo* gerenciador);
void ESCOPO_acao4(NoAST* no_pai, GerenciadorEscopo* gerenciador);
void ESCOPO_acao5(NoAST* no_pai, GerenciadorEscopo* gerenciador);
void ESCOPO_acao6(NoAST* no_pai, GerenciadorEscopo* gerenciador);
void ESCOPO_acao7(NoAST* no_pai, GerenciadorEscopo* gerenciador);

// Namespace DEC (Declaração de Tipos na Tabela de Símbolos)
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

// Namespace DECLARE_VERIFY (Verificação de Uso de Variável Declarada)
void DECLARAR_VERIFICAR_acao1(NoAST* no_pai, GerenciadorEscopo* gerenciador); // action1
void DECLARAR_VERIFICAR_acao2(NoAST* no_pai, GerenciadorEscopo* gerenciador); // action2

// Namespace BREAK (Verificação de break dentro de escopo de comando de repetição)
void BREAK_acao1(NoAST* no_pai, GerenciadorEscopo* gerenciador);

char* AUXILIAR_obter_tipo(const char* tipo, int contador_vetor);
int* AUXILIAR_obter_tamanhos_vetor(const char* tipo, int* num_elementos);

// Namespace EXPA (Ações para Expressões)
void EXPA_contador_vetor(NoAST* no_pai, GerenciadorEscopo* gerenciador); // arrayCounter
void EXPA_passar_numero(NoAST* no_pai, GerenciadorEscopo* gerenciador); // passNum
void EXPA_imprimir_expressao0_h(NoAST* no_pai, GerenciadorEscopo* gerenciador); // printExpression0_h
void EXPA_imprimir_expressao0(NoAST* no_pai, GerenciadorEscopo* gerenciador); // printExpression0
void EXPA_imprimir_expressao1(NoAST* no_pai, GerenciadorEscopo* gerenciador); // printExpression1
void EXPA_imprimir_expressao2(NoAST* no_pai, GerenciadorEscopo* gerenciador); // printExpression2
void EXPA_avaliar_identificador(NoAST* no_pai, GerenciadorEscopo* gerenciador); // evaluateIdent
void EXPA_lexema_para_valor(NoAST* no_pai, GerenciadorEscopo* gerenciador); // lexemeToValue
void EXPA_definir_operacao(NoAST* no_pai, GerenciadorEscopo* gerenciador); // setOperation
void EXPA_ident_para_cima(NoAST* no_pai, GerenciadorEscopo* gerenciador); // identUp
void EXPA_valor_para_cima(NoAST* no_pai, GerenciadorEscopo* gerenciador); // valueUp
void EXPA_valor_segundo_filho_para_cima(NoAST* no_pai, GerenciadorEscopo* gerenciador); // secondChildValueUp
void EXPA_gerar_no(NoAST* no_pai, GerenciadorEscopo* gerenciador); // generateNode
void EXPA_definir_operacao2(NoAST* no_pai, GerenciadorEscopo* gerenciador); // setOperation2
void EXPA_termo(NoAST* no_pai, GerenciadorEscopo* gerenciador); // term

#endif // ACOES_SEMANTICAS_H