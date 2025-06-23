#include "acoes_semanticas.h"
#include "gerenciador_escopo.h"
#include "no_ast.h"
#include "tabela_simbolos.h"
#include "token.h"
#include "resolvedor_expressao.h" // Para funções de resolução de expressões
#include "lista_codigo.h"         // Para manipulação de listas de código
#include "vetor_strings.h"        // Para vetores dinâmicos de strings
#include "erros.h"                // Para erros semânticos
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// =================================================================
// Implementações do namespace CODIGO (Geração de código)
// =================================================================

void CODIGO_relop_action(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // EXPRESSION' -> relop NUMEXPRESSION
    // EXPRESSION'.code = NUMEXPRESSION.code ||
    // EXPRESSION'.var = EXPRESSION'.var relop NUMEXPRESSION.var
    // NUMEXPRESSION.var [0] EXPRESSION' -> relop NUMEXPRESSION [0]
    NoAST* numexpression = no_pai->filhos[1];
    NoAST* relop = no_pai->filhos[0];

    // Splice numexpression->codigo into no_pai->codigo
    lista_codigo_adicionar_lista(no_pai->codigo, numexpression->codigo);

    // Add the relational operation line
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "%s = %s %s %s",
             no_pai->res_var_codigo.var,
             no_pai->res_var_codigo.var,
             relop->token->lexema,
             numexpression->res_var_codigo.var);
    lista_codigo_adicionar_string(no_pai->codigo, strdup(buffer));
}

void CODIGO_lidar_break(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // STATEMENT -> break ;
    // STATEMENT.code = goto STATEMENT.for_nxt [0]
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "goto %s", no_pai->para_proximo);
    lista_codigo_adicionar_string(no_pai->codigo, strdup(buffer));
}

void CODIGO_lidar_leitura(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // READSTAT -> read LVALUE
    // READSTAT.code = read LVALUE.var [1]
    NoAST* lvalue = no_pai->filhos[1];
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "read %s", lvalue->res_var_codigo.var);
    lista_codigo_adicionar_string(no_pai->codigo, strdup(buffer));
}

void CODIGO_lidar_impressao(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // PRINTSTAT -> print EXPRESSION
    // PRINTSTAT.code = EXPRESSION.code || print EXPRESSION.var [0]
    NoAST* expressao = no_pai->filhos[1];

    lista_codigo_adicionar_lista(no_pai->codigo, expressao->codigo);

    char buffer[256];
    snprintf(buffer, sizeof(buffer), "print %s", expressao->res_var_codigo.var);
    lista_codigo_adicionar_string(no_pai->codigo, strdup(buffer));
}

void CODIGO_obter_variavel_lvalue(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // LVALUE -> ident ALLOCAUX
    // LVALUE.var = ident.lexema [0]
    NoAST* ident = no_pai->filhos[0];
    no_pai->res_var_codigo.var = strdup(ident->token->lexema);
}

void CODIGO_obter_valor_retorno(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // ATRIBSTAT' -> FUNCCALL
    // ATRIBSTAT.var = $rv
    no_pai->res_var_codigo.var = strdup("$rv");
}

void CODIGO_lidar_retorno(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // RETURNSTAT -> return RETURNSTAT'
    // RETURNSTAT.code = RETURNSTAT'.code || goto RETURNSTAT.nxt
    NoAST* returnstat_linha = no_pai->filhos[1];
    lista_codigo_adicionar_lista(no_pai->codigo, returnstat_linha->codigo);

    char buffer[256];
    snprintf(buffer, sizeof(buffer), "goto %s", no_pai->proximo);
    lista_codigo_adicionar_string(no_pai->codigo, strdup(buffer));
}

void CODIGO_lidar_retorno2(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // RETURNSTAT' -> LVALUE
    // RETURNSTAT'.code = "$rv = LVALUE.var";
    NoAST* lvalue = no_pai->filhos[0];
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "$rv = %s", lvalue->res_var_codigo.var);
    lista_codigo_adicionar_string(no_pai->codigo, strdup(buffer));
}

void CODIGO_obter_parametros(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // PARAMLISTCALL' -> , PARAMLISTCALL
    // PARAMLISTCALL'.cnt = PARAMLISTCALL.cnt [0]
    NoAST* paramlist = no_pai->filhos[1];
    no_pai->res_var_codigo.contador_param = paramlist->res_var_codigo.contador_param;
}

void CODIGO_chamada_paramlist(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // PARAMLISTCALL -> ident PARAMLISTCALL'
    // PARAMLISTCALL.cnt = 1 + PARAMLISTCALL'.cnt [1]
    // PARAMLISTCALL.code = param ident || PARAMLISTCALL'.code [0]
    NoAST* ident = no_pai->filhos[0];
    NoAST* paramlist = no_pai->filhos[1];

    no_pai->res_var_codigo.contador_param = 1 + paramlist->res_var_codigo.contador_param;

    char buffer[256];
    snprintf(buffer, sizeof(buffer), "param %s", ident->token->lexema);
    lista_codigo_adicionar_string(no_pai->codigo, strdup(buffer));
    lista_codigo_adicionar_lista(no_pai->codigo, paramlist->codigo);
}

void CODIGO_chamada_funcao(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // FUNCCALL -> call ( ident ( PARAMLISTCALL ) )
    // FUNCCALL.return = newLabel [0]
    // FUNCCALL.code = PARAMLISTCALL.code || $r = FUNCCALL.return ||
    //                 call ident, PARAMLISTCALL.cnt || FUNCCALL.return: [1]
    NoAST* ident = no_pai->filhos[2];
    NoAST* paramlistcall = no_pai->filhos[4];

    no_pai->res_var_codigo.ret = no_ast_gerar_novo_rotulo(no_pai); // Assuming no_ast_gerar_novo_rotulo generates a new label

    lista_codigo_adicionar_lista(no_pai->codigo, paramlistcall->codigo);

    char buffer[256];
    snprintf(buffer, sizeof(buffer), "$r = %s", no_pai->res_var_codigo.ret);
    lista_codigo_adicionar_string(no_pai->codigo, strdup(buffer));

    snprintf(buffer, sizeof(buffer), "call %s %d",
             ident->token->lexema,
             paramlistcall->res_var_codigo.contador_param);
    lista_codigo_adicionar_string(no_pai->codigo, strdup(buffer));

    snprintf(buffer, sizeof(buffer), "%s:", no_pai->res_var_codigo.ret);
    lista_codigo_adicionar_string(no_pai->codigo, strdup(buffer));
}

void CODIGO_ir_para_principal(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // PROGRAM -> FUNCLIST PROGRAM'
    // PROGRAM.code -> goto MAIN || FUNCLIST.code || MAIN: || PROGRAM'.code [1]
    NoAST* funclist = no_pai->filhos[0];
    NoAST* program_linha = no_pai->filhos[1];

    lista_codigo_adicionar_string(no_pai->codigo, strdup("goto MAIN"));
    lista_codigo_adicionar_lista(no_pai->codigo, funclist->codigo);
    lista_codigo_adicionar_string(no_pai->codigo, strdup("MAIN:"));
    lista_codigo_adicionar_lista(no_pai->codigo, program_linha->codigo);
}

void CODIGO_funcdef_h(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // FUNCDEF -> def ident ( PARAMLIST ) { STATELIST }
    // begin = newlabel
    // STATELIST.nxt = 'return'
    // FUNCDEF.cod = begin: || STATELIST.code || goto return
    NoAST* ident = no_pai->filhos[1];
    no_pai->res_var_codigo.inicio = strdup(ident->token->lexema);
    NoAST* statelist = no_pai->filhos[6];
    statelist->proximo = strdup("$r");
}

void CODIGO_funcdef_s(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // FUNCDEF -> def ident ( PARAMLIST ) { STATELIST }
    // FUNCDEF.cod = begin: || STATELIST.code || goto STATELIST.nxt
    NoAST* statelist = no_pai->filhos[6];

    char buffer[256];
    snprintf(buffer, sizeof(buffer), "%s:", no_pai->res_var_codigo.inicio);
    lista_codigo_adicionar_string(no_pai->codigo, strdup(buffer));

    lista_codigo_adicionar_lista(no_pai->codigo, statelist->codigo);

    snprintf(buffer, sizeof(buffer), "goto %s", statelist->proximo);
    lista_codigo_adicionar_string(no_pai->codigo, strdup(buffer));
}

void CODIGO_acao_else_vazio(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // IFSTAT' -> '' (epsilon)
    // IFSTAT'.begin = IFSTAT'.nxt
    no_pai->res_var_codigo.inicio = strdup(no_pai->proximo);
}

void CODIGO_acao_else_h(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // IFSTAT' -> else { STATEMENT }
    // begin = newlabel()
    // STATEMENT.nxt = IFSTAT'.nxt
    // IFSTAT'.code = begin: || statement.code || goto STATEMENT.nxt
    NoAST* statement = no_pai->filhos[2];
    statement->proximo = strdup(no_pai->proximo);
    no_pai->res_var_codigo.inicio = no_ast_gerar_novo_rotulo(no_pai);
}

void CODIGO_acao_else_s(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // IFSTAT' -> else { STATEMENT }
    // IFSTAT'.code = begin: || statement.code || goto STATEMENT.nxt
    NoAST* statement = no_pai->filhos[2];

    char buffer[256];
    snprintf(buffer, sizeof(buffer), "%s:", no_pai->res_var_codigo.inicio);
    lista_codigo_adicionar_string(no_pai->codigo, strdup(buffer));

    lista_codigo_adicionar_lista(no_pai->codigo, statement->codigo);

    snprintf(buffer, sizeof(buffer), "goto %s", statement->proximo);
    lista_codigo_adicionar_string(no_pai->codigo, strdup(buffer));
}

void CODIGO_acao_if_s(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // IFSTAT -> if ( EXPRESSION ) { STATEMENT } IFSTAT'
    // ALL_PRODS.nxt = IFSTAT.next
    // IFSTAT.code = if not EXPRESS.val goto IFSTAT'.begin ||
    //               STATEMENT.code || goto STATEMENT.nxt
    //               IFSTAT'.code
    NoAST* expressao = no_pai->filhos[2];
    NoAST* statement = no_pai->filhos[5];
    NoAST* ifstat_linha = no_pai->filhos[7];

    lista_codigo_adicionar_lista(no_pai->codigo, expressao->codigo);

    char buffer[256];
    snprintf(buffer, sizeof(buffer), "if False %s goto %s",
             expressao->res_var_codigo.var,
             ifstat_linha->res_var_codigo.inicio);
    lista_codigo_adicionar_string(no_pai->codigo, strdup(buffer));

    lista_codigo_adicionar_lista(no_pai->codigo, statement->codigo);

    snprintf(buffer, sizeof(buffer), "goto %s", statement->proximo);
    lista_codigo_adicionar_string(no_pai->codigo, strdup(buffer));

    lista_codigo_adicionar_lista(no_pai->codigo, ifstat_linha->codigo);
}

void CODIGO_acao_for(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // FORSTAT -> for ( ATRIBSTAT1 ; EXPRESSION ; ATRIBSTAT2 ) STATEMENT
    // ALL_PRODS.nxt = FORSTAT.next
    // begin = newlabel()
    // FORSTAT.code = ATRIBSTAT1 || begin: || EXPRESSION
    //                || if not EXPRESSION.val goto FORSTAT.next ||
    //                STATEMENT.code || ATRIBSTAT2
    //                || goto begin
    NoAST* atribstat1 = no_pai->filhos[2];
    NoAST* expressao = no_pai->filhos[4];
    NoAST* atribstat2 = no_pai->filhos[6];
    NoAST* statement = no_pai->filhos[8];

    char* rotulo_inicio_laco = no_ast_gerar_novo_rotulo(no_pai);

    lista_codigo_adicionar_lista(no_pai->codigo, atribstat1->codigo);

    char buffer[256];
    snprintf(buffer, sizeof(buffer), "%s:", rotulo_inicio_laco);
    lista_codigo_adicionar_string(no_pai->codigo, strdup(buffer));

    lista_codigo_adicionar_lista(no_pai->codigo, expressao->codigo);

    snprintf(buffer, sizeof(buffer), "if False %s goto %s",
             expressao->res_var_codigo.var,
             no_pai->proximo); // FORSTAT.next is no_pai->proximo
    lista_codigo_adicionar_string(no_pai->codigo, strdup(buffer));

    lista_codigo_adicionar_lista(no_pai->codigo, statement->codigo);
    lista_codigo_adicionar_lista(no_pai->codigo, atribstat2->codigo);

    snprintf(buffer, sizeof(buffer), "goto %s", rotulo_inicio_laco);
    lista_codigo_adicionar_string(no_pai->codigo, strdup(buffer));

    free(rotulo_inicio_laco); // Free the dynamically allocated label
}

void CODIGO_herdar_proximo_for(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // FORSTAT -> for ( ATRIBSTAT1 ; EXPRESSION ; ATRIBSTAT2 ) STATEMENT
    NoAST* statement = no_pai->filhos[8];
    statement->para_proximo = strdup(no_pai->proximo); // Inherit for_nxt from father
}

void CODIGO_atribuicao(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // ATRIBSTAT -> LVALUE = ATRIBSTAT'
    // ATRIBSTAT.code = ATRSBSTAT'.code || LVALUE.lexema = ATRIBSTAT'.var
    NoAST* lvalue = no_pai->filhos[0];
    NoAST* atribstat_linha = no_pai->filhos[2];
    char buffer[256];
    char* assign_code;

    // Splice code from lvalue and atribstat_linha
    lista_codigo_adicionar_lista(no_pai->codigo, lvalue->codigo);
    lista_codigo_adicionar_lista(no_pai->codigo, atribstat_linha->codigo);

    if (lvalue->sdt_mat.contador_vetor > 0) {
        // Assuming mathSDT.node->value holds the calculated array address
        // This needs a proper way to get the resolved array address.
        // For simplicity, directly using value for now.
        snprintf(buffer, sizeof(buffer), "%s = %s",
                 lvalue->sdt_mat.no->valor, // This might need more resolution
                 atribstat_linha->res_var_codigo.var);
        assign_code = strdup(buffer);
    } else {
        snprintf(buffer, sizeof(buffer), "%s = %s",
                 lvalue->filhos[0]->token->lexema, // Direct identifier lexeme
                 atribstat_linha->res_var_codigo.var);
        assign_code = strdup(buffer);
    }
    lista_codigo_adicionar_string(no_pai->codigo, assign_code);
}

void CODIGO_definir_valor_expressao(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // ATRIBSTAT' -> EXPRESSION
    NoAST* expressao = no_pai->filhos[0];
    no_pai->res_var_codigo.var = strdup(expressao->res_var_codigo.var);
    lista_codigo_adicionar_lista(no_pai->codigo, expressao->codigo);
}

void CODIGO_definir_rotulo(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // STATELIST -> STATEMENT STATELIST'
    NoAST* statement = no_pai->filhos[0];
    statement->proximo = no_ast_gerar_novo_rotulo(no_pai); // Generates a new label for STATEMENT.nxt
}

void CODIGO_obter_codigo_filhos(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // Generic action to splice code from all children to the father
    for (size_t i = 0; i < no_pai->num_filhos; i++) {
        lista_codigo_adicionar_lista(no_pai->codigo, no_pai->filhos[i]->codigo);
    }
}

void CODIGO_obter_codigo_filhos_2(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // STATELIST -> STATEMENT STATELIST'
    NoAST* statement = no_pai->filhos[0];
    NoAST* statelist_linha = no_pai->filhos[1];

    lista_codigo_adicionar_lista(no_pai->codigo, statement->codigo);

    char buffer[256];
    snprintf(buffer, sizeof(buffer), "%s:", statement->proximo);
    lista_codigo_adicionar_string(no_pai->codigo, strdup(buffer));

    lista_codigo_adicionar_lista(no_pai->codigo, statelist_linha->codigo);
}

void CODIGO_herdar_proximo(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // Generic action to inherit 'nxt' and 'for_nxt' attributes to children
    for (size_t i = 0; i < no_pai->num_filhos; i++) {
        if (no_pai->filhos[i]) { // Check for non-NULL child
            if (no_pai->proximo) {
                no_pai->filhos[i]->proximo = strdup(no_pai->proximo);
            }
            if (no_pai->para_proximo) {
                no_pai->filhos[i]->para_proximo = strdup(no_pai->para_proximo);
            }
        }
    }
}

void CODIGO_definir_rotulo_final(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    no_pai->proximo = strdup("END");
}

void CODIGO_gerar_codigo_final(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "%s:", no_pai->proximo);
    lista_codigo_adicionar_string(no_pai->codigo, strdup(buffer));
}

void CODIGO_imprimir_codigo(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // Print each line of code
    // Assuming lista_codigo_imprimir_tudo exists
    // Otherwise, iterate through no_pai->codigo and print each string
    lista_codigo_imprimir_tudo(no_pai->codigo);
}


// =================================================================
// Implementações do namespace ESCOPO (Gerenciamento de escopos)
// =================================================================

void ESCOPO_acao1(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // PROGRAM  ->  STATEMENT
    // PROGRAM  ->  FUNCLIST PROGRAM'
    // PROGRAM  ->  ''
    // STATEMENT  ->  { STATELIST }
    gerenciador_abrir_escopo(gerenciador, ""); // Default scope type
}

void ESCOPO_acao2(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // FUNCDEF  ->  def ident ( PARAMLIST ) { STATELIST }
    // STATEMENT  ->  { STATELIST }
    // IFSTAT  -> if ( EXPRESSION ) { STATEMENT } IFSTAT'
    // IFSTAT' -> else { STATEMENT }
    // FORSTAT  ->  for ( ATRIBSTAT ; EXPRESSION ; ATRIBSTAT ) STATEMENT
    gerenciador_fechar_escopo(gerenciador);
}

void ESCOPO_acao3(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // FUNCDEF  ->  def ident ( PARAMLIST ) { STATELIST }
    // PARAMLIST  ->  int ident PARAMLIST'
    // PARAMLIST  ->  float ident PARAMLIST'
    // PARAMLIST  ->  string ident PARAMLIST'
    // VARDECL  ->  int ident INDEX
    // VARDECL  ->  float ident INDEX
    // VARDECL  ->  string ident INDEX
    // add_symbol(ident)
    NoAST* ident = no_pai->filhos[1];
    gerenciador_adicionar_simbolo(gerenciador, ident->token);
}

void ESCOPO_acao4(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // FUNCDEF  ->  def ident ( PARAMLIST ) { STATELIST }
    gerenciador_abrir_escopo(gerenciador, "function");
}

void ESCOPO_acao5(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // IFSTAT  -> if ( EXPRESSION ) { STATEMENT } IFSTAT'
    gerenciador_abrir_escopo(gerenciador, "if");
}

void ESCOPO_acao6(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // IFSTAT' -> else { STATEMENT }
    gerenciador_abrir_escopo(gerenciador, "else");
}

void ESCOPO_acao7(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // FORSTAT  ->  for ( ATRIBSTAT ; EXPRESSION ; ATRIBSTAT ) STATEMENT
    gerenciador_abrir_escopo(gerenciador, "for");
}

// =================================================================
// Implementações do namespace DEC (Declarações de tipos)
// =================================================================

void DEC_acao1(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // FUNCDEF -> def ident ( PARAMLIST ) { STATELIST }
    // add_type(ident.val_lex, "function")
    NoAST* ident = no_pai->filhos[1];
    gerenciador_adicionar_tipo(gerenciador, ident->token, strdup("function"));
}

void DEC_acao2(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // PARAMLIST -> int ident PARAMLIST'
    // add_type(ident.val_lex, "int")
    NoAST* ident = no_pai->filhos[1];
    gerenciador_adicionar_tipo(gerenciador, ident->token, strdup("int"));
}

void DEC_acao3(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // PARAMLIST -> float ident PARAMLIST'
    // add_type(ident.val_lex, "float")
    NoAST* ident = no_pai->filhos[1];
    gerenciador_adicionar_tipo(gerenciador, ident->token, strdup("float"));
}

void DEC_acao4(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // PARAMLIST -> string ident PARAMLIST'
    // add_type(ident.val_lex, "string")
    NoAST* ident = no_pai->filhos[1];
    gerenciador_adicionar_tipo(gerenciador, ident->token, strdup("string"));
}

void DEC_acao5(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // INDEX -> [int_constant] INDEX1
    // INDEX1.initial_type = INDEX.initial_type
    NoAST* index = no_pai;
    NoAST* index1 = no_pai->filhos[3];
    index1->sdt_dec.tipo_inicial = strdup(index->sdt_dec.tipo_inicial);
}

void DEC_acao6(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // INDEX -> [int_constant] INDEX1
    // INDEX.type = array(int_constant.val_lex, INDEX1.type)
    NoAST* index = no_pai;
    NoAST* index1 = no_pai->filhos[3];
    NoAST* int_const = no_pai->filhos[1];
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "array(%s, %s)", int_const->token->lexema, index1->sdt_dec.tipo);
    index->sdt_dec.tipo = strdup(buffer);
}

void DEC_acao7(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // INDEX-> '' (epsilon)
    // INDEX.type = INDEX.initial_type
    NoAST* index = no_pai;
    index->sdt_dec.tipo = strdup(index->sdt_dec.tipo_inicial);
}

void DEC_acao8(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // VARDECL -> int ident INDEX
    // INDEX.initial_type = "int"
    NoAST* index = no_pai->filhos[2];
    index->sdt_dec.tipo_inicial = strdup("int");
}

void DEC_acao9(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // VARDECL -> float ident INDEX
    // INDEX.initial_type = "float"
    NoAST* index = no_pai->filhos[2];
    index->sdt_dec.tipo_inicial = strdup("float");
}

void DEC_acao10(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // VARDECL -> string ident INDEX
    // INDEX.initial_type = "string"
    NoAST* index = no_pai->filhos[2];
    index->sdt_dec.tipo_inicial = strdup("string");
}

void DEC_acao11(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // VARDECL -> int ident INDEX
    // VARDECL -> float ident INDEX
    // VARDECL -> string ident INDEX
    // add_type(ident.val_lex, INDEX.type)
    NoAST* ident = no_pai->filhos[1];
    NoAST* index = no_pai->filhos[2];
    gerenciador_adicionar_tipo(gerenciador, ident->token, strdup(index->sdt_dec.tipo));
}

// =================================================================
// Implementações do namespace DECLARAR_VERIFICAR (Verificação de declarações)
// =================================================================

void DECLARAR_VERIFICAR_acao1(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // FUNCCALL -> call ( ident ( PARAMLISTCALL ) )
    // is_declared(ident.val_lex)
    // is_type(ident.val_lex, "function")
    NoAST* ident = no_pai->filhos[2];
    if (!gerenciador_esta_declarado(gerenciador, ident->token)) {
        char msg_error[512];
        snprintf(msg_error, sizeof(msg_error), "%s usado antes da declaracao na linha %d:%d",
                 ident->token->lexema, ident->token->linha, ident->token->coluna);
        // Assuming your 'erros.h' has a function like `reportar_erro_semantico`
        reportar_erro_semantico(msg_error);
    }
    if (!gerenciador_e_do_tipo(gerenciador, ident->token, "function")) {
        char msg_error[512];
        snprintf(msg_error, sizeof(msg_error), "%s na linha %d:%d nao e uma funcao",
                 ident->token->lexema, ident->token->linha, ident->token->coluna);
        reportar_erro_semantico(msg_error);
    }
}

void DECLARAR_VERIFICAR_acao2(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // PARAMLISTCALL -> ident PARAMLISTCALL'
    // LVALUE -> ident ALLOCAUX
    // is_declared(ident.val_lex)
    NoAST* ident = no_pai->filhos[0]; // Assuming ident is always the first child
    if (!gerenciador_esta_declarado(gerenciador, ident->token)) {
        char msg_error[512];
        snprintf(msg_error, sizeof(msg_error), "%s usado antes da declaracao na linha %d:%d",
                 ident->token->lexema, ident->token->linha, ident->token->coluna);
        reportar_erro_semantico(msg_error);
    }
}

// =================================================================
// Implementações do namespace BREAK (Verificação de break)
// =================================================================

void BREAK_acao1(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // STATEMENT -> break ;
    if (!gerenciador_esta_em_escopo(gerenciador, "for")) {
        NoAST* break_t = no_pai->filhos[0];
        char msg_error[512];
        snprintf(msg_error, sizeof(msg_error), "break usado fora de um comando for na linha %d:%d",
                 break_t->token->linha, break_t->token->coluna);
        reportar_erro_semantico(msg_error);
    }
}

// =================================================================
// Implementações do namespace AUXILIAR (Funções auxiliares)
// =================================================================

char* AUXILIAR_obter_tipo(const char* tipo_str, int contador_vetor) {
    // This C implementation requires careful memory management.
    // The returned char* must be freed by the caller.
    char f = tipo_str[0];
    char* final_tipo = NULL;

    if (f == 'i') {
        return strdup("int");
    } else if (f == 'f') {
        return strdup("float");
    } else if (f == 's') {
        return strdup("string");
    }

    if (!(f == 'a')) {
        char msg_error[256];
        snprintf(msg_error, sizeof(msg_error), "TIPO INVALIDO: %s", tipo_str);
        reportar_erro_semantico(msg_error);
        return NULL; // Should not reach here if error reporting exits
    }

    int counter = 0;
    const char* ptr = tipo_str;
    while (*ptr != '\0') {
        if (*ptr == '(') {
            counter++;
        } else if (*ptr == 'i') {
            final_tipo = strdup("int");
        } else if (*ptr == 'f') {
            final_tipo = strdup("float");
        } else if (*ptr == 's') {
            final_tipo = strdup("string");
        }
        ptr++;
    }

    if (counter != contador_vetor) {
        reportar_erro_semantico("tentativa de operacoes com arrays com numero incorreto de dimensoes");
        free(final_tipo); // Free if it was allocated
        return NULL;
    }
    return final_tipo; // Caller must free this
}

int* AUXILIAR_obter_tamanhos_vetor(const char* tipo_str, int* num_elementos) {
    // This C implementation requires careful memory management.
    // The returned int* must be freed by the caller.
    VetorStrings* temp_values = vetor_strings_criar(); // Using a dynamic string vector as a temp store

    const char* ptr = tipo_str;
    while (*ptr != '\0') {
        if (*ptr >= '0' && *ptr <= '9') {
            // Extract the number
            char num_buffer[16];
            int i = 0;
            while (*ptr >= '0' && *ptr <= '9' && i < 15) {
                num_buffer[i++] = *ptr;
                ptr++;
            }
            num_buffer[i] = '\0';
            vetor_strings_adicionar(temp_values, strdup(num_buffer));
            ptr--; // Decrement to compensate for the last ptr++ in inner loop
        }
        ptr++;
    }

    *num_elementos = vetor_strings_tamanho(temp_values);
    int* values = (int*)malloc(sizeof(int) * (*num_elementos));
    if (values == NULL) {
        // Handle allocation error
        reportar_erro_semantico("Erro de alocacao de memoria para tamanhos de vetor.");
        vetor_strings_liberar(temp_values);
        return NULL;
    }

    for (size_t i = 0; i < *num_elementos; i++) {
        values[i] = atoi(vetor_strings_obter(temp_values, i));
    }

    vetor_strings_liberar(temp_values); // Free the temp string vector and its contents
    return values; // Caller must free this
}


// =================================================================
// Implementações do namespace EXPA (Ações para expressões)
// =================================================================

void EXPA_avaliar_identificador(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // LVALUE -> IDENT ALLOCAUX
    NoAST* lvalue_node = no_pai;
    NoAST* ident_node = no_pai->filhos[0];
    NoAST* allocaux_node = no_pai->filhos[1];

    char* type_str = gerenciador_obter_tipo(gerenciador, ident_node->token);
    int num_elements;
    int* max_dims = AUXILIAR_obter_tamanhos_vetor(type_str, &num_elements);

    lvalue_node->sdt_mat.contador_vetor = allocaux_node->sdt_mat.contador_vetor;

    lista_codigo_adicionar_lista(lvalue_node->codigo, allocaux_node->codigo);

    if (lvalue_node->sdt_mat.contador_vetor > 0) {
        // This part deals with array indexation and needs careful implementation
        // in C to match the C++ `ExpressionTreeResolver::generateTempVar()` and `Node` creation.
        VetorStrings* array_indexation_vars = allocaux_node->sdt_mat.vetor_array_var; // Assuming this is a VectorStrings*

        char* somador_temp_var = resolvedor_expressao_gerar_var_temp();
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "%s = 0", somador_temp_var);
        lista_codigo_adicionar_string(lvalue_node->codigo, strdup(buffer));

        int counter = 0;
        for (size_t i = 0; i < vetor_strings_tamanho(array_indexation_vars); i++) {
            char* k_var = vetor_strings_obter(array_indexation_vars, i);
            char* current_temp_var = resolvedor_expressao_gerar_var_temp();

            snprintf(buffer, sizeof(buffer), "%s = %s", current_temp_var, k_var);
            lista_codigo_adicionar_string(lvalue_node->codigo, strdup(buffer));

            for (int j = 0; j < counter; ++j) {
                snprintf(buffer, sizeof(buffer), "%s = %s * %d", current_temp_var, current_temp_var, max_dims[j]);
                lista_codigo_adicionar_string(lvalue_node->codigo, strdup(buffer));
            }
            snprintf(buffer, sizeof(buffer), "%s = %s + %s", somador_temp_var, somador_temp_var, current_temp_var);
            lista_codigo_adicionar_string(lvalue_node->codigo, strdup(buffer));
            free(current_temp_var); // Free temp var generated by resolver
        }

        snprintf(buffer, sizeof(buffer), "%s[%s]", ident_node->token->lexema, somador_temp_var);
        lvalue_node->sdt_mat.no = no_arvore_expressao_criar_no('n', strdup(type_str), strdup(buffer));
        free(somador_temp_var);
    } else {
        lvalue_node->sdt_mat.no = no_arvore_expressao_criar_no('n', strdup(type_str), strdup(ident_node->token->lexema));
    }

    free(type_str); // Free the type string obtained from scope manager
    free(max_dims); // Free the dynamically allocated array dimensions
}

void EXPA_lexema_para_valor(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // FACTOR -> int_constant | float_constant | string_constant
    // Factor.Node = new Node(value)
    NoAST* factor_node = no_pai;
    NoAST* constant_node = no_pai->filhos[0];
    char* type_str = NULL;

    if (strcmp(constant_node->id, "NI") == 0) {
        type_str = strdup("int");
    } else if (strcmp(constant_node->id, "NPF") == 0) {
        type_str = strdup("float");
    } else if (strcmp(constant_node->id, "STRC") == 0) {
        type_str = strdup("string");
    }
    factor_node->sdt_mat.no = no_arvore_expressao_criar_no('n', type_str, strdup(constant_node->token->lexema));
}

void EXPA_definir_operacao(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // NUMEXPRESSIONAUX -> op NUMEXPRESSION
    NoAST* numexpr_aux_node = no_pai;
    NoAST* operation_node = no_pai->filhos[0];
    NoAST* numexpression_node = no_pai->filhos[1];

    numexpr_aux_node->sdt_mat.operacao = operation_node->token->lexema[0];
    numexpr_aux_node->sdt_mat.no = numexpression_node->sdt_mat.no; // Transfer ownership of Node*
    numexpr_aux_node->sdt_mat.contador_vetor = numexpression_node->sdt_mat.contador_vetor;

    // Splice code and result variable
    lista_codigo_adicionar_lista(numexpr_aux_node->codigo, numexpression_node->codigo);
    numexpr_aux_node->res_var_codigo = numexpression_node->res_var_codigo;
}

void EXPA_ident_para_cima(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // FACTOR -> LVALUE
    NoAST* factor_node = no_pai;
    NoAST* lvalue_node = no_pai->filhos[0];
    factor_node->sdt_mat.no = lvalue_node->sdt_mat.no; // Transfer ownership of Node*
    factor_node->sdt_mat.contador_vetor = lvalue_node->sdt_mat.contador_vetor;
    lista_codigo_adicionar_lista(factor_node->codigo, lvalue_node->codigo);
}

void EXPA_valor_para_cima(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // UNARYEXPR -> FACTOR
    NoAST* unaryexpr_node = no_pai;
    NoAST* factor_node = no_pai->filhos[0];
    unaryexpr_node->sdt_mat.contador_vetor = factor_node->sdt_mat.contador_vetor;
    unaryexpr_node->sdt_mat.no = factor_node->sdt_mat.no; // Transfer ownership of Node*
    unaryexpr_node->res_var_codigo = factor_node->res_var_codigo;
    lista_codigo_adicionar_lista(unaryexpr_node->codigo, factor_node->codigo);
}

void EXPA_valor_segundo_filho_para_cima(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // UNARYEXPR -> PLUS/MINUS FACTOR
    NoAST* unaryexpr_node = no_pai;
    NoAST* symbol_node = no_pai->filhos[0]; // PLUS or MINUS
    NoAST* factor_node = no_pai->filhos[1];

    unaryexpr_node->res_var_codigo.var = resolvedor_expressao_gerar_var_temp();

    char buffer[256];
    snprintf(buffer, sizeof(buffer), "%s = %c%s",
             unaryexpr_node->res_var_codigo.var,
             symbol_node->token->lexema[0],
             factor_node->sdt_mat.no->valor);
    lista_codigo_adicionar_string(unaryexpr_node->codigo, strdup(buffer));

    // Create a new node representing the unary operation
    unaryexpr_node->sdt_mat.no = no_arvore_expressao_criar_no(symbol_node->token->lexema[0], strdup("N"), factor_node->sdt_mat.no, NULL);
}

void EXPA_gerar_no(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // NUMEXPRESSION -> TERM NUMEXPRESSIONAUX
    NoAST* numexpression_node = no_pai;
    NoAST* term_node = no_pai->filhos[0];
    NoAST* numexpression_aux_node = no_pai->filhos[1];

    if (numexpression_aux_node->sdt_mat.no != NULL) {
        char* term_type = AUXILIAR_obter_tipo(term_node->sdt_mat.no->tipo, term_node->sdt_mat.contador_vetor);
        char* aux_type = AUXILIAR_obter_tipo(numexpression_aux_node->sdt_mat.no->tipo, numexpression_aux_node->sdt_mat.contador_vetor);

        // Type checking (simplified, you might want a more robust system)
        if (strcmp(term_type, aux_type) != 0 &&
            !( (strcmp(term_type, "int") == 0 && strcmp(aux_type, "float") == 0) ||
               (strcmp(term_type, "float") == 0 && strcmp(aux_type, "int") == 0) ) ) {
            reportar_erro_semantico("Incompatibilidade de tipos em operacao aritmetica/logica.");
        }
        free(term_type);
        free(aux_type);

        char* varA = (term_node->res_var_codigo.var != NULL && strlen(term_node->res_var_codigo.var) > 0) ? strdup(term_node->res_var_codigo.var) : strdup(term_node->sdt_mat.no->valor);
        char* varB = (numexpression_aux_node->res_var_codigo.var != NULL && strlen(numexpression_aux_node->res_var_codigo.var) > 0) ? strdup(numexpression_aux_node->res_var_codigo.var) : strdup(numexpression_aux_node->sdt_mat.no->valor);

        lista_codigo_adicionar_lista(numexpression_node->codigo, term_node->codigo);
        lista_codigo_adicionar_lista(numexpression_node->codigo, numexpression_aux_node->codigo);

        numexpression_node->res_var_codigo.var = resolvedor_expressao_gerar_var_temp();
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "%s = %s %c %s",
                 numexpression_node->res_var_codigo.var,
                 varA,
                 numexpression_aux_node->sdt_mat.operacao,
                 varB);
        lista_codigo_adicionar_string(numexpression_node->codigo, strdup(buffer));
        free(varA);
        free(varB);

        numexpression_node->sdt_mat.no = no_arvore_expressao_criar_no(
            numexpression_aux_node->sdt_mat.operacao,
            strdup("N"), // Type could be resolved more accurately here
            term_node->sdt_mat.no,
            numexpression_aux_node->sdt_mat.no
        );
    } else {
        numexpression_node->sdt_mat.no = term_node->sdt_mat.no;
        numexpression_node->sdt_mat.contador_vetor = term_node->sdt_mat.contador_vetor;
        numexpression_node->res_var_codigo = term_node->res_var_codigo;
        lista_codigo_adicionar_lista(numexpression_node->codigo, term_node->codigo);
    }
}

void EXPA_definir_operacao2(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // UNARYEXPRAUX -> * TERM | / TERM | % TERM
    NoAST* unaryexpr_aux_node = no_pai;
    NoAST* op_node = no_pai->filhos[0];
    NoAST* term_node = no_pai->filhos[1];

    unaryexpr_aux_node->sdt_mat.operacao = op_node->token->lexema[0];
    unaryexpr_aux_node->sdt_mat.no = term_node->sdt_mat.no; // Transfer ownership of Node*
    unaryexpr_aux_node->sdt_mat.contador_vetor = term_node->sdt_mat.contador_vetor;
    lista_codigo_adicionar_lista(unaryexpr_aux_node->codigo, term_node->codigo);
    unaryexpr_aux_node->res_var_codigo = term_node->res_var_codigo;
}

void EXPA_termo(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // TERM -> UNARYEXPR UNARYEXPRAUX
    NoAST* term_node = no_pai;
    NoAST* unaryexpr_node = no_pai->filhos[0];
    NoAST* unaryexpr_aux_node = no_pai->filhos[1];

    if (unaryexpr_aux_node->sdt_mat.no == NULL) { // Epsilon production for UNARYEXPRAUX
        term_node->sdt_mat.no = unaryexpr_node->sdt_mat.no;
        term_node->sdt_mat.contador_vetor = unaryexpr_node->sdt_mat.contador_vetor;
        lista_codigo_adicionar_lista(term_node->codigo, unaryexpr_node->codigo);

        term_node->res_var_codigo.var = resolvedor_expressao_gerar_var_temp();
        char buffer[256];
        char* varA_val = (unaryexpr_node->res_var_codigo.var != NULL && strlen(unaryexpr_node->res_var_codigo.var) > 0) ?
                         strdup(unaryexpr_node->res_var_codigo.var) : strdup(unaryexpr_node->sdt_mat.no->valor);

        // Handle array dereferencing if it's an array and not fully indexed
        if (unaryexpr_node->sdt_mat.contador_vetor > 0) {
            snprintf(buffer, sizeof(buffer), "%s = %s", term_node->res_var_codigo.var, varA_val);
        } else {
            snprintf(buffer, sizeof(buffer), "%s = %s", term_node->res_var_codigo.var, varA_val);
        }
        lista_codigo_adicionar_string(term_node->codigo, strdup(buffer));
        free(varA_val);

    } else {
        char* unary_type = AUXILIAR_obter_tipo(unaryexpr_node->sdt_mat.no->tipo, unaryexpr_node->sdt_mat.contador_vetor);
        char* aux_type = AUXILIAR_obter_tipo(unaryexpr_aux_node->sdt_mat.no->tipo, unaryexpr_aux_node->sdt_mat.contador_vetor);

        // Type checking
        if (strcmp(unary_type, aux_type) != 0 &&
            !( (strcmp(unary_type, "int") == 0 && strcmp(aux_type, "float") == 0) ||
               (strcmp(unary_type, "float") == 0 && strcmp(aux_type, "int") == 0) ) ) {
            reportar_erro_semantico("Incompatibilidade de tipos em operacao aritmetica/logica.");
        }
        free(unary_type);
        free(aux_type);

        char* varA = (unaryexpr_node->res_var_codigo.var != NULL && strlen(unaryexpr_node->res_var_codigo.var) > 0) ? strdup(unaryexpr_node->res_var_codigo.var) : strdup(unaryexpr_node->sdt_mat.no->valor);
        char* varB = (unaryexpr_aux_node->res_var_codigo.var != NULL && strlen(unaryexpr_aux_node->res_var_codigo.var) > 0) ? strdup(unaryexpr_aux_node->res_var_codigo.var) : strdup(unaryexpr_aux_node->sdt_mat.no->valor);

        lista_codigo_adicionar_lista(term_node->codigo, unaryexpr_node->codigo);
        lista_codigo_adicionar_lista(term_node->codigo, unaryexpr_aux_node->codigo);

        term_node->res_var_codigo.var = resolvedor_expressao_gerar_var_temp();
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "%s = %s %c %s",
                 term_node->res_var_codigo.var,
                 varA,
                 unaryexpr_aux_node->sdt_mat.operacao,
                 varB);
        lista_codigo_adicionar_string(term_node->codigo, strdup(buffer));
        free(varA);
        free(varB);

        term_node->sdt_mat.no = no_arvore_expressao_criar_no(
            unaryexpr_aux_node->sdt_mat.operacao,
            strdup("N"), // Type could be resolved here
            unaryexpr_node->sdt_mat.no,
            unaryexpr_aux_node->sdt_mat.no
        );
    }
}

void EXPA_imprimir_expressao0_h(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // EXPRESSION -> NUMEXPRESSION EXPRESSION'
    NoAST* numexpression_node = no_pai->filhos[0];
    NoAST* expression_linha_node = no_pai->filhos[1];
    expression_linha_node->res_var_codigo.var = strdup(numexpression_node->res_var_codigo.var);
}

void EXPA_imprimir_expressao0(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // EXPRESSION -> NUMEXPRESSION EXPRESSION'
    NoAST* expression_node = no_pai;
    NoAST* numexpression_node = no_pai->filhos[0];
    NoAST* expression_linha_node = no_pai->filhos[1];

    expression_node->res_var_codigo = numexpression_node->res_var_codigo; // Copy struct

    lista_codigo_adicionar_lista(expression_node->codigo, numexpression_node->codigo);
    lista_codigo_adicionar_lista(expression_node->codigo, expression_linha_node->codigo);

    // This calls a C function `resolvedor_expressao_imprimir_arvore_binaria`
    resolvedor_expressao_imprimir_arvore_binaria(numexpression_node->sdt_mat.no);
}

void EXPA_passar_numero(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // FACTOR -> OPEN_PARENTHESIS NUMEXPRESSION CLOSE_PARENTHESIS
    NoAST* factor_node = no_pai;
    NoAST* numexpression_node = no_pai->filhos[1];

    factor_node->res_var_codigo = numexpression_node->res_var_codigo; // Copy struct
    lista_codigo_adicionar_lista(factor_node->codigo, numexpression_node->codigo);
    factor_node->sdt_mat.no = numexpression_node->sdt_mat.no; // Transfer ownership
}

void EXPA_imprimir_expressao1(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // EXPRESSION' -> RELOP NUMEXPRESSION
    NoAST* numexpression_node = no_pai->filhos[1];
    resolvedor_expressao_imprimir_arvore_binaria(numexpression_node->sdt_mat.no);
}

void EXPA_imprimir_expressao2(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // ALLOCEXPRESSION' -> TYPE OPEN_BRACKET NUMEXPRESSION CLOSE_BRACKET ALLOCAUX
    NoAST* numexpression_node = no_pai->filhos[2];
    resolvedor_expressao_imprimir_arvore_binaria(numexpression_node->sdt_mat.no);
}

void EXPA_contador_vetor(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // ALLOCAUX -> [ NUMEXPRESSION ] ALLOCAUX1
    NoAST* allocaux_node = no_pai;
    NoAST* numexpression_node = no_pai->filhos[1];
    NoAST* allocaux1_node = no_pai->filhos[3];

    // Ensure vetor_array_var is initialized
    if (allocaux_node->sdt_mat.vetor_array_var == NULL) {
        allocaux_node->sdt_mat.vetor_array_var = vetor_strings_criar();
    }
    vetor_strings_adicionar(allocaux_node->sdt_mat.vetor_array_var, strdup(numexpression_node->res_var_codigo.var));
    vetor_strings_adicionar_vetor(allocaux_node->sdt_mat.vetor_array_var, allocaux1_node->sdt_mat.vetor_array_var); // Splice the vector

    allocaux_node->sdt_mat.contador_vetor = allocaux1_node->sdt_mat.contador_vetor + 1;

    lista_codigo_adicionar_lista(allocaux_node->codigo, numexpression_node->codigo);
    lista_codigo_adicionar_lista(allocaux_node->codigo, allocaux1_node->codigo);
}


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

#include "analisador_lexico.h"

// Função auxiliar para verificar se um caractere é espaço em branco
static int eh_espaco_branco(char c) {
    return c == ' ' || c == '\t' || c == '\r';
}

// Função para encontrar o maior token dentre os possíveis
static PossivelToken* encontrar_maior_token(AnalisadorLexico* analisador) {
    PossivelToken* resposta = NULL;
    unsigned int maior_tamanho = 0;
    
    for (int i = 0; i < analisador->num_possiveis_tokens; i++) {
        unsigned int tamanho_token = strlen(analisador->possiveis_tokens[i]->token->lexema);
        if (i == 0 || tamanho_token > maior_tamanho) {
            maior_tamanho = tamanho_token;
            resposta = analisador->possiveis_tokens[i];
        }
    }
    return resposta;
}

// Função para obter o ID do token baseado no lexema
static char* obter_id_token(AnalisadorLexico* analisador, const char* id, const char* lexema) {
    // Verificar se é identificador e palavra-chave
    if (strcmp(id, "IDENT") == 0) {
        for (int i = 0; i < analisador->num_palavras_chave; i++) {
            if (strcmp(lexema, analisador->palavras_chave[i]) == 0) {
                // Converter para maiúsculas
                char* novo_id = strdup(analisador->palavras_chave[i]);
                for (char* p = novo_id; *p; p++) *p = toupper(*p);
                return novo_id;
            }
        }
    }
    
    // Verificar se é operador matemático ou caractere único
    if (strcmp(id, "MATHOP") != 0 && strcmp(id, "ONECHAR") != 0) {
        return strdup(id);
    }
    
    // Procurar no mapeamento
    for (int i = 0; i < analisador->num_mapeamentos; i++) {
        if (strcmp(lexema, analisador->mapeamento_tokens[i].lexema) == 0) {
            return strdup(analisador->mapeamento_tokens[i].id);
        }
    }
    
    return strdup(id); // Fallback
}

AnalisadorLexico* criar_analisador_lexico(FILE* arquivo, char** palavras_chave, int num_palavras) {
    AnalisadorLexico* analisador = malloc(sizeof(AnalisadorLexico));
    analisador->arquivo = arquivo;
    
    // Copiar palavras-chave
    analisador->num_palavras_chave = num_palavras;
    analisador->palavras_chave = malloc(num_palavras * sizeof(char*));
    for (int i = 0; i < num_palavras; i++) {
        analisador->palavras_chave[i] = strdup(palavras_chave[i]);
    }
    
    // Inicializar identificadores de token
    analisador->num_identificadores = 7;
    analisador->identificadores_token = malloc(analisador->num_identificadores * sizeof(IdentificadorToken*));
    analisador->identificadores_token[0] = criar_identificador_ident();
    analisador->identificadores_token[1] = criar_identificador_inteiro();
    analisador->identificadores_token[2] = criar_identificador_float();
    analisador->identificadores_token[3] = criar_identificador_operador_matematico();
    analisador->identificadores_token[4] = criar_identificador_relop();
    analisador->identificadores_token[5] = criar_identificador_um_caractere();
    analisador->identificadores_token[6] = criar_identificador_string_const();
    
    // Inicializar mapeamento de tokens
    analisador->num_mapeamentos = 14;
    analisador->mapeamento_tokens = malloc(analisador->num_mapeamentos * sizeof(*analisador->mapeamento_tokens));
    
    // Operadores e delimitadores
    char* mapeamentos[][2] = {
        {"+", "PLUS"},
        {"-", "MINUS"},
        {"*", "MULTIPLICATION"},
        {"/", "DIVISION"},
        {"%", "MODULUS"},
        {"=", "ASSIGN"},
        {"(", "OPEN_PARENTHESIS"},
        {")", "CLOSE_PARENTHESIS"},
        {"{", "OPEN_BRACE"},
        {"}", "CLOSE_BRACE"},
        {"[", "OPEN_BRACKET"},
        {"]", "CLOSE_BRACKET"},
        {";", "SEMICOLON"},
        {",", "COMMA"}
    };
    
    for (int i = 0; i < analisador->num_mapeamentos; i++) {
        analisador->mapeamento_tokens[i].lexema = strdup(mapeamentos[i][0]);
        analisador->mapeamento_tokens[i].id = strdup(mapeamentos[i][1]);
    }
    
    // Inicializar contadores
    analisador->linha = 1;
    analisador->coluna = 0;
    analisador->linhas_durante_token = 0;
    analisador->lendo_token = 0;
    
    // Inicializar listas vazias
    analisador->possiveis_tokens = NULL;
    analisador->num_possiveis_tokens = 0;
    analisador->tokens = NULL;
    analisador->num_tokens = 0;
    
    return analisador;
}

Token* proximo_token(AnalisadorLexico* analisador) {
    if (analisador->arquivo) {
        while (!feof(analisador->arquivo)) {
            analisador->caractere_atual = fgetc(analisador->arquivo);
            int eof = feof(analisador->arquivo);
            
            if (eof && !analisador->lendo_token) break;
            
            analisador->coluna++;
            if (analisador->caractere_atual == '\n') {
                analisador->linha++;
                analisador->coluna = 0;
                if (analisador->lendo_token) analisador->linhas_durante_token++;
            }

            // Ignorar espaços em branco fora de tokens
            if (!eof && eh_espaco_branco(analisador->caractere_atual) && !analisador->lendo_token) {
                continue;
            }

            int em_execucao = 0;
            for (int i = 0; i < analisador->num_identificadores; i++) {
                IdentificadorToken* id = analisador->identificadores_token[i];
                
                if (identificador_token_sucedeu(id)) continue;
                
                int resultado = analisar_caractere(id, analisador->caractere_atual, eof);
                
                if (resultado == MAQ_EXECUTANDO) {
                    em_execucao++;
                    continue;
                }
                
                if (resultado == MAQ_SUCESSO) {
                    char* lexema = obter_lexema(id);
                    char* token_id = obter_id_token(analisador, obter_identificador(id), lexema);
                    
                    Token* token = criar_token(lexema, token_id, obter_linha_inicio(id), obter_coluna_inicio(id));
                    
                    // Adicionar aos tokens possíveis
                    analisador->num_possiveis_tokens++;
                    analisador->possiveis_tokens = realloc(analisador->possiveis_tokens, 
                        analisador->num_possiveis_tokens * sizeof(PossivelToken*));
                    
                    PossivelToken* pt = malloc(sizeof(PossivelToken));
                    pt->token = token;
                    pt->inicio = obter_posicao_inicio(id);
                    analisador->possiveis_tokens[analisador->num_possiveis_tokens - 1] = pt;
                }
            }

            analisador->lendo_token = em_execucao > 0;

            if (em_execucao > 0) continue;

            // Nenhum token válido encontrado
            if (analisador->num_possiveis_tokens == 0) {
                char erro_msg[100];
                snprintf(erro_msg, sizeof(erro_msg), "Erro começando em %d:%d", analisador->linha, analisador->coluna);
                erro_lexico(erro_msg);
            }

            // Selecionar o maior token
            PossivelToken* token_escolhido = encontrar_maior_token(analisador);
            Token* token = token_escolhido->token;
            
            // Adicionar à lista de tokens
            analisador->num_tokens++;
            analisador->tokens = realloc(analisador->tokens, 
                analisador->num_tokens * sizeof(Token*));
            analisador->tokens[analisador->num_tokens - 1] = token;

            // Atualizar posição
            analisador->coluna = token->coluna + strlen(token->lexema) - 1;
            if (analisador->linhas_durante_token) {
                analisador->linha -= analisador->linhas_durante_token;
                analisador->linhas_durante_token = 0;
            }

            // Reposicionar cursor
            int proxima_posicao = token_escolhido->inicio + strlen(token->lexema);
            fseek(analisador->arquivo, proxima_posicao, SEEK_SET);

            // Limpar tokens possíveis e resetar identificadores
            for (int i = 0; i < analisador->num_possiveis_tokens; i++) {
                free(analisador->possiveis_tokens[i]);
            }
            free(analisador->possiveis_tokens);
            analisador->possiveis_tokens = NULL;
            analisador->num_possiveis_tokens = 0;
            
            for (int i = 0; i < analisador->num_identificadores; i++) {
                resetar_identificador(analisador->identificadores_token[i]);
            }

            return token;
        }
    }
    return NULL;
}

void destruir_analisador_lexico(AnalisadorLexico* analisador) {
    if (!analisador) return;
    
    // Liberar palavras-chave
    for (int i = 0; i < analisador->num_palavras_chave; i++) {
        free(analisador->palavras_chave[i]);
    }
    free(analisador->palavras_chave);
    
    // Liberar identificadores de token
    for (int i = 0; i < analisador->num_identificadores; i++) {
        destruir_identificador(analisador->identificadores_token[i]);
    }
    free(analisador->identificadores_token);
    
    // Liberar mapeamento
    for (int i = 0; i < analisador->num_mapeamentos; i++) {
        free(analisador->mapeamento_tokens[i].lexema);
        free(analisador->mapeamento_tokens[i].id);
    }
    free(analisador->mapeamento_tokens);
    
    // Liberar tokens
    for (int i = 0; i < analisador->num_tokens; i++) {
        destruir_token(analisador->tokens[i]);
    }
    free(analisador->tokens);
    
    // Liberar tokens possíveis
    for (int i = 0; i < analisador->num_possiveis_tokens; i++) {
        free(analisador->possiveis_tokens[i]);
    }
    free(analisador->possiveis_tokens);
    
    free(analisador);
}

#ifndef ANALISADOR_LEXICO_H
#define ANALISADOR_LEXICO_H

#include "tokens.h"
#include "tabela_simbolos.h"
#include "identificadores_token.h"
#include "erros.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Estrutura para token possível
typedef struct {
    Token* token;
    int inicio;
} PossivelToken;

// Estrutura principal do analisador léxico
typedef struct {
    FILE* arquivo;
    char** palavras_chave;
    int num_palavras_chave;
    
    IdentificadorToken** identificadores_token;
    int num_identificadores;
    
    PossivelToken** possiveis_tokens;
    int num_possiveis_tokens;
    
    Token** tokens;
    int num_tokens;
    
    // Mapeamento de lexema para ID
    struct {
        char* lexema;
        char* id;
    } *mapeamento_tokens;
    int num_mapeamentos;
    
    char caractere_atual;
    int linha;
    int coluna;
    int linhas_durante_token;
    int lendo_token;
} AnalisadorLexico;

// Funções públicas
AnalisadorLexico* criar_analisador_lexico(FILE* arquivo, char** palavras_chave, int num_palavras);
Token* proximo_token(AnalisadorLexico* analisador);
void destruir_analisador_lexico(AnalisadorLexico* analisador);

#endif

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

#ifndef ERROS_H
#define ERROS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Tipos de erro
typedef enum {
    ERRO_LEXICO,
    ERRO_SINTATICO,
    ERRO_SEMANTICO
} TipoErro;

// Estrutura base para erros
typedef struct {
    TipoErro tipo;
    char mensagem[512];
} Erro;

// Função para criar erro léxico
Erro criar_erro_lexico(const char* mensagem) {
    Erro erro;
    erro.tipo = ERRO_LEXICO;
    snprintf(erro.mensagem, sizeof(erro.mensagem), "Erro Léxico: %s", mensagem);
    return erro;
}

// Função para criar erro sintático
Erro criar_erro_sintatico(const char* mensagem) {
    Erro erro;
    erro.tipo = ERRO_SINTATICO;
    snprintf(erro.mensagem, sizeof(erro.mensagem), "Erro Sintático: %s", mensagem);
    return erro;
}

// Função para criar erro semântico
Erro criar_erro_semantico(const char* mensagem) {
    Erro erro;
    erro.tipo = ERRO_SEMANTICO;
    snprintf(erro.mensagem, sizeof(erro.mensagem), "Erro Semântico: %s", mensagem);
    return erro;
}

// Macros para lançar erros
#define LANCAR_ERRO_LEXICO(msg) do { \
    Erro _erro = criar_erro_lexico(msg); \
    fprintf(stderr, "%s\n", _erro.mensagem); \
    exit(EXIT_FAILURE); \
} while(0)

#define LANCAR_ERRO_SINTATICO(msg) do { \
    Erro _erro = criar_erro_sintatico(msg); \
    fprintf(stderr, "%s\n", _erro.mensagem); \
    exit(EXIT_FAILURE); \
} while(0)

#define LANCAR_ERRO_SEMANTICO(msg) do { \
    Erro _erro = criar_erro_semantico(msg); \
    fprintf(stderr, "%s\n", _erro.mensagem); \
    exit(EXIT_FAILURE); \
} while(0)

// Funções para tratamento de erros
void tratar_erro(const Erro* erro) {
    fprintf(stderr, "%s\n", erro->mensagem);
    exit(EXIT_FAILURE);
}

void imprimir_erro(const Erro* erro) {
    const char* tipo_str = "";
    switch(erro->tipo) {
        case ERRO_LEXICO: tipo_str = "Léxico"; break;
        case ERRO_SINTATICO: tipo_str = "Sintático"; break;
        case ERRO_SEMANTICO: tipo_str = "Semântico"; break;
    }
    fprintf(stderr, "[ERRO %s] %s\n", tipo_str, erro->mensagem);
}

#endif

#include "gerenciador_escopo.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define CAPACIDADE_INICIAL_FILHOS 10

GerenciadorEscopo* criar_gerenciador_escopo() {
    GerenciadorEscopo* gerenciador = malloc(sizeof(GerenciadorEscopo));
    gerenciador->escopo_raiz = NULL;
    gerenciador->escopo_atual = NULL;
    return gerenciador;
}

void abrir_escopo(GerenciadorEscopo* gerenciador, const char* nome_escopo) {
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
    if (!gerenciador->escopo_raiz) {
        gerenciador->escopo_raiz = novo_escopo;
    } else if (gerenciador->escopo_atual) {
        // Adicionar como filho do escopo atual
        if (gerenciador->escopo_atual->num_escopos_filhos >= gerenciador->escopo_atual->capacidade_filhos) {
            // Redimensionar array se necessário
            int nova_capacidade = gerenciador->escopo_atual->capacidade_filhos * 2;
            gerenciador->escopo_atual->escopos_filhos = realloc(
                gerenciador->escopo_atual->escopos_filhos,
                nova_capacidade * sizeof(ItemEscopo*)
            );
            gerenciador->escopo_atual->capacidade_filhos = nova_capacidade;
        }
        gerenciador->escopo_atual->escopos_filhos[gerenciador->escopo_atual->num_escopos_filhos++] = novo_escopo;
    }
    
    gerenciador->escopo_atual = novo_escopo;
}

void fechar_escopo(GerenciadorEscopo* gerenciador) {
    if (gerenciador->escopo_atual && gerenciador->escopo_atual->escopo_ancestral) {
        gerenciador->escopo_atual = gerenciador->escopo_atual->escopo_ancestral;
    }
}

void adicionar_simbolo(GerenciadorEscopo* gerenciador, Token* token) {
    if (!gerenciador->escopo_atual) return;
    
    if (simbolo_existe(gerenciador->escopo_atual->tabela_simbolos, token->lexema)) {
        char erro_msg[100];
        snprintf(erro_msg, sizeof(erro_msg), "%s redeclarado em %d:%d",
                 token->lexema, token->linha, token->coluna);
        erro_semantico(erro_msg);
        return;
    }
    
    adicionar_simbolo_tabela(gerenciador->escopo_atual->tabela_simbolos, token);
}

void definir_tipo_simbolo(GerenciadorEscopo* gerenciador, Token* token, const char* tipo) {
    ItemEscopo* escopo = gerenciador->escopo_atual;
    
    while (escopo) {
        if (simbolo_existe(escopo->tabela_simbolos, token->lexema)) {
            definir_tipo_simbolo_tabela(escopo->tabela_simbolos, token, tipo);
            return;
        }
        escopo = escopo->escopo_ancestral;
    }
    
    char erro_msg[100];
    snprintf(erro_msg, sizeof(erro_msg), 
             "Impossível atribuir tipo a %s em %d:%d",
             token->lexema, token->linha, token->coluna);
    erro_semantico(erro_msg);
}

bool simbolo_declarado(GerenciadorEscopo* gerenciador, Token* token) {
    ItemEscopo* escopo = gerenciador->escopo_atual;
    
    while (escopo) {
        if (simbolo_existe(escopo->tabela_simbolos, token->lexema)) {
            return true;
        }
        escopo = escopo->escopo_ancestral;
    }
    return false;
}

bool simbolo_e_tipo(GerenciadorEscopo* gerenciador, Token* token, const char* tipo) {
    ItemEscopo* escopo = gerenciador->escopo_atual;
    
    while (escopo) {
        if (simbolo_existe(escopo->tabela_simbolos, token->lexema)) {
            return verificar_tipo_simbolo(escopo->tabela_simbolos, token, tipo);
        }
        escopo = escopo->escopo_ancestral;
    }
    return false;
}

char* obter_tipo_simbolo(GerenciadorEscopo* gerenciador, Token* token) {
    ItemEscopo* escopo = gerenciador->escopo_atual;
    
    while (escopo) {
        if (simbolo_existe(escopo->tabela_simbolos, token->lexema)) {
            return obter_tipo_simbolo_tabela(escopo->tabela_simbolos, token);
        }
        escopo = escopo->escopo_ancestral;
    }
    return "";
}

bool esta_no_escopo(GerenciadorEscopo* gerenciador, const char* tipo_escopo) {
    ItemEscopo* escopo = gerenciador->escopo_atual;
    
    while (escopo) {
        if (strcmp(escopo->nome, tipo_escopo) == 0) {
            return true;
        }
        escopo = escopo->escopo_ancestral;
    }
    return false;
}

void imprimir_tabelas(GerenciadorEscopo* gerenciador, FILE* saida) {
    if (!gerenciador->escopo_raiz) return;
    
    // Pilha para percurso em profundidade
    ItemEscopo** pilha = malloc(100 * sizeof(ItemEscopo*));
    int topo = -1;
    pilha[++topo] = gerenciador->escopo_raiz;
    
    while (topo >= 0) {
        ItemEscopo* atual = pilha[topo--];
        
        fprintf(saida, "Tabela de Símbolos");
        if (strlen(atual->nome) > 0) {
            fprintf(saida, " tipo: %s", atual->nome);
        }
        fprintf(saida, "\n");
        imprimir_tabela_simbolos(atual->tabela_simbolos, saida);
        
        // Empilhar filhos
        for (int i = atual->num_escopos_filhos - 1; i >= 0; i--) {
            if (topo < 99) {
                pilha[++topo] = atual->escopos_filhos[i];
            }
        }
    }
    
    free(pilha);
}

void liberar_gerenciador_escopo(GerenciadorEscopo* gerenciador) {
    if (!gerenciador) return;
    
    // Liberar escopos recursivamente
    ItemEscopo** pilha = malloc(100 * sizeof(ItemEscopo*));
    int topo = -1;
    pilha[++topo] = gerenciador->escopo_raiz;
    
    while (topo >= 0) {
        ItemEscopo* atual = pilha[topo--];
        
        // Empilhar filhos
        for (int i = 0; i < atual->num_escopos_filhos; i++) {
            if (topo < 99) {
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

#ifndef GERENCIADOR_ESCOPO_H
#define GERENCIADOR_ESCOPO_H

#include "tabela_simbolos.h" // Antigo symbols_table.h
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
void abrir_escopo(GerenciadorEscopo* gerenciador, const char* nome_escopo);

// Fecha o escopo atual
void fechar_escopo(GerenciadorEscopo* gerenciador);

// Adiciona um novo símbolo ao escopo atual
void adicionar_simbolo(GerenciadorEscopo* gerenciador, Token* token);

// Define o tipo de um símbolo
void definir_tipo_simbolo(GerenciadorEscopo* gerenciador, Token* token, const char* tipo);

// Verifica se um símbolo está declarado
bool simbolo_declarado(GerenciadorEscopo* gerenciador, Token* token);

// Verifica se um símbolo é de um tipo específico
bool simbolo_e_tipo(GerenciadorEscopo* gerenciador, Token* token, const char* tipo);

// Obtém o tipo de um símbolo
char* obter_tipo_simbolo(GerenciadorEscopo* gerenciador, Token* token);

// Verifica se está em um tipo específico de escopo
bool esta_no_escopo(GerenciadorEscopo* gerenciador, const char* tipo_escopo);

// Imprime todas as tabelas de símbolos
void imprimir_tabelas(GerenciadorEscopo* gerenciador, FILE* saida);

// Libera a memória do gerenciador de escopos
void liberar_gerenciador_escopo(GerenciadorEscopo* gerenciador);

#endif

#include "identificadores_tokens.h"
#include <ctype.h> 
#include <string.h>
#include <stdlib.h>
#include "token.h"

// =================================================================
// Funções auxiliares
// =================================================================

static int eh_letra(char c) {
    return isalpha((unsigned char)c) || c == '_';
}

static int eh_digito(char c) {
    return isdigit((unsigned char)c);
}

// =================================================================
// Implementações das funções de transição para cada identificador
// =================================================================

// Identificador de identificadores (ex: nomes de variáveis)
static const char* estado_ident_0(char c, int eof) {
    if (eof) return "morto";
    if (eh_letra(c)) return "1";
    return "morto";
}

static const char* estado_ident_1(char c, int eof) {
    if (eof) return "final";
    if (eh_letra(c) || eh_digito(c) || c == '_') return "1";
    return "final";
}

// Identificador de números inteiros (NI)
static const char* estado_inteiro_0(char c, int eof) {
    if (eof) return "morto";
    if (eh_digito(c)) return "1";
    return "morto";
}

static const char* estado_inteiro_1(char c, int eof) {
    if (eof) return "final";
    if (eh_digito(c)) return "1";
    if (c == 'e') return "2";
    return "final";
}

static const char* estado_inteiro_2(char c, int eof) {
    if (eof) return "morto";
    if (eh_digito(c)) return "3";
    if (c == '+' || c == '-') return "4";
    return "morto";
}

static const char* estado_inteiro_3(char c, int eof) {
    if (eof) return "final";
    if (eh_digito(c)) return "3";
    return "final";
}

static const char* estado_inteiro_4(char c, int eof) {
    if (eof) return "morto";
    if (eh_digito(c)) return "3";
    return "morto";
}

// Identificador de números de ponto flutuante (NPF)
static const char* estado_float_0(char c, int eof) {
    if (eof) return "morto";
    if (c == '.') return "1";
    if (eh_digito(c)) return "6";
    return "morto";
}

static const char* estado_float_1(char c, int eof) {
    if (eof) return "morto";
    if (eh_digito(c)) return "2";
    return "morto";
}

static const char* estado_float_2(char c, int eof) {
    if (eof) return "final";
    if (eh_digito(c)) return "2";
    if (c == 'e') return "3";
    return "final";
}

static const char* estado_float_3(char c, int eof) {
    if (eof) return "morto";
    if (eh_digito(c)) return "4";
    if (c == '+' || c == '-') return "5";
    return "morto";
}

static const char* estado_float_4(char c, int eof) {
    if (eof) return "final";
    if (eh_digito(c)) return "4";
    return "final";
}

static const char* estado_float_5(char c, int eof) {
    if (eof) return "morto";
    if (eh_digito(c)) return "4";
    return "morto";
}

static const char* estado_float_6(char c, int eof) {
    if (eof) return "morto";
    if (eh_digito(c)) return "6";
    if (c == '.') return "2";
    if (c == 'e') return "7";
    return "morto";
}

static const char* estado_float_7(char c, int eof) {
    if (eof) return "morto";
    if (c == '+' || c == '-') return "5";
    if (eh_digito(c)) return "4";
    return "morto";
}

// Identificador de operadores matemáticos (MATHOP)
static const char* estado_mathop_0(char c, int eof) {
    if (eof) return "morto";
    if (c == '+' || c == '-' || c == '*' || c == '/' || c == '%' || c == '=') return "final";
    return "morto";
}

// Identificador de caracteres únicos (ONECHAR)
static const char* estado_onechar_0(char c, int eof) {
    if (eof) return "morto";
    if (c == '(' || c == ')' || c == '{' || c == '}' || 
        c == '[' || c == ']' || c == ';' || c == ',') return "final";
    return "morto";
}

// Identificador de constantes de string (STRC)
static const char* estado_strc_0(char c, int eof) {
    if (eof) return "morto";
    if (c == '"') return "1";
    return "morto";
}

static const char* estado_strc_1(char c, int eof) {
    if (eof) return "morto";
    if (c != '"') return "1";
    return "final";
}

// Identificador de operadores relacionais (RELOP)
static const char* estado_relop_0(char c, int eof) {
    if (eof) return "morto";
    if (c == '>' || c == '<') return "1";
    if (c == '=' || c == '!') return "2";
    return "morto";
}

static const char* estado_relop_1(char c, int eof) {
    if (eof) return "final_1";
    if (c == '=') return "final_2";
    return "final_1";
}

static const char* estado_relop_2(char c, int eof) {
    if (eof) return "morto";
    if (c == '=') return "final_2";
    return "morto";
}

// =================================================================
// Construção das máquinas de estado finito (FSM) para cada token
// =================================================================

// FSM para identificadores
static TransicaoEstado transicoes_ident[] = {
    {"0", estado_ident_0},
    {"1", estado_ident_1}
};

static const char* estados_finais_ident[] = {"final"};
static const char* estados_mortos_ident[] = {"morto"};

static MaquinaEstado maquina_ident = {
    transicoes_ident,
    sizeof(transicoes_ident) / sizeof(TransicaoEstado),
    "0",
    estados_finais_ident,
    sizeof(estados_finais_ident) / sizeof(const char*),
    estados_mortos_ident,
    sizeof(estados_mortos_ident) / sizeof(const char*)
};

// FSM para números inteiros (NI)
static TransicaoEstado transicoes_inteiro[] = {
    {"0", estado_inteiro_0},
    {"1", estado_inteiro_1},
    {"2", estado_inteiro_2},
    {"3", estado_inteiro_3},
    {"4", estado_inteiro_4}
};

static const char* estados_finais_inteiro[] = {"final"};
static const char* estados_mortos_inteiro[] = {"morto"};

static MaquinaEstado maquina_inteiro = {
    transicoes_inteiro,
    sizeof(transicoes_inteiro) / sizeof(TransicaoEstado),
    "0",
    estados_finais_inteiro,
    sizeof(estados_finais_inteiro) / sizeof(const char*),
    estados_mortos_inteiro,
    sizeof(estados_mortos_inteiro) / sizeof(const char*)
};

// FSM para números de ponto flutuante (NPF)
static TransicaoEstado transicoes_float[] = {
    {"0", estado_float_0},
    {"1", estado_float_1},
    {"2", estado_float_2},
    {"3", estado_float_3},
    {"4", estado_float_4},
    {"5", estado_float_5},
    {"6", estado_float_6},
    {"7", estado_float_7}
};

static const char* estados_finais_float[] = {"final"};
static const char* estados_mortos_float[] = {"morto"};

static MaquinaEstado maquina_float = {
    transicoes_float,
    sizeof(transicoes_float) / sizeof(TransicaoEstado),
    "0",
    estados_finais_float,
    sizeof(estados_finais_float) / sizeof(const char*),
    estados_mortos_float,
    sizeof(estados_mortos_float) / sizeof(const char*)
};

// FSM para operadores matemáticos (MATHOP)
static TransicaoEstado transicoes_mathop[] = {
    {"0", estado_mathop_0}
};

static const char* estados_finais_mathop[] = {"final"};
static const char* estados_mortos_mathop[] = {"morto"};

static MaquinaEstado maquina_mathop = {
    transicoes_mathop,
    sizeof(transicoes_mathop) / sizeof(TransicaoEstado),
    "0",
    estados_finais_mathop,
    sizeof(estados_finais_mathop) / sizeof(const char*),
    estados_mortos_mathop,
    sizeof(estados_mortos_mathop) / sizeof(const char*)
};

// FSM para caracteres únicos (ONECHAR)
static TransicaoEstado transicoes_onechar[] = {
    {"0", estado_onechar_0}
};

static const char* estados_finais_onechar[] = {"final"};
static const char* estados_mortos_onechar[] = {"morto"};

static MaquinaEstado maquina_onechar = {
    transicoes_onechar,
    sizeof(transicoes_onechar) / sizeof(TransicaoEstado),
    "0",
    estados_finais_onechar,
    sizeof(estados_finais_onechar) / sizeof(const char*),
    estados_mortos_onechar,
    sizeof(estados_mortos_onechar) / sizeof(const char*)
};

// FSM para constantes de string (STRC)
static TransicaoEstado transicoes_strc[] = {
    {"0", estado_strc_0},
    {"1", estado_strc_1}
};

static const char* estados_finais_strc[] = {"final"};
static const char* estados_mortos_strc[] = {"morto"};

static MaquinaEstado maquina_strc = {
    transicoes_strc,
    sizeof(transicoes_strc) / sizeof(TransicaoEstado),
    "0",
    estados_finais_strc,
    sizeof(estados_finais_strc) / sizeof(const char*),
    estados_mortos_strc,
    sizeof(estados_mortos_strc) / sizeof(const char*)
};

// FSM para operadores relacionais (RELOP)
static TransicaoEstado transicoes_relop[] = {
    {"0", estado_relop_0},
    {"1", estado_relop_1},
    {"2", estado_relop_2}
};

static const char* estados_finais_relop[] = {"final_1", "final_2"};
static const char* estados_mortos_relop[] = {"morto"};

static MaquinaEstado maquina_relop = {
    transicoes_relop,
    sizeof(transicoes_relop) / sizeof(TransicaoEstado),
    "0",
    estados_finais_relop,
    sizeof(estados_finais_relop) / sizeof(const char*),
    estados_mortos_relop,
    sizeof(estados_mortos_relop) / sizeof(const char*)
};

// =================================================================
// Inicialização dos identificadores de token
// =================================================================

IdentificadorToken identTokenIdentifier = {
    "IDENT",
    &maquina_ident
};

IdentificadorToken integerTokenIdentifier = {
    "NI",
    &maquina_inteiro
};

IdentificadorToken floatTokenIdentifier = {
    "NPF",
    &maquina_float
};

IdentificadorToken mathOperatorTokenIdentifier = {
    "MATHOP",
    &maquina_mathop
};

IdentificadorToken oneCharTokenIdentifier = {
    "ONECHAR",
    &maquina_onechar
};

IdentificadorToken stringConstCharTokenIdentifier = {
    "STRC",
    &maquina_strc
};

IdentificadorToken relopTokenIdentifier = {
    "RELOP",
    &maquina_relop
};

#ifndef IDENTIFICADORES_TOKENS_H
#define IDENTIFICADORES_TOKENS_H

#include "token.h"

// Tipo de função de transição de estado
typedef const char* (*FuncaoTransicao)(char c, int eof);

// Estrutura para uma transição de estado
typedef struct {
    const char* nome_estado;
    FuncaoTransicao funcao_transicao;
} TransicaoEstado;

// Máquina de estado finito
typedef struct {
    TransicaoEstado* transicoes;
    int num_transicoes;
    const char* estado_inicial;
    const char** estados_finais;
    int num_estados_finais;
    const char** estados_mortos;
    int num_estados_mortos;
} MaquinaEstado;

// Identificador de token
typedef struct {
    const char* id_token;
    MaquinaEstado* maquina_estado;
} IdentificadorToken;

// Declaração das funções de inicialização
void inicializar_identificadores();

// Declarações dos identificadores de token
extern IdentificadorToken identTokenIdentifier;
extern IdentificadorToken integerTokenIdentifier;
extern IdentificadorToken floatTokenIdentifier;
extern IdentificadorToken mathOperatorTokenIdentifier;
extern IdentificadorToken relopTokenIdentifier;
extern IdentificadorToken oneCharTokenIdentifier;
extern IdentificadorToken stringConstCharTokenIdentifier;

#endif // IDENTIFICADORES_TOKENS_H

#include "item_pilha_analise.h"
#include <stdlib.h>
#include <string.h>

ItemPilha* criar_item_simbolo(const char* simbolo) {
    ItemPilha* item = (ItemPilha*)malloc(sizeof(ItemPilha));
    item->tipo = TIPO_SIMBOLO;
    item->simbolo = strdup(simbolo);
    item->acao_semantica = NULL;
    item->no_ast = NULL;
    item->ancestral = NULL;
    return item;
}

ItemPilha* criar_item_acao(FuncaoAcaoSemantica acao) {
    ItemPilha* item = (ItemPilha*)malloc(sizeof(ItemPilha));
    item->tipo = TIPO_ACAO;
    item->simbolo = NULL;
    item->acao_semantica = acao;
    item->no_ast = NULL;
    item->ancestral = NULL;
    return item;
}

void definir_ancestralidade(ItemPilha* item, NoAST* no, NoAST* ancestral) {
    item->no_ast = no;
    item->ancestral = ancestral;
}

void liberar_item_pilha(ItemPilha* item) {
    if (item) {
        if (item->simbolo) free(item->simbolo);
        free(item);
    }
}

#ifndef ITEM_PILHA_ANALISE_H
#define ITEM_PILHA_ANALISE_H

#include "no_ast.h"            // Antigo ast_node.h
#include "gerenciador_escopo.h" // Antigo scope_manager.h

typedef enum {
    TIPO_SIMBOLO,
    TIPO_ACAO
} TipoItemPilha;

typedef void (*FuncaoAcaoSemantica)(NoAST* no_pai, GerenciadorEscopo* gerenciador_escopo);

typedef struct ItemPilha {
    TipoItemPilha tipo;
    char* simbolo; // Para itens do tipo TIPO_SIMBOLO
    FuncaoAcaoSemantica acao_semantica; // Para itens do tipo TIPO_ACAO
    NoAST* no_ast;
    NoAST* ancestral;
} ItemPilha;

// Cria um item de pilha do tipo símbolo
ItemPilha* criar_item_simbolo(const char* simbolo);

// Cria um item de pilha do tipo ação semântica
ItemPilha* criar_item_acao(FuncaoAcaoSemantica acao);

// Define a relação de ancestralidade na AST
void definir_ancestralidade(ItemPilha* item, NoAST* no, NoAST* ancestral);

// Libera a memória de um item da pilha
void liberar_item_pilha(ItemPilha* item);

#endif

#include "maquina_estados.h"
#include <stdlib.h>
#include <string.h>

MaquinaEstados* criar_maquina_estados(EntradaTransicao* transicoes, int num_trans,
                                      const char* estado_inicial, 
                                      char** estados_finais,
                                      char** estados_retrocesso) {
    MaquinaEstados* maq = (MaquinaEstados*)malloc(sizeof(MaquinaEstados));
    
    maq->transicoes = (EntradaTransicao*)malloc(sizeof(EntradaTransicao) * num_trans);
    for (int i = 0; i < num_trans; i++) {
        maq->transicoes[i].estado = strdup(transicoes[i].estado);
        maq->transicoes[i].funcao_transicao = transicoes[i].funcao_transicao;
    }
    maq->num_transicoes = num_trans;
    
    maq->estado_inicial = strdup(estado_inicial);
    
    // Contar estados finais
    int num_finais = 0;
    while (estados_finais[num_finais]) num_finais++;
    maq->estados_finais = (char**)malloc(sizeof(char*) * (num_finais + 1));
    for (int i = 0; i <= num_finais; i++) {
        maq->estados_finais[i] = estados_finais[i] ? strdup(estados_finais[i]) : NULL;
    }
    
    // Contar estados de retrocesso
    int num_retrocesso = 0;
    while (estados_retrocesso && estados_retrocesso[num_retrocesso]) num_retrocesso++;
    maq->estados_retrocesso = (char**)malloc(sizeof(char*) * (num_retrocesso + 1));
    for (int i = 0; i <= num_retrocesso; i++) {
        maq->estados_retrocesso[i] = estados_retrocesso && estados_retrocesso[i] ? 
            strdup(estados_retrocesso[i]) : NULL;
    }
    
    reiniciar_maquina(maq);
    return maq;
}

void reiniciar_maquina(MaquinaEstados* maq) {
    maq->estado_atual = strdup(maq->estado_inicial);
    
    if (maq->lexema) {
        free(maq->lexema);
    }
    maq->lexema = strdup("");
    
    maq->status = MAQ_OCIOSA;
}

EstadoMaquina transicionar_maquina(MaquinaEstados* maq, char c, int is_eof) {
    if (maq->status == MAQ_ERRO || maq->status == MAQ_SUCESSO) {
        return maq->status;
    }
    
    // Encontrar função de transição para o estado atual
    char* (*funcao_transicao)(char, int) = NULL;
    for (int i = 0; i < maq->num_transicoes; i++) {
        if (strcmp(maq->transicoes[i].estado, maq->estado_atual) == 0) {
            funcao_transicao = maq->transicoes[i].funcao_transicao;
            break;
        }
    }
    
    if (!funcao_transicao) {
        maq->status = MAQ_ERRO;
        return maq->status;
    }
    
    // Executar transição
    char* proximo_estado = funcao_transicao(c, is_eof);
    free(maq->estado_atual);
    maq->estado_atual = strdup(proximo_estado);
    maq->status = MAQ_EXECUTANDO;
    
    // Verificar estado terminal
    if (strcmp(proximo_estado, "dead") == 0) {
        maq->status = MAQ_ERRO;
        return maq->status;
    }
    
    // Verificar estado final
    for (int i = 0; maq->estados_finais[i]; i++) {
        if (strcmp(proximo_estado, maq->estados_finais[i]) == 0) {
            maq->status = MAQ_SUCESSO;
            break;
        }
    }
    
    // Atualizar lexema se não precisar retroceder
    if (!deve_retroceder_cursor(maq)) {
        char novo_char[2] = {c, '\0'};
        char* novo_lexema = (char*)malloc(strlen(maq->lexema) + 2);
        strcpy(novo_lexema, maq->lexema);
        strcat(novo_lexema, novo_char);
        free(maq->lexema);
        maq->lexema = novo_lexema;
    }
    
    return maq->status;
}

int deve_retroceder_cursor(MaquinaEstados* maq) {
    if (maq->status != MAQ_SUCESSO) return 0;
    
    // Verificar se é estado final
    int eh_estado_final = 0;
    for (int i = 0; maq->estados_finais[i]; i++) {
        if (strcmp(maq->estado_atual, maq->estados_finais[i]) == 0) {
            eh_estado_final = 1;
            break;
        }
    }
    if (!eh_estado_final) return 0;
    
    // Verificar se é estado de retrocesso
    for (int i = 0; maq->estados_retrocesso && maq->estados_retrocesso[i]; i++) {
        if (strcmp(maq->estado_atual, maq->estados_retrocesso[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

void liberar_maquina_estados(MaquinaEstados* maq) {
    if (!maq) return;
    
    // Liberar transições
    for (int i = 0; i < maq->num_transicoes; i++) {
        free(maq->transicoes[i].estado);
    }
    free(maq->transicoes);
    
    // Liberar estados
    free(maq->estado_inicial);
    free(maq->estado_atual);
    free(maq->lexema);
    
    // Liberar estados finais
    for (int i = 0; maq->estados_finais[i]; i++) {
        free(maq->estados_finais[i]);
    }
    free(maq->estados_finais);
    
    // Liberar estados de retrocesso
    if (maq->estados_retrocesso) {
        for (int i = 0; maq->estados_retrocesso[i]; i++) {
            free(maq->estados_retrocesso[i]);
        }
        free(maq->estados_retrocesso);
    }
    
    free(maq);
}

EstadoMaquina obter_status_maquina(MaquinaEstados* maq) {
    return maq->status;
}

const char* obter_lexema_maquina(MaquinaEstados* maq) {
    return maq->lexema;
}

#ifndef MAQUINA_ESTADOS_H
#define MAQUINA_ESTADOS_H

#include <stdlib.h>
#include <string.h>

// Estados da máquina
typedef enum {
    MAQ_OCIOSA,
    MAQ_EXECUTANDO,
    MAQ_SUCESSO,
    MAQ_ERRO
} EstadoMaquina;

// Estrutura para entrada de transição
typedef struct {
    char* estado;
    char* (*funcao_transicao)(char c, int is_eof);
} EntradaTransicao;

// Estrutura principal da máquina de estados
typedef struct {
    EntradaTransicao* transicoes;
    int num_transicoes;
    
    char* estado_inicial;
    char** estados_finais;
    char** estados_retrocesso;
    
    char* estado_atual;
    char* lexema;
    EstadoMaquina status;
} MaquinaEstados;

// Funções públicas
MaquinaEstados* criar_maquina_estados(EntradaTransicao* transicoes, int num_trans,
                                      const char* estado_inicial, 
                                      char** estados_finais,
                                      char** estados_retrocesso);

void reiniciar_maquina(MaquinaEstados* maq);
EstadoMaquina transicionar_maquina(MaquinaEstados* maq, char c, int is_eof);
int deve_retroceder_cursor(MaquinaEstados* maq);
void liberar_maquina_estados(MaquinaEstados* maq);

// Funções auxiliares
EstadoMaquina obter_status_maquina(MaquinaEstados* maq);
const char* obter_lexema_maquina(MaquinaEstados* maq);

#endif

#include "no_ast.h"
#include "erros.h"

// Implementação das funções

NoAST* criar_no_ast(const char* identificador, int* contador_rotulos) {
    NoAST* no = (NoAST*)malloc(sizeof(NoAST));
    no->identificador = strdup(identificador);
    no->token = NULL;
    
    no->filhos = NULL;
    no->quantidade_filhos = 0;
    no->capacidade_filhos = 0;
    
    no->codigo = criar_lista_string();
    no->proximo = NULL;
    no->proximo_para_for = NULL;
    
    no->decSDT.tipo_inicial = NULL;
    no->decSDT.tipo = NULL;
    
    no->codigo_res_var.variavel = NULL;
    no->codigo_res_var.inicio = NULL;
    no->codigo_res_var.retorno = NULL;
    no->codigo_res_var.valor_retorno = strdup("0");
    no->codigo_res_var.contador_parametros = 0;
    
    no->mathSDT.contador_arrays = 0;
    no->mathSDT.variaveis_array = criar_lista_string();
    no->mathSDT.operacao = '\0';
    no->mathSDT.no = NULL;
    
    no->contador_rotulos = contador_rotulos;
    
    return no;
}

void adicionar_filho(NoAST* self, NoAST* filho) {
    if (self->quantidade_filhos == self->capacidade_filhos) {
        int nova_capacidade = self->capacidade_filhos == 0 ? 1 : self->capacidade_filhos * 2;
        NoAST** novos_filhos = (NoAST**)realloc(self->filhos, sizeof(NoAST*) * nova_capacidade);
        if (!novos_filhos) {
            fprintf(stderr, "Erro de alocação de memória\n");
            exit(EXIT_FAILURE);
        }
        self->filhos = novos_filhos;
        self->capacidade_filhos = nova_capacidade;
    }
    self->filhos[self->quantidade_filhos++] = filho;
}

void definir_filhos(NoAST* self, NoAST** novos_filhos, int quantidade) {
    // Liberar filhos antigos se existirem
    if (self->filhos) {
        free(self->filhos);
    }
    
    self->filhos = novos_filhos;
    self->quantidade_filhos = quantidade;
    self->capacidade_filhos = quantidade;
}

void definir_token(NoAST* self, Token* token) {
    self->token = token;
}

char* novo_rotulo(NoAST* self) {
    (*self->contador_rotulos)++;
    char* rotulo = (char*)malloc(20 * sizeof(char));
    snprintf(rotulo, 20, "L%d", *self->contador_rotulos);
    return rotulo;
}

NoExpressao* criar_no_expressao_unario(char operacao, const char* valor, NoExpressao* filhoA) {
    NoExpressao* no = (NoExpressao*)malloc(sizeof(NoExpressao));
    no->operacao = operacao;
    no->valor = strdup(valor);
    no->filhoA = filhoA;
    no->filhoB = NULL;
    no->tipo = filhoA->tipo; // Compartilha o mesmo tipo
    
    return no;
}

NoExpressao* criar_no_expressao_binario(char operacao, const char* valor, NoExpressao* filhoA, NoExpressao* filhoB) {
    if (strcmp(filhoA->tipo, filhoB->tipo) == 0) {
        NoExpressao* no = (NoExpressao*)malloc(sizeof(NoExpressao));
        no->operacao = operacao;
        no->valor = strdup(valor);
        no->filhoA = filhoA;
        no->filhoB = filhoB;
        no->tipo = strdup(filhoA->tipo); // Duplica o tipo
        
        return no;
    } else {
        imprimir_erro_tipo(operacao, filhoA, filhoB);
        erro_semantico("Erro de tipo na construção da árvore de expressão");
        return NULL;
    }
}

NoExpressao* criar_no_expressao_simples(char operacao, const char* tipo, const char* valor) {
    NoExpressao* no = (NoExpressao*)malloc(sizeof(NoExpressao));
    no->operacao = operacao;
    no->tipo = strdup(tipo);
    no->valor = strdup(valor);
    no->filhoA = NULL;
    no->filhoB = NULL;
    return no;
}

NoExpressao* criar_no_expressao_basico(char operacao, const char* valor) {
    NoExpressao* no = (NoExpressao*)malloc(sizeof(NoExpressao));
    no->operacao = operacao;
    no->valor = strdup(valor);
    no->tipo = NULL;
    no->filhoA = NULL;
    no->filhoB = NULL;
    return no;
}

ListaString* criar_lista_string() {
    ListaString* lista = (ListaString*)malloc(sizeof(ListaString));
    lista->itens = NULL;
    lista->tamanho = 0;
    lista->capacidade = 0;
    return lista;
}

void adicionar_string(ListaString* lista, const char* str) {
    if (lista->tamanho == lista->capacidade) {
        int nova_capacidade = lista->capacidade == 0 ? 4 : lista->capacidade * 2;
        char** novos_itens = (char**)realloc(lista->itens, sizeof(char*) * nova_capacidade);
        if (!novos_itens) {
            fprintf(stderr, "Erro de alocação de memória\n");
            exit(EXIT_FAILURE);
        }
        lista->itens = novos_itens;
        lista->capacidade = nova_capacidade;
    }
    lista->itens[lista->tamanho++] = strdup(str);
}

// Funções de liberação de memória
void liberar_lista_string(ListaString* lista) {
    if (lista) {
        for (int i = 0; i < lista->tamanho; i++) {
            free(lista->itens[i]);
        }
        free(lista->itens);
        free(lista);
    }
}

void liberar_no_expressao(NoExpressao* no) {
    if (no) {
        free(no->tipo);
        free(no->valor);
        liberar_no_expressao(no->filhoA);
        liberar_no_expressao(no->filhoB);
        free(no);
    }
}

void liberar_no_ast(NoAST* no) {
    if (no) {
        free(no->identificador);
        
        // Liberar filhos
        for (int i = 0; i < no->quantidade_filhos; i++) {
            liberar_no_ast(no->filhos[i]);
        }
        free(no->filhos);
        
        // Liberar SDTs
        free(no->decSDT.tipo_inicial);
        free(no->decSDT.tipo);
        
        free(no->codigo_res_var.variavel);
        free(no->codigo_res_var.inicio);
        free(no->codigo_res_var.retorno);
        free(no->codigo_res_var.valor_retorno);
        
        liberar_no_expressao(no->mathSDT.no);
        liberar_lista_string(no->mathSDT.variaveis_array);
        liberar_lista_string(no->codigo);
        
        free(no->proximo);
        free(no->proximo_para_for);
        
        free(no);
    }
}

void imprimir_erro_tipo(char operacao, NoExpressao* filhoA, NoExpressao* filhoB) {
    fprintf(stderr, "====================================================\n");
    fprintf(stderr, "Erro: operação '%c' entre tipos incompatíveis\n", operacao);
    fprintf(stderr, "Tipo do primeiro operando: %s\n", filhoA->tipo);
    fprintf(stderr, "Tipo do segundo operando: %s\n", filhoB->tipo);
}

#ifndef NO_AST_H
#define NO_AST_H

#include "tokens.h"
#include "tipo_enum.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Estrutura para nó de expressão
typedef struct NoExpressao {
    char operacao;
    char* tipo;
    char* valor;
    struct NoExpressao* filhoA;
    struct NoExpressao* filhoB;
} NoExpressao;

// Estrutura para lista de strings
typedef struct ListaString {
    char** itens;
    int tamanho;
    int capacidade;
} ListaString;

// Estrutura para SDT matemática
typedef struct SDTMatematica {
    int contador_arrays;
    ListaString* variaveis_array;
    char operacao;
    NoExpressao* no;
} SDTMatematica;

// Estrutura para SDT de declaração
typedef struct SDTDec {
    char* tipo_inicial;
    char* tipo;
} SDTDec;

// Estrutura para código
typedef struct Codigo {
    char* variavel;
    char* inicio;
    char* retorno;
    char* valor_retorno;
    int contador_parametros;
} Codigo;

// Estrutura principal da AST
typedef struct NoAST {
    char* identificador;
    Token* token;
    
    // Filhos
    struct NoAST** filhos;
    int quantidade_filhos;
    int capacidade_filhos;
    
    // Código gerado
    ListaString* codigo;
    char* proximo;
    char* proximo_para_for;

    // SDTs
    SDTDec decSDT;
    Codigo codigo_res_var;
    SDTMatematica mathSDT;
    
    // Contador de rótulos (simulado como ponteiro para contador global)
    int* contador_rotulos;
} NoAST;

// Funções para manipulação de nós
NoAST* criar_no_ast(const char* identificador, int* contador_rotulos);
void adicionar_filho(NoAST* self, NoAST* filho);
void definir_filhos(NoAST* self, NoAST** novos_filhos, int quantidade);
void definir_token(NoAST* self, Token* token);
char* novo_rotulo(NoAST* self);
void liberar_no_ast(NoAST* no);

// Funções para nós de expressão
NoExpressao* criar_no_expressao_unario(char operacao, const char* valor, NoExpressao* filhoA);
NoExpressao* criar_no_expressao_binario(char operacao, const char* valor, NoExpressao* filhoA, NoExpressao* filhoB);
NoExpressao* criar_no_expressao_simples(char operacao, const char* tipo, const char* valor);
NoExpressao* criar_no_expressao_basico(char operacao, const char* valor);
void liberar_no_expressao(NoExpressao* no);

// Funções auxiliares
ListaString* criar_lista_string();
void adicionar_string(ListaString* lista, const char* str);
void liberar_lista_string(ListaString* lista);
void imprimir_erro_tipo(char operacao, NoExpressao* filhoA, NoExpressao* filhoB);

#endif

#include "resolvedor_expressao.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

ResolvedorExpressao* criar_resolvedor_expressao() {
    ResolvedorExpressao* resolvedor = (ResolvedorExpressao*)malloc(sizeof(ResolvedorExpressao));
    resolvedor->contador_temp = 0;
    return resolvedor;
}

void liberar_resolvedor_expressao(ResolvedorExpressao* resolvedor) {
    free(resolvedor);
}

void imprimir_arvore_prefixo(const char* prefixo, NoExpressao* no, int eh_esquerda) {
    if (no != NULL) {
        printf("%s", prefixo);
        printf("%s", eh_esquerda ? "├── " : "└── ");

        if (strcmp(no->valor, "N") == 0) {
            printf("%c\n", no->operacao);
        } else {
            printf("%s\n", no->valor);
        }

        // Calcular novo prefixo
        char novo_prefixo[300];
        snprintf(novo_prefixo, sizeof(novo_prefixo), "%s%s", prefixo, eh_esquerda ? "│    " : "     ");
        
        // Chamar recursivamente para filhos
        imprimir_arvore_prefixo(novo_prefixo, no->filhoA, 1);
        imprimir_arvore_prefixo(novo_prefixo, no->filhoB, 0);
    }
}

void imprimir_erro(char operacao, NoExpressao* noA, NoExpressao* noB) {
    printf("====================================================\n");
    printf("Erro: operação '%c' entre tipos incompatíveis\n", operacao);
    
    printf("\nSubárvore esquerda:\n");
    imprimir_arvore_prefixo("      ", noA, 1);
    
    printf("\nSubárvore direita:\n");
    imprimir_arvore_prefixo("      ", noB, 0);
}

void imprimir_arvore(NoExpressao* no) {
    printf("====================================================\n");
    if (no && no->tipo) {
        printf("Tipo: %s\n", no->tipo);
    } else {
        printf("Árvore de expressão\n");
    }
    imprimir_arvore_prefixo("", no, 0);
}

char* gerar_variavel_temporaria(ResolvedorExpressao* resolvedor) {
    char* variavel = (char*)malloc(20 * sizeof(char));
    snprintf(variavel, 20, "$%d", resolvedor->contador_temp);
    resolvedor->contador_temp++;
    return variavel;
}

#ifndef RESOLVEDOR_EXPRESSAO_H
#define RESOLVEDOR_EXPRESSAO_H

#include "no_expressao.h"
#include <stdio.h>

// Estrutura para resolver árvores de expressão
typedef struct {
    int contador_temp;
} ResolvedorExpressao;

// Funções públicas
void imprimir_arvore_prefixo(const char* prefixo, NoExpressao* no, int eh_esquerda);
void imprimir_erro(char operacao, NoExpressao* noA, NoExpressao* noB);
void imprimir_arvore(NoExpressao* no);
char* gerar_variavel_temporaria(ResolvedorExpressao* resolvedor);

// Função para criar um resolvedor
ResolvedorExpressao* criar_resolvedor_expressao();

// Função para liberar memória
void liberar_resolvedor_expressao(ResolvedorExpressao* resolvedor);

#endif

#include "tabela_analise.h"
#include "acoes_semanticas.h" // Antigo semantic_actions.h
#include <string.h>
#include <stddef.h> // Para size_t


extern const EntradaTabelaSDT tabela_sdt_entries[];
extern const size_t num_entradas_sdt; 

const ItemTabelaSDT* obter_itens_sdt(const char* chave, size_t* quantidade) {
    for (size_t i = 0; i < num_entradas_sdt; i++) {
        if (strcmp(tabela_sdt_entries[i].chave, chave) == 0) {
            *quantidade = tabela_sdt_entries[i].quantidade;
            return tabela_sdt_entries[i].itens;
        }
    }
    return NULL;
}

const EntradaTabelaAnalise tabela_analise[] = {
    {
        "PROGRAM",
        (MapeamentoTerminal[]) {
            {"DEF", "PROGRAM-DEF"},
            {"IDENT", "PROGRAM-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR"},
            {"OPEN_BRACE", "PROGRAM-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR"},
            {"INT", "PROGRAM-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR"},
            {"FLOAT", "PROGRAM-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR"},
            {"STRING", "PROGRAM-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR"},
            {"SEMICOLON", "PROGRAM-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR"},
            {"BREAK", "PROGRAM-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR"},
            {"PRINT", "PROGRAM-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR"},
            {"READ", "PROGRAM-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR"},
            {"RETURN", "PROGRAM-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR"},
            {"IF", "PROGRAM-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR"},
            {"FOR", "PROGRAM-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR"},
            {"$", "PROGRAM-$"}
        },
        14 // Número de mapeamentos para PROGRAM
    },
    {
        "PROGRAM'",
        (MapeamentoTerminal[]) {
            {"IDENT", "PROGRAM'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR"},
            {"OPEN_BRACE", "PROGRAM'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR"},
            {"INT", "PROGRAM'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR"},
            {"FLOAT", "PROGRAM'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR"},
            {"STRING", "PROGRAM'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR"},
            {"SEMICOLON", "PROGRAM'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR"},
            {"BREAK", "PROGRAM'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR"},
            {"PRINT", "PROGRAM'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR"},
            {"READ", "PROGRAM'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR"},
            {"RETURN", "PROGRAM'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR"},
            {"IF", "PROGRAM'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR"},
            {"FOR", "PROGRAM'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR"},
            {"$", "PROGRAM'-$"}
        },
        13 // Número de mapeamentos
    },
    {
        "FUNCLIST",
        (MapeamentoTerminal[]) {
            {"DEF", "FUNCLIST-DEF"},
        },
        1
    },
    {
        "FUNCLIST'",
        (MapeamentoTerminal[]) {
            {"DEF", "FUNCLIST'-DEF"},
            {"IDENT", "FUNCLIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$"},
            {"OPEN_BRACE", "FUNCLIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$"},
            {"INT", "FUNCLIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$"},
            {"FLOAT", "FUNCLIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$"},
            {"STRING", "FUNCLIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$"},
            {"SEMICOLON", "FUNCLIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$"},
            {"BREAK", "FUNCLIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$"},
            {"PRINT", "FUNCLIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$"},
            {"READ", "FUNCLIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$"},
            {"RETURN", "FUNCLIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$"},
            {"IF", "FUNCLIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$"},
            {"FOR", "FUNCLIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$"},
            {"$", "FUNCLIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$"}
        },
        14
    },
    {
        "FUNCDEF",
        (MapeamentoTerminal[]) {
            {"DEF", "FUNCDEF-DEF"},
        },
        1
    },
    {
        "PARAMLIST",
        (MapeamentoTerminal[]) {
            {"CLOSE_PARENTHESIS", "PARAMLIST-CLOSE_PARENTHESIS"},
            {"INT", "PARAMLIST-INT"},
            {"FLOAT", "PARAMLIST-FLOAT"},
            {"STRING", "PARAMLIST-STRING"},
        },
        4
    },
    {
        "PARAMLIST'",
        (MapeamentoTerminal[]) {
            {"CLOSE_PARENTHESIS", "PARAMLIST'-CLOSE_PARENTHESIS"},
            {"COMMA", "PARAMLIST'-COMMA"},
        },
        2
    },
    {
        "STATEMENT",
        (MapeamentoTerminal[]) {
            {"IDENT", "STATEMENT-IDENT"},
            {"OPEN_BRACE", "STATEMENT-OPEN_BRACE"},
            {"INT", "STATEMENT-INT-FLOAT-STRING"},
            {"FLOAT", "STATEMENT-INT-FLOAT-STRING"},
            {"STRING", "STATEMENT-INT-FLOAT-STRING"},
            {"SEMICOLON", "STATEMENT-SEMICOLON"},
            {"BREAK", "STATEMENT-BREAK"},
            {"PRINT", "STATEMENT-PRINT"},
            {"READ", "STATEMENT-READ"},
            {"RETURN", "STATEMENT-RETURN"},
            {"IF", "STATEMENT-IF"},
            {"FOR", "STATEMENT-FOR"},
        },
        12
    },
    {
        "INDEX",
        (MapeamentoTerminal[]) {
            {"SEMICOLON", "INDEX-SEMICOLON"},
            {"OPEN_BRACKET", "INDEX-OPEN_BRACKET"},
        },
        2
    },
    {
        "VARDECL",
        (MapeamentoTerminal[]) {
            {"INT", "VARDECL-INT"},
            {"FLOAT", "VARDECL-FLOAT"},
            {"STRING", "VARDECL-STRING"},
        },
        3
    },
    {
        "ATRIBSTAT",
        (MapeamentoTerminal[]) {
            {"IDENT", "ATRIBSTAT-IDENT"},
        },
        1
    },
    {
        "ATRIBSTAT'",
        (MapeamentoTerminal[]) {
            {"IDENT", "ATRIBSTAT'-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL"},
            {"OPEN_PARENTHESIS", "ATRIBSTAT'-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL"},
            {"NI", "ATRIBSTAT'-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL"},
            {"PLUS", "ATRIBSTAT'-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL"},
            {"MINUS", "ATRIBSTAT'-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL"},
            {"NPF", "ATRIBSTAT'-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL"},
            {"STRC", "ATRIBSTAT'-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL"},
            {"NULL", "ATRIBSTAT'-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL"},
            {"CALL", "ATRIBSTAT'-CALL"},
            {"NEW", "ATRIBSTAT'-NEW"},
        },
        10
    },
    {
        "FUNCCALL",
        (MapeamentoTerminal[]) {
            {"CALL", "FUNCCALL-CALL"},
        },
        1
    },
    {
        "PARAMLISTCALL",
        (MapeamentoTerminal[]) {
            {"IDENT", "PARAMLISTCALL-IDENT"},
            {"CLOSE_PARENTHESIS", "PARAMLISTCALL-CLOSE_PARENTHESIS"},
        },
        2
    },
    {
        "PARAMLISTCALL'",
        (MapeamentoTerminal[]) {
            {"CLOSE_PARENTHESIS", "PARAMLISTCALL'-CLOSE_PARENTHESIS"},
            {"COMMA", "PARAMLISTCALL'-COMMA"},
        },
        2
    },
    {
        "PRINTSTAT",
        (MapeamentoTerminal[]) {
            {"PRINT", "PRINTSTAT-PRINT"},
        },
        1
    },
    {
        "READSTAT",
        (MapeamentoTerminal[]) {
            {"READ", "READSTAT-READ"},
        },
        1
    },
    {
        "RETURNSTAT",
        (MapeamentoTerminal[]) {
            {"RETURN", "RETURNSTAT-RETURN"},
        },
        1
    },
    {
        "RETURNSTAT'",
        (MapeamentoTerminal[]) {
            {"IDENT", "RETURNSTAT'-IDENT"},
            {"SEMICOLON", "RETURNSTAT'-SEMICOLON"},
        },
        2
    },
    {
        "IFSTAT",
        (MapeamentoTerminal[]) {
            {"IF", "IFSTAT-IF"},
        },
        1
    },
    {
        "IFSTAT'",
        (MapeamentoTerminal[]) {
            {"IDENT", "IFSTAT'-IDENT-OPEN_BRACE-CLOSE_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$"},
            {"OPEN_BRACE", "IFSTAT'-IDENT-OPEN_BRACE-CLOSE_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$"},
            {"CLOSE_BRACE", "IFSTAT'-IDENT-OPEN_BRACE-CLOSE_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$"},
            {"INT", "IFSTAT'-IDENT-OPEN_BRACE-CLOSE_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$"},
            {"FLOAT", "IFSTAT'-IDENT-OPEN_BRACE-CLOSE_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$"},
            {"STRING", "IFSTAT'-IDENT-OPEN_BRACE-CLOSE_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$"},
            {"SEMICOLON", "IFSTAT'-IDENT-OPEN_BRACE-CLOSE_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$"},
            {"BREAK", "IFSTAT'-IDENT-OPEN_BRACE-CLOSE_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$"},
            {"PRINT", "IFSTAT'-IDENT-OPEN_BRACE-CLOSE_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$"},
            {"READ", "IFSTAT'-IDENT-OPEN_BRACE-CLOSE_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$"},
            {"RETURN", "IFSTAT'-IDENT-OPEN_BRACE-CLOSE_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$"},
            {"IF", "IFSTAT'-IDENT-OPEN_BRACE-CLOSE_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$"},
            {"FOR", "IFSTAT'-IDENT-OPEN_BRACE-CLOSE_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$"},
            {"$", "IFSTAT'-IDENT-OPEN_BRACE-CLOSE_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$"},
            {"ELSE", "IFSTAT'-ELSE"},
        },
        15
    },
    {
        "FORSTAT",
        (MapeamentoTerminal[]) {
            {"FOR", "FORSTAT-FOR"},
        },
        1
    },
    {
        "STATELIST",
        (MapeamentoTerminal[]) {
            {"IDENT", "STATELIST-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR"},
            {"OPEN_BRACE", "STATELIST-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR"},
            {"INT", "STATELIST-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR"},
            {"FLOAT", "STATELIST-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR"},
            {"STRING", "STATELIST-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR"},
            {"SEMICOLON", "STATELIST-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR"},
            {"BREAK", "STATELIST-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR"},
            {"PRINT", "STATELIST-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR"},
            {"READ", "STATELIST-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR"},
            {"RETURN", "STATELIST-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR"},
            {"IF", "STATELIST-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR"},
            {"FOR", "STATELIST-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR"},
        },
        12
    },
    {
        "STATELIST'",
        (MapeamentoTerminal[]) {
            {"IDENT", "STATELIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR"},
            {"OPEN_BRACE", "STATELIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR"},
            {"INT", "STATELIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR"},
            {"FLOAT", "STATELIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR"},
            {"STRING", "STATELIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR"},
            {"SEMICOLON", "STATELIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR"},
            {"BREAK", "STATELIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR"},
            {"PRINT", "STATELIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR"},
            {"READ", "STATELIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR"},
            {"RETURN", "STATELIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR"},
            {"IF", "STATELIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR"},
            {"FOR", "STATELIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR"},
            {"CLOSE_BRACE", "STATELIST'-CLOSE_BRACE"},
        },
        13
    },
    {
        "ALLOCAUX",
        (MapeamentoTerminal[]) {
            {"CLOSE_PARENTHESIS", "ALLOCAUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_BRACKET-ASSIGN-RELOP-PLUS-MINUS-MULTIPLICATION-DIVISION-MODULUS"},
            {"SEMICOLON", "ALLOCAUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_BRACKET-ASSIGN-RELOP-PLUS-MINUS-MULTIPLICATION-DIVISION-MODULUS"},
            {"CLOSE_BRACKET", "ALLOCAUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_BRACKET-ASSIGN-RELOP-PLUS-MINUS-MULTIPLICATION-DIVISION-MODULUS"},
            {"ASSIGN", "ALLOCAUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_BRACKET-ASSIGN-RELOP-PLUS-MINUS-MULTIPLICATION-DIVISION-MODULUS"},
            {"RELOP", "ALLOCAUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_BRACKET-ASSIGN-RELOP-PLUS-MINUS-MULTIPLICATION-DIVISION-MODULUS"},
            {"PLUS", "ALLOCAUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_BRACKET-ASSIGN-RELOP-PLUS-MINUS-MULTIPLICATION-DIVISION-MODULUS"},
            {"MINUS", "ALLOCAUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_BRACKET-ASSIGN-RELOP-PLUS-MINUS-MULTIPLICATION-DIVISION-MODULUS"},
            {"MULTIPLICATION", "ALLOCAUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_BRACKET-ASSIGN-RELOP-PLUS-MINUS-MULTIPLICATION-DIVISION-MODULUS"},
            {"DIVISION", "ALLOCAUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_BRACKET-ASSIGN-RELOP-PLUS-MINUS-MULTIPLICATION-DIVISION-MODULUS"},
            {"MODULUS", "ALLOCAUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_BRACKET-ASSIGN-RELOP-PLUS-MINUS-MULTIPLICATION-DIVISION-MODULUS"},
            {"OPEN_BRACKET", "ALLOCAUX-OPEN_BRACKET"},
        },
        11
    },
    {
        "ALLOCEXPRESSION",
        (MapeamentoTerminal[]) {
            {"NEW", "ALLOCEXPRESSION-NEW"},
        },
        1
    },
    {
        "ALLOCEXPRESSION'",
        (MapeamentoTerminal[]) {
            {"INT", "ALLOCEXPRESSION'-INT"},
            {"FLOAT", "ALLOCEXPRESSION'-FLOAT"},
            {"STRING", "ALLOCEXPRESSION'-STRING"},
        },
        3
    },
    {
        "EXPRESSION",
        (MapeamentoTerminal[]) {
            {"IDENT", "EXPRESSION-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL"},
            {"OPEN_PARENTHESIS", "EXPRESSION-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL"},
            {"NI", "EXPRESSION-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL"},
            {"PLUS", "EXPRESSION-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL"},
            {"MINUS", "EXPRESSION-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL"},
            {"NPF", "EXPRESSION-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL"},
            {"STRC", "EXPRESSION-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL"},
            {"NULL", "EXPRESSION-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL"},
        },
        8
    },
    {
        "EXPRESSION'",
        (MapeamentoTerminal[]) {
            {"CLOSE_PARENTHESIS", "EXPRESSION'-CLOSE_PARENTHESIS-SEMICOLON"},
            {"SEMICOLON", "EXPRESSION'-CLOSE_PARENTHESIS-SEMICOLON"},
            {"RELOP", "EXPRESSION'-RELOP"},
        },
        3
    },
    {
        "NUMEXPRESSIONAUX",
        (MapeamentoTerminal[]) {
            {"CLOSE_PARENTHESIS", "NUMEXPRESSIONAUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_BRACKET-RELOP"},
            {"SEMICOLON", "NUMEXPRESSIONAUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_BRACKET-RELOP"},
            {"CLOSE_BRACKET", "NUMEXPRESSIONAUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_BRACKET-RELOP"},
            {"RELOP", "NUMEXPRESSIONAUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_BRACKET-RELOP"},
            {"PLUS", "NUMEXPRESSIONAUX-PLUS"},
            {"MINUS", "NUMEXPRESSIONAUX-MINUS"},
        },
        6
    },
    {
        "NUMEXPRESSION",
        (MapeamentoTerminal[]) {
            {"IDENT", "NUMEXPRESSION-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL"},
            {"OPEN_PARENTHESIS", "NUMEXPRESSION-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL"},
            {"NI", "NUMEXPRESSION-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL"},
            {"PLUS", "NUMEXPRESSION-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL"},
            {"MINUS", "NUMEXPRESSION-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL"},
            {"NPF", "NUMEXPRESSION-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL"},
            {"STRC", "NUMEXPRESSION-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL"},
            {"NULL", "NUMEXPRESSION-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL"},
        },
        8
    },
    {
        "TERM",
        (MapeamentoTerminal[]) {
            {"IDENT", "TERM-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL"},
            {"OPEN_PARENTHESIS", "TERM-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL"},
            {"NI", "TERM-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL"},
            {"PLUS", "TERM-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL"},
            {"MINUS", "TERM-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL"},
            {"NPF", "TERM-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL"},
            {"STRC", "TERM-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL"},
            {"NULL", "TERM-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL"},
        },
        8
    },
    {
        "UNARYEXPRAUX",
        (MapeamentoTerminal[]) {
            {"CLOSE_PARENTHESIS", "UNARYEXPRAUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_BRACKET-RELOP-PLUS-MINUS"},
            {"SEMICOLON", "UNARYEXPRAUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_BRACKET-RELOP-PLUS-MINUS"},
            {"CLOSE_BRACKET", "UNARYEXPRAUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_BRACKET-RELOP-PLUS-MINUS"},
            {"RELOP", "UNARYEXPRAUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_BRACKET-RELOP-PLUS-MINUS"},
            {"PLUS", "UNARYEXPRAUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_BRACKET-RELOP-PLUS-MINUS"},
            {"MINUS", "UNARYEXPRAUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_BRACKET-RELOP-PLUS-MINUS"},
            {"MULTIPLICATION", "UNARYEXPRAUX-MULTIPLICATION"},
            {"DIVISION", "UNARYEXPRAUX-DIVISION"},
            {"MODULUS", "UNARYEXPRAUX-MODULUS"},
        },
        9
    },
    {
        "UNARYEXPR",
        (MapeamentoTerminal[]) {
            {"IDENT", "UNARYEXPR-IDENT-OPEN_PARENTHESIS-NI-NPF-STRC-NULL"},
            {"OPEN_PARENTHESIS", "UNARYEXPR-IDENT-OPEN_PARENTHESIS-NI-NPF-STRC-NULL"},
            {"NI", "UNARYEXPR-IDENT-OPEN_PARENTHESIS-NI-NPF-STRC-NULL"},
            {"NPF", "UNARYEXPR-IDENT-OPEN_PARENTHESIS-NI-NPF-STRC-NULL"},
            {"STRC", "UNARYEXPR-IDENT-OPEN_PARENTHESIS-NI-NPF-STRC-NULL"},
            {"NULL", "UNARYEXPR-IDENT-OPEN_PARENTHESIS-NI-NPF-STRC-NULL"},
            {"PLUS", "UNARYEXPR-PLUS"},
            {"MINUS", "UNARYEXPR-MINUS"},
        },
        8
    },
    {
        "FACTOR",
        (MapeamentoTerminal[]) {
            {"IDENT", "FACTOR-IDENT"},
            {"OPEN_PARENTHESIS", "FACTOR-OPEN_PARENTHESIS"},
            {"NI", "FACTOR-NI"},
            {"NPF", "FACTOR-NPF"},
            {"STRC", "FACTOR-STRC"},
            {"NULL", "FACTOR-NULL"},
        },
        6
    },
    {
        "LVALUE",
        (MapeamentoTerminal[]) {
            {"IDENT", "LVALUE-IDENT"},
        },
        1
    }
};

const size_t num_entradas_tabela = sizeof(tabela_analise) / sizeof(EntradaTabelaAnalise);



#ifndef TABELA_ANALISE_H
#define TABELA_ANALISE_H

#include <string.h>
#include "item_pilha_parser.h"

// Estrutura para uma entrada na tabela SDT
typedef struct {
    const char* chave;
    ItemPilha* producao;
    size_t tamanho;
} EntradaTabelaSDT;

// Estrutura para um mapeamento terminal -> produção
typedef struct {
    const char* terminal;
    const char* chave_sdt;
} MapeamentoTerminal;

// Estrutura principal da tabela de análise
typedef struct {
    const char* nao_terminal;
    MapeamentoTerminal* mapeamentos;
    size_t num_mapeamentos;
} EntradaTabelaAnalise;

extern const EntradaTabelaAnalise tabela_analise[];
extern const size_t num_entradas_tabela;

// Protótipos das funções auxiliares
const EntradaTabelaSDT* buscar_entrada_sdt(const char* chave);

#endif

#include "tabela_sdt_auxiliar.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Forward declarations for all initialization functions
static void inicializar_producao_PROGRAM_DEF(ItemTabelaSDT* itens);
static void inicializar_producao_PROGRAM_IDENT(ItemTabelaSDT* itens);
static void inicializar_producao_PROGRAM_DOLAR(ItemTabelaSDT* itens);
static void inicializar_producao_PROGRAM_QUOTE_IDENT(ItemTabelaSDT* itens);
static void inicializar_producao_PROGRAM_QUOTE_DOLAR(ItemTabelaSDT* itens);
static void inicializar_producao_FUNCLIST_DEF(ItemTabelaSDT* itens);
static void inicializar_producao_FUNCLIST_QUOTE_DEF(ItemTabelaSDT* itens);
static void inicializar_producao_FUNCLIST_QUOTE_IDENT(ItemTabelaSDT* itens);
static void inicializar_producao_FUNCDEF_DEF(ItemTabelaSDT* itens);
static void inicializar_producao_PARAMLIST_CLOSE_PARENTHESIS(ItemTabelaSDT* itens);
static void inicializar_producao_PARAMLIST_INT(ItemTabelaSDT* itens);
static void inicializar_producao_PARAMLIST_FLOAT(ItemTabelaSDT* itens);
static void inicializar_producao_PARAMLIST_STRING(ItemTabelaSDT* itens);
static void inicializar_producao_PARAMLIST_QUOTE_CLOSE_PARENTHESIS(ItemTabelaSDT* itens);
static void inicializar_producao_PARAMLIST_QUOTE_COMMA(ItemTabelaSDT* itens);
static void inicializar_producao_STATEMENT_IDENT(ItemTabelaSDT* itens);
static void inicializar_producao_STATEMENT_OPEN_BRACE(ItemTabelaSDT* itens);
static void inicializar_producao_STATEMENT_INT_FLOAT_STRING(ItemTabelaSDT* itens);
static void inicializar_producao_STATEMENT_SEMICOLON(ItemTabelaSDT* itens);
static void inicializar_producao_STATEMENT_BREAK(ItemTabelaSDT* itens);
static void inicializar_producao_STATEMENT_PRINT(ItemTabelaSDT* itens);
static void inicializar_producao_STATEMENT_READ(ItemTabelaSDT* itens);
static void inicializar_producao_STATEMENT_RETURN(ItemTabelaSDT* itens);
static void inicializar_producao_STATEMENT_IF(ItemTabelaSDT* itens);
static void inicializar_producao_STATEMENT_FOR(ItemTabelaSDT* itens);
static void inicializar_producao_INDEX_SEMICOLON(ItemTabelaSDT* itens);
static void inicializar_producao_INDEX_OPEN_BRACKET(ItemTabelaSDT* itens);
static void inicializar_producao_VARDECL_INT(ItemTabelaSDT* itens);
static void inicializar_producao_VARDECL_FLOAT(ItemTabelaSDT* itens);
static void inicializar_producao_VARDECL_STRING(ItemTabelaSDT* itens);
static void inicializar_producao_ATRIBSTAT_IDENT(ItemTabelaSDT* itens);
static void inicializar_producao_ATRIBSTAT_QUOTE_IDENT(ItemTabelaSDT* itens);
static void inicializar_producao_ATRIBSTAT_QUOTE_CALL(ItemTabelaSDT* itens);
static void inicializar_producao_ATRIBSTAT_QUOTE_NEW(ItemTabelaSDT* itens);
static void inicializar_producao_FUNCCALL_CALL(ItemTabelaSDT* itens);
static void inicializar_producao_PARAMLISTCALL_IDENT(ItemTabelaSDT* itens);
static void inicializar_producao_PARAMLISTCALL_CLOSE_PARENTHESIS(ItemTabelaSDT* itens);
static void inicializar_producao_PARAMLISTCALL_QUOTE_CLOSE_PARENTHESIS(ItemTabelaSDT* itens);
static void inicializar_producao_PARAMLISTCALL_QUOTE_COMMA(ItemTabelaSDT* itens);
static void inicializar_producao_PRINTSTAT_PRINT(ItemTabelaSDT* itens);
static void inicializar_producao_READSTAT_READ(ItemTabelaSDT* itens);
static void inicializar_producao_RETURNSTAT_RETURN(ItemTabelaSDT* itens);
static void inicializar_producao_RETURNSTAT_QUOTE_IDENT(ItemTabelaSDT* itens);
static void inicializar_producao_RETURNSTAT_QUOTE_SEMICOLON(ItemTabelaSDT* itens);
static void inicializar_producao_IFSTAT_IF(ItemTabelaSDT* itens);
static void inicializar_producao_IFSTAT_QUOTE_IDENT(ItemTabelaSDT* itens);
static void inicializar_producao_IFSTAT_QUOTE_ELSE(ItemTabelaSDT* itens);
static void inicializar_producao_FORSTAT_FOR(ItemTabelaSDT* itens);
static void inicializar_producao_STATELIST_IDENT(ItemTabelaSDT* itens);
static void inicializar_producao_STATELIST_QUOTE_IDENT(ItemTabelaSDT* itens);
static void inicializar_producao_STATELIST_QUOTE_CLOSE_BRACE(ItemTabelaSDT* itens);
static void inicializar_producao_ALLOCAUX_CLOSE_PARENTHESIS(ItemTabelaSDT* itens);
static void inicializar_producao_ALLOCAUX_OPEN_BRACKET(ItemTabelaSDT* itens);
static void inicializar_producao_ALLOCEXPRESSION_NEW(ItemTabelaSDT* itens);
static void inicializar_producao_ALLOCEXPRESSION_QUOTE_INT(ItemTabelaSDT* itens);
static void inicializar_producao_ALLOCEXPRESSION_QUOTE_FLOAT(ItemTabelaSDT* itens);
static void inicializar_producao_ALLOCEXPRESSION_QUOTE_STRING(ItemTabelaSDT* itens);
static void inicializar_producao_EXPRESSION_IDENT(ItemTabelaSDT* itens);
static void inicializar_producao_EXPRESSION_QUOTE_CLOSE_PARENTHESIS(ItemTabelaSDT* itens);
static void inicializar_producao_EXPRESSION_QUOTE_RELOP(ItemTabelaSDT* itens);
static void inicializar_producao_NUMEXPRESSIONAUX_CLOSE_PARENTHESIS(ItemTabelaSDT* itens);
static void inicializar_producao_NUMEXPRESSIONAUX_PLUS(ItemTabelaSDT* itens);
static void inicializar_producao_NUMEXPRESSIONAUX_MINUS(ItemTabelaSDT* itens);
static void inicializar_producao_NUMEXPRESSION_IDENT(ItemTabelaSDT* itens);
static void inicializar_producao_TERM_IDENT(ItemTabelaSDT* itens);
static void inicializar_producao_UNARYEXPRAUX_CLOSE_PARENTHESIS(ItemTabelaSDT* itens);
static void inicializar_producao_UNARYEXPRAUX_MULTIPLICATION(ItemTabelaSDT* itens);
static void inicializar_producao_UNARYEXPRAUX_DIVISION(ItemTabelaSDT* itens);
static void inicializar_producao_UNARYEXPRAUX_MODULUS(ItemTabelaSDT* itens);
static void inicializar_producao_UNARYEXPR_IDENT(ItemTabelaSDT* itens);
static void inicializar_producao_UNARYEXPR_PLUS(ItemTabelaSDT* itens);
static void inicializar_producao_UNARYEXPR_MINUS(ItemTabelaSDT* itens);
static void inicializar_producao_FACTOR_IDENT(ItemTabelaSDT* itens);
static void inicializar_producao_FACTOR_OPEN_PARENTHESIS(ItemTabelaSDT* itens);
static void inicializar_producao_FACTOR_NI(ItemTabelaSDT* itens);
static void inicializar_producao_FACTOR_NPF(ItemTabelaSDT* itens);
static void inicializar_producao_FACTOR_STRC(ItemTabelaSDT* itens);
static void inicializar_producao_FACTOR_NULL(ItemTabelaSDT* itens);


void inicializar_entradas(EntradaTabelaSDT* entradas) {
    // PROGRAM-DEF
    entradas[0].chave = strdup("PROGRAM-DEF");
    entradas[0].quantidade = 7;
    entradas[0].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 7);
    inicializar_producao_PROGRAM_DEF(entradas[0].itens);
    
    // PROGRAM-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR
    entradas[1].chave = strdup("PROGRAM-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR");
    entradas[1].quantidade = 6;
    entradas[1].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 6);
    inicializar_producao_PROGRAM_IDENT(entradas[1].itens);

    // PROGRAM-$
    entradas[2].chave = strdup("PROGRAM-$");
    entradas[2].quantidade = 2;
    entradas[2].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 2);
    inicializar_producao_PROGRAM_DOLAR(entradas[2].itens);

    // PROGRAM'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR
    entradas[3].chave = strdup("PROGRAM'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR");
    entradas[3].quantidade = 3;
    entradas[3].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 3);
    inicializar_producao_PROGRAM_QUOTE_IDENT(entradas[3].itens);

    // PROGRAM'-$
    entradas[4].chave = strdup("PROGRAM'-$");
    entradas[4].quantidade = 1;
    entradas[4].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 1);
    inicializar_producao_PROGRAM_QUOTE_DOLAR(entradas[4].itens);

    // FUNCLIST-DEF
    entradas[5].chave = strdup("FUNCLIST-DEF");
    entradas[5].quantidade = 4;
    entradas[5].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 4);
    inicializar_producao_FUNCLIST_DEF(entradas[5].itens);

    // FUNCLIST'-DEF
    entradas[6].chave = strdup("FUNCLIST'-DEF");
    entradas[6].quantidade = 3;
    entradas[6].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 3);
    inicializar_producao_FUNCLIST_QUOTE_DEF(entradas[6].itens);

    // FUNCLIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$
    entradas[7].chave = strdup("FUNCLIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$");
    entradas[7].quantidade = 1;
    entradas[7].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 1);
    inicializar_producao_FUNCLIST_QUOTE_IDENT(entradas[7].itens);

    // FUNCDEF-DEF
    entradas[8].chave = strdup("FUNCDEF-DEF");
    entradas[8].quantidade = 14;
    entradas[8].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 14);
    inicializar_producao_FUNCDEF_DEF(entradas[8].itens);

    // PARAMLIST-CLOSE_PARENTHESIS
    entradas[9].chave = strdup("PARAMLIST-CLOSE_PARENTHESIS");
    entradas[9].quantidade = 1;
    entradas[9].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 1);
    inicializar_producao_PARAMLIST_CLOSE_PARENTHESIS(entradas[9].itens);

    // PARAMLIST-INT
    entradas[10].chave = strdup("PARAMLIST-INT");
    entradas[10].quantidade = 5;
    entradas[10].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 5);
    inicializar_producao_PARAMLIST_INT(entradas[10].itens);

    // PARAMLIST-FLOAT
    entradas[11].chave = strdup("PARAMLIST-FLOAT");
    entradas[11].quantidade = 5;
    entradas[11].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 5);
    inicializar_producao_PARAMLIST_FLOAT(entradas[11].itens);

    // PARAMLIST-STRING
    entradas[12].chave = strdup("PARAMLIST-STRING");
    entradas[12].quantidade = 5;
    entradas[12].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 5);
    inicializar_producao_PARAMLIST_STRING(entradas[12].itens);

    // PARAMLIST'-CLOSE_PARENTHESIS
    entradas[13].chave = strdup("PARAMLIST'-CLOSE_PARENTHESIS");
    entradas[13].quantidade = 1;
    entradas[13].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 1);
    inicializar_producao_PARAMLIST_QUOTE_CLOSE_PARENTHESIS(entradas[13].itens);

    // PARAMLIST'-COMMA
    entradas[14].chave = strdup("PARAMLIST'-COMMA");
    entradas[14].quantidade = 2;
    entradas[14].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 2);
    inicializar_producao_PARAMLIST_QUOTE_COMMA(entradas[14].itens);

    // STATEMENT-IDENT
    entradas[15].chave = strdup("STATEMENT-IDENT");
    entradas[15].quantidade = 4;
    entradas[15].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 4);
    inicializar_producao_STATEMENT_IDENT(entradas[15].itens);

    // STATEMENT-OPEN_BRACE
    entradas[16].chave = strdup("STATEMENT-OPEN_BRACE");
    entradas[16].quantidade = 7;
    entradas[16].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 7);
    inicializar_producao_STATEMENT_OPEN_BRACE(entradas[16].itens);

    // STATEMENT-INT-FLOAT-STRING
    entradas[17].chave = strdup("STATEMENT-INT-FLOAT-STRING");
    entradas[17].quantidade = 3;
    entradas[17].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 3);
    inicializar_producao_STATEMENT_INT_FLOAT_STRING(entradas[17].itens);

    // STATEMENT-SEMICOLON
    entradas[18].chave = strdup("STATEMENT-SEMICOLON");
    entradas[18].quantidade = 1;
    entradas[18].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 1);
    inicializar_producao_STATEMENT_SEMICOLON(entradas[18].itens);

    // STATEMENT-BREAK
    entradas[19].chave = strdup("STATEMENT-BREAK");
    entradas[19].quantidade = 4;
    entradas[19].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 4);
    inicializar_producao_STATEMENT_BREAK(entradas[19].itens);

    // STATEMENT-PRINT
    entradas[20].chave = strdup("STATEMENT-PRINT");
    entradas[20].quantidade = 4;
    entradas[20].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 4);
    inicializar_producao_STATEMENT_PRINT(entradas[20].itens);

    // STATEMENT-READ
    entradas[21].chave = strdup("STATEMENT-READ");
    entradas[21].quantidade = 4;
    entradas[21].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 4);
    inicializar_producao_STATEMENT_READ(entradas[21].itens);

    // STATEMENT-RETURN
    entradas[22].chave = strdup("STATEMENT-RETURN");
    entradas[22].quantidade = 4;
    entradas[22].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 4);
    inicializar_producao_STATEMENT_RETURN(entradas[22].itens);

    // STATEMENT-IF
    entradas[23].chave = strdup("STATEMENT-IF");
    entradas[23].quantidade = 3;
    entradas[23].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 3);
    inicializar_producao_STATEMENT_IF(entradas[23].itens);

    // STATEMENT-FOR
    entradas[24].chave = strdup("STATEMENT-FOR");
    entradas[24].quantidade = 3;
    entradas[24].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 3);
    inicializar_producao_STATEMENT_FOR(entradas[24].itens);

    // INDEX-SEMICOLON
    entradas[25].chave = strdup("INDEX-SEMICOLON");
    entradas[25].quantidade = 2;
    entradas[25].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 2);
    inicializar_producao_INDEX_SEMICOLON(entradas[25].itens);

    // INDEX-OPEN_BRACKET
    entradas[26].chave = strdup("INDEX-OPEN_BRACKET");
    entradas[26].quantidade = 6;
    entradas[26].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 6);
    inicializar_producao_INDEX_OPEN_BRACKET(entradas[26].itens);

    // VARDECL-INT
    entradas[27].chave = strdup("VARDECL-INT");
    entradas[27].quantidade = 7;
    entradas[27].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 7);
    inicializar_producao_VARDECL_INT(entradas[27].itens);

    // VARDECL-FLOAT
    entradas[28].chave = strdup("VARDECL-FLOAT");
    entradas[28].quantidade = 7;
    entradas[28].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 7);
    inicializar_producao_VARDECL_FLOAT(entradas[28].itens);

    // VARDECL-STRING
    entradas[29].chave = strdup("VARDECL-STRING");
    entradas[29].quantidade = 7;
    entradas[29].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 7);
    inicializar_producao_VARDECL_STRING(entradas[29].itens);

    // ATRIBSTAT-IDENT
    entradas[30].chave = strdup("ATRIBSTAT-IDENT");
    entradas[30].quantidade = 5;
    entradas[30].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 5);
    inicializar_producao_ATRIBSTAT_IDENT(entradas[30].itens);

    // ATRIBSTAT'-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL
    entradas[31].chave = strdup("ATRIBSTAT'-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL");
    entradas[31].quantidade = 2;
    entradas[31].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 2);
    inicializar_producao_ATRIBSTAT_QUOTE_IDENT(entradas[31].itens);

    // ATRIBSTAT'-CALL
    entradas[32].chave = strdup("ATRIBSTAT'-CALL");
    entradas[32].quantidade = 4;
    entradas[32].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 4);
    inicializar_producao_ATRIBSTAT_QUOTE_CALL(entradas[32].itens);

    // ATRIBSTAT'-NEW
    entradas[33].chave = strdup("ATRIBSTAT'-NEW");
    entradas[33].quantidade = 1;
    entradas[33].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 1);
    inicializar_producao_ATRIBSTAT_QUOTE_NEW(entradas[33].itens);

    // FUNCCALL-CALL
    entradas[34].chave = strdup("FUNCCALL-CALL");
    entradas[34].quantidade = 9;
    entradas[34].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 9);
    inicializar_producao_FUNCCALL_CALL(entradas[34].itens);

    // PARAMLISTCALL-IDENT
    entradas[35].chave = strdup("PARAMLISTCALL-IDENT");
    entradas[35].quantidade = 4;
    entradas[35].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 4);
    inicializar_producao_PARAMLISTCALL_IDENT(entradas[35].itens);

    // PARAMLISTCALL-CLOSE_PARENTHESIS
    entradas[36].chave = strdup("PARAMLISTCALL-CLOSE_PARENTHESIS");
    entradas[36].quantidade = 1;
    entradas[36].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 1);
    inicializar_producao_PARAMLISTCALL_CLOSE_PARENTHESIS(entradas[36].itens);

    // PARAMLISTCALL'-CLOSE_PARENTHESIS
    entradas[37].chave = strdup("PARAMLISTCALL'-CLOSE_PARENTHESIS");
    entradas[37].quantidade = 1;
    entradas[37].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 1);
    inicializar_producao_PARAMLISTCALL_QUOTE_CLOSE_PARENTHESIS(entradas[37].itens);

    // PARAMLISTCALL'-COMMA
    entradas[38].chave = strdup("PARAMLISTCALL'-COMMA");
    entradas[38].quantidade = 4;
    entradas[38].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 4);
    inicializar_producao_PARAMLISTCALL_QUOTE_COMMA(entradas[38].itens);

    // PRINTSTAT-PRINT
    entradas[39].chave = strdup("PRINTSTAT-PRINT");
    entradas[39].quantidade = 3;
    entradas[39].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 3);
    inicializar_producao_PRINTSTAT_PRINT(entradas[39].itens);

    // READSTAT-READ
    entradas[40].chave = strdup("READSTAT-READ");
    entradas[40].quantidade = 3;
    entradas[40].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 3);
    inicializar_producao_READSTAT_READ(entradas[40].itens);

    // RETURNSTAT-RETURN
    entradas[41].chave = strdup("RETURNSTAT-RETURN");
    entradas[41].quantidade = 3;
    entradas[41].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 3);
    inicializar_producao_RETURNSTAT_RETURN(entradas[41].itens);

    // RETURNSTAT'-IDENT
    entradas[42].chave = strdup("RETURNSTAT'-IDENT");
    entradas[42].quantidade = 2;
    entradas[42].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 2);
    inicializar_producao_RETURNSTAT_QUOTE_IDENT(entradas[42].itens);

    // RETURNSTAT'-SEMICOLON
    entradas[43].chave = strdup("RETURNSTAT'-SEMICOLON");
    entradas[43].quantidade = 1;
    entradas[43].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 1);
    inicializar_producao_RETURNSTAT_QUOTE_SEMICOLON(entradas[43].itens);

    // IFSTAT-IF
    entradas[44].chave = strdup("IFSTAT-IF");
    entradas[44].quantidade = 12;
    entradas[44].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 12);
    inicializar_producao_IFSTAT_IF(entradas[44].itens);

    // IFSTAT'-IDENT-OPEN_BRACE-CLOSE_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$
    entradas[45].chave = strdup("IFSTAT'-IDENT-OPEN_BRACE-CLOSE_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$");
    entradas[45].quantidade = 2;
    entradas[45].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 2);
    inicializar_producao_IFSTAT_QUOTE_IDENT(entradas[45].itens);

    // IFSTAT'-ELSE
    entradas[46].chave = strdup("IFSTAT'-ELSE");
    entradas[46].quantidade = 8;
    entradas[46].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 8);
    inicializar_producao_IFSTAT_QUOTE_ELSE(entradas[46].itens);

    // FORSTAT-FOR
    entradas[47].chave = strdup("FORSTAT-FOR");
    entradas[47].quantidade = 13;
    entradas[47].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 13);
    inicializar_producao_FORSTAT_FOR(entradas[47].itens);

    // STATELIST-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR
    entradas[48].chave = strdup("STATELIST-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR");
    entradas[48].quantidade = 5;
    entradas[48].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 5);
    inicializar_producao_STATELIST_IDENT(entradas[48].itens);

    // STATELIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR
    entradas[49].chave = strdup("STATELIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR");
    entradas[49].quantidade = 3;
    entradas[49].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 3);
    inicializar_producao_STATELIST_QUOTE_IDENT(entradas[49].itens);

    // STATELIST'-CLOSE_BRACE
    entradas[50].chave = strdup("STATELIST'-CLOSE_BRACE");
    entradas[50].quantidade = 3;
    entradas[50].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 3);
    inicializar_producao_STATELIST_QUOTE_CLOSE_BRACE(entradas[50].itens);

    // ALLOCAUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_BRACKET-ASSIGN-RELOP-PLUS-MINUS-MULTIPLICATION-DIVISION-MODULUS
    entradas[51].chave = strdup("ALLOCAUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_BRACKET-ASSIGN-RELOP-PLUS-MINUS-MULTIPLICATION-DIVISION-MODULUS");
    entradas[51].quantidade = 1;
    entradas[51].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 1);
    inicializar_producao_ALLOCAUX_CLOSE_PARENTHESIS(entradas[51].itens);

    // ALLOCAUX-OPEN_BRACKET
    entradas[52].chave = strdup("ALLOCAUX-OPEN_BRACKET");
    entradas[52].quantidade = 6;
    entradas[52].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 6);
    inicializar_producao_ALLOCAUX_OPEN_BRACKET(entradas[52].itens);

    // ALLOCEXPRESSION-NEW
    entradas[53].chave = strdup("ALLOCEXPRESSION-NEW");
    entradas[53].quantidade = 2;
    entradas[53].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 2);
    inicializar_producao_ALLOCEXPRESSION_NEW(entradas[53].itens);

    // ALLOCEXPRESSION'-INT
    entradas[54].chave = strdup("ALLOCEXPRESSION'-INT");
    entradas[54].quantidade = 6;
    entradas[54].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 6);
    inicializar_producao_ALLOCEXPRESSION_QUOTE_INT(entradas[54].itens);

    // ALLOCEXPRESSION'-FLOAT
    entradas[55].chave = strdup("ALLOCEXPRESSION'-FLOAT");
    entradas[55].quantidade = 6;
    entradas[55].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 6);
    inicializar_producao_ALLOCEXPRESSION_QUOTE_FLOAT(entradas[55].itens);

    // ALLOCEXPRESSION'-STRING
    entradas[56].chave = strdup("ALLOCEXPRESSION'-STRING");
    entradas[56].quantidade = 6;
    entradas[56].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 6);
    inicializar_producao_ALLOCEXPRESSION_QUOTE_STRING(entradas[56].itens);
    
    // EXPRESSION-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL
    entradas[57].chave = strdup("EXPRESSION-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL");
    entradas[57].quantidade = 4;
    entradas[57].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 4);
    inicializar_producao_EXPRESSION_IDENT(entradas[57].itens);

    // EXPRESSION'-CLOSE_PARENTHESIS-SEMICOLON
    entradas[58].chave = strdup("EXPRESSION'-CLOSE_PARENTHESIS-SEMICOLON");
    entradas[58].quantidade = 1;
    entradas[58].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 1);
    inicializar_producao_EXPRESSION_QUOTE_CLOSE_PARENTHESIS(entradas[58].itens);

    // EXPRESSION'-RELOP
    entradas[59].chave = strdup("EXPRESSION'-RELOP");
    entradas[59].quantidade = 4;
    entradas[59].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 4);
    inicializar_producao_EXPRESSION_QUOTE_RELOP(entradas[59].itens);

    // NUMEXPRESSIONAUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_BRACKET-RELOP
    entradas[60].chave = strdup("NUMEXPRESSIONAUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_BRACKET-RELOP");
    entradas[60].quantidade = 1;
    entradas[60].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 1);
    inicializar_producao_NUMEXPRESSIONAUX_CLOSE_PARENTHESIS(entradas[60].itens);

    // NUMEXPRESSIONAUX-PLUS
    entradas[61].chave = strdup("NUMEXPRESSIONAUX-PLUS");
    entradas[61].quantidade = 3;
    entradas[61].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 3);
    inicializar_producao_NUMEXPRESSIONAUX_PLUS(entradas[61].itens);

    // NUMEXPRESSIONAUX-MINUS
    entradas[62].chave = strdup("NUMEXPRESSIONAUX-MINUS");
    entradas[62].quantidade = 3;
    entradas[62].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 3);
    inicializar_producao_NUMEXPRESSIONAUX_MINUS(entradas[62].itens);

    // NUMEXPRESSION-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL
    entradas[63].chave = strdup("NUMEXPRESSION-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL");
    entradas[63].quantidade = 3;
    entradas[63].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 3);
    inicializar_producao_NUMEXPRESSION_IDENT(entradas[63].itens);

    // TERM-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL
    entradas[64].chave = strdup("TERM-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL");
    entradas[64].quantidade = 3;
    entradas[64].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 3);
    inicializar_producao_TERM_IDENT(entradas[64].itens);

    // UNARYEXPRAUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_BRACKET-RELOP-PLUS-MINUS
    entradas[65].chave = strdup("UNARYEXPRAUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_BRACKET-RELOP-PLUS-MINUS");
    entradas[65].quantidade = 1;
    entradas[65].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 1);
    inicializar_producao_UNARYEXPRAUX_CLOSE_PARENTHESIS(entradas[65].itens);

    // UNARYEXPRAUX-MULTIPLICATION
    entradas[66].chave = strdup("UNARYEXPRAUX-MULTIPLICATION");
    entradas[66].quantidade = 3;
    entradas[66].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 3);
    inicializar_producao_UNARYEXPRAUX_MULTIPLICATION(entradas[66].itens);

    // UNARYEXPRAUX-DIVISION
    entradas[67].chave = strdup("UNARYEXPRAUX-DIVISION");
    entradas[67].quantidade = 3;
    entradas[67].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 3);
    inicializar_producao_UNARYEXPRAUX_DIVISION(entradas[67].itens);

    // UNARYEXPRAUX-MODULUS
    entradas[68].chave = strdup("UNARYEXPRAUX-MODULUS");
    entradas[68].quantidade = 3;
    entradas[68].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 3);
    inicializar_producao_UNARYEXPRAUX_MODULUS(entradas[68].itens);

    // UNARYEXPR-IDENT-OPEN_PARENTHESIS-NI-NPF-STRC-NULL
    entradas[69].chave = strdup("UNARYEXPR-IDENT-OPEN_PARENTHESIS-NI-NPF-STRC-NULL");
    entradas[69].quantidade = 2;
    entradas[69].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 2);
    inicializar_producao_UNARYEXPR_IDENT(entradas[69].itens);

    // UNARYEXPR-PLUS
    entradas[70].chave = strdup("UNARYEXPR-PLUS");
    entradas[70].quantidade = 3;
    entradas[70].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 3);
    inicializar_producao_UNARYEXPR_PLUS(entradas[70].itens);

    // UNARYEXPR-MINUS
    entradas[71].chave = strdup("UNARYEXPR-MINUS");
    entradas[71].quantidade = 3;
    entradas[71].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 3);
    inicializar_producao_UNARYEXPR_MINUS(entradas[71].itens);

    // FACTOR-IDENT
    entradas[72].chave = strdup("FACTOR-IDENT");
    entradas[72].quantidade = 2;
    entradas[72].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 2);
    inicializar_producao_FACTOR_IDENT(entradas[72].itens);

    // FACTOR-OPEN_PARENTHESIS
    entradas[73].chave = strdup("FACTOR-OPEN_PARENTHESIS");
    entradas[73].quantidade = 4;
    entradas[73].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 4);
    inicializar_producao_FACTOR_OPEN_PARENTHESIS(entradas[73].itens);

    // FACTOR-NI
    entradas[74].chave = strdup("FACTOR-NI");
    entradas[74].quantidade = 2;
    entradas[74].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 2);
    inicializar_producao_FACTOR_NI(entradas[74].itens);

    // FACTOR-NPF
    entradas[75].chave = strdup("FACTOR-NPF");
    entradas[75].quantidade = 2;
    entradas[75].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 2);
    inicializar_producao_FACTOR_NPF(entradas[75].itens);

    // FACTOR-STRC
    entradas[76].chave = strdup("FACTOR-STRC");
    entradas[76].quantidade = 2;
    entradas[76].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 2);
    inicializar_producao_FACTOR_STRC(entradas[76].itens);

    // FACTOR-NULL
    entradas[77].chave = strdup("FACTOR-NULL");
    entradas[77].quantidade = 1;
    entradas[77].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 1);
    inicializar_producao_FACTOR_NULL(entradas[77].itens);

    // LVALUE-IDENT - The one you provided as the last entry
    entradas[78].chave = strdup("LVALUE-IDENT");
    entradas[78].quantidade = 6;
    entradas[78].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 6);
    entradas[78].itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "IDENT"}};
    entradas[78].itens[1] = (ItemTabelaSDT){ACAO, {.acao = DECLARAR_VERIFICAR_acao2}};
    entradas[78].itens[2] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_obter_variavel_lvalue}};
    entradas[78].itens[3] = (ItemTabelaSDT){ACAO, {.acao = EXPA_avaliar_identificador}};
    entradas[78].itens[4] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "ALLOCAUX"}};
    entradas[78].itens[5] = (ItemTabelaSDT){ACAO, {.acao = EXPA_avaliar_identificador}};
}

static void inicializar_producao_PROGRAM_DOLAR(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){ACAO, {.acao = ESCOPO_acao1}};
    itens[1] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "ε"}};
}

static void inicializar_producao_PROGRAM_QUOTE_IDENT(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_herdar_proximo}};
    itens[1] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "STATEMENT"}};
    itens[2] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_obter_codigo_filhos}};
}

static void inicializar_producao_PROGRAM_QUOTE_DOLAR(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "ε"}};
}

static void inicializar_producao_FUNCLIST_DEF(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_herdar_proximo}};
    itens[1] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "FUNCDEF"}};
    itens[2] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "FUNCLIST'"}};
    itens[3] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_obter_codigo_filhos}};
}

static void inicializar_producao_FUNCLIST_QUOTE_DEF(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_herdar_proximo}};
    itens[1] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "FUNCLIST"}};
    itens[2] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_obter_codigo_filhos}};
}

static void inicializar_producao_FUNCLIST_QUOTE_IDENT(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "ε"}};
}

static void inicializar_producao_FUNCDEF_DEF(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "DEF"}};
    itens[1] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "IDENT"}};
    itens[2] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_funcdef_h}};
    itens[3] = (ItemTabelaSDT){ACAO, {.acao = ESCOPO_acao3}};
    itens[4] = (ItemTabelaSDT){ACAO, {.acao = DEC_acao1}};
    itens[5] = (ItemTabelaSDT){ACAO, {.acao = ESCOPO_acao4}};
    itens[6] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "OPEN_PARENTHESIS"}};
    itens[7] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "PARAMLIST"}};
    itens[8] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "CLOSE_PARENTHESIS"}};
    itens[9] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "OPEN_BRACE"}};
    itens[10] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "STATELIST"}};
    itens[11] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "CLOSE_BRACE"}};
    itens[12] = (ItemTabelaSDT){ACAO, {.acao = ESCOPO_acao2}};
    itens[13] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_funcdef_s}};
}

static void inicializar_producao_PARAMLIST_CLOSE_PARENTHESIS(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "ε"}};
}

static void inicializar_producao_PARAMLIST_INT(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "INT"}};
    itens[1] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "IDENT"}};
    itens[2] = (ItemTabelaSDT){ACAO, {.acao = ESCOPO_acao3}};
    itens[3] = (ItemTabelaSDT){ACAO, {.acao = DEC_acao2}};
    itens[4] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "PARAMLIST'"}};
}

static void inicializar_producao_PARAMLIST_FLOAT(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "FLOAT"}};
    itens[1] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "IDENT"}};
    itens[2] = (ItemTabelaSDT){ACAO, {.acao = ESCOPO_acao3}};
    itens[3] = (ItemTabelaSDT){ACAO, {.acao = DEC_acao3}};
    itens[4] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "PARAMLIST'"}};
}

static void inicializar_producao_PARAMLIST_STRING(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "STRING"}};
    itens[1] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "IDENT"}};
    itens[2] = (ItemTabelaSDT){ACAO, {.acao = ESCOPO_acao3}};
    itens[3] = (ItemTabelaSDT){ACAO, {.acao = DEC_acao4}};
    itens[4] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "PARAMLIST'"}};
}

static void inicializar_producao_PARAMLIST_QUOTE_CLOSE_PARENTHESIS(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "ε"}};
}

static void inicializar_producao_PARAMLIST_QUOTE_COMMA(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "COMMA"}};
    itens[1] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "PARAMLIST"}};
}

static void inicializar_producao_STATEMENT_IDENT(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_herdar_proximo}};
    itens[1] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "ATRIBSTAT"}};
    itens[2] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "SEMICOLON"}};
    itens[3] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_obter_codigo_filhos}};
}

static void inicializar_producao_STATEMENT_OPEN_BRACE(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_herdar_proximo}};
    itens[1] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "OPEN_BRACE"}};
    itens[2] = (ItemTabelaSDT){ACAO, {.acao = ESCOPO_acao1}};
    itens[3] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "STATELIST"}};
    itens[4] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "CLOSE_BRACE"}};
    itens[5] = (ItemTabelaSDT){ACAO, {.acao = ESCOPO_acao2}};
    itens[6] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_obter_codigo_filhos}};
}

static void inicializar_producao_STATEMENT_INT_FLOAT_STRING(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "VARDECL"}};
    itens[1] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "SEMICOLON"}};
    itens[2] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_obter_codigo_filhos}};
}

static void inicializar_producao_STATEMENT_SEMICOLON(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "SEMICOLON"}};
}

static void inicializar_producao_STATEMENT_BREAK(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "BREAK"}};
    itens[1] = (ItemTabelaSDT){ACAO, {.acao = BREAK_acao1}};
    itens[2] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "SEMICOLON"}};
    itens[3] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_handle_break}};
}

static void inicializar_producao_STATEMENT_PRINT(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_herdar_proximo}};
    itens[1] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "PRINTSTAT"}};
    itens[2] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "SEMICOLON"}};
    itens[3] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_obter_codigo_filhos}};
}

static void inicializar_producao_STATEMENT_READ(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_herdar_proximo}};
    itens[1] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "READSTAT"}};
    itens[2] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "SEMICOLON"}};
    itens[3] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_obter_codigo_filhos}};
}

static void inicializar_producao_STATEMENT_RETURN(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_herdar_proximo}};
    itens[1] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "RETURNSTAT"}};
    itens[2] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "SEMICOLON"}};
    itens[3] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_obter_codigo_filhos}};
}

static void inicializar_producao_STATEMENT_IF(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_herdar_proximo}};
    itens[1] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "IFSTAT"}};
    itens[2] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_obter_codigo_filhos}};
}

static void inicializar_producao_STATEMENT_FOR(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_herdar_proximo}};
    itens[1] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "FORSTAT"}};
    itens[2] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_obter_codigo_filhos}};
}

static void inicializar_producao_INDEX_SEMICOLON(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "ε"}};
    itens[1] = (ItemTabelaSDT){ACAO, {.acao = DEC_acao7}};
}

static void inicializar_producao_INDEX_OPEN_BRACKET(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "OPEN_BRACKET"}};
    itens[1] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "NI"}};
    itens[2] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "CLOSE_BRACKET"}};
    itens[3] = (ItemTabelaSDT){ACAO, {.acao = DEC_acao5}};
    itens[4] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "INDEX"}};
    itens[5] = (ItemTabelaSDT){ACAO, {.acao = DEC_acao6}};
}

static void inicializar_producao_VARDECL_INT(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_herdar_proximo}};
    itens[1] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "INT"}};
    itens[2] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "IDENT"}};
    itens[3] = (ItemTabelaSDT){ACAO, {.acao = ESCOPO_acao3}};
    itens[4] = (ItemTabelaSDT){ACAO, {.acao = DEC_acao8}};
    itens[5] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "INDEX"}};
    itens[6] = (ItemTabelaSDT){ACAO, {.acao = DEC_acao11}};
    itens[7] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_obter_codigo_filhos}}; // This was missing in your C++ reference. Assuming it should be here.
}

static void inicializar_producao_VARDECL_FLOAT(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_herdar_proximo}};
    itens[1] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "FLOAT"}};
    itens[2] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "IDENT"}};
    itens[3] = (ItemTabelaSDT){ACAO, {.acao = ESCOPO_acao3}};
    itens[4] = (ItemTabelaSDT){ACAO, {.acao = DEC_acao9}};
    itens[5] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "INDEX"}};
    itens[6] = (ItemTabelaSDT){ACAO, {.acao = DEC_acao11}};
    itens[7] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_obter_codigo_filhos}};
}

static void inicializar_producao_VARDECL_STRING(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_herdar_proximo}};
    itens[1] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "STRING"}};
    itens[2] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "IDENT"}};
    itens[3] = (ItemTabelaSDT){ACAO, {.acao = ESCOPO_acao3}};
    itens[4] = (ItemTabelaSDT){ACAO, {.acao = DEC_acao10}};
    itens[5] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "INDEX"}};
    itens[6] = (ItemTabelaSDT){ACAO, {.acao = DEC_acao11}};
    itens[7] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_obter_codigo_filhos}};
}

static void inicializar_producao_ATRIBSTAT_IDENT(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_herdar_proximo}};
    itens[1] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "LVALUE"}};
    itens[2] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "ASSIGN"}};
    itens[3] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "ATRIBSTAT'"}};
    itens[4] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_assign}};
}

static void inicializar_producao_ATRIBSTAT_QUOTE_IDENT(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "EXPRESSION"}};
    itens[1] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_set_expression_val}};
}

static void inicializar_producao_ATRIBSTAT_QUOTE_CALL(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_get_return_val}};
    itens[1] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_herdar_proximo}};
    itens[2] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "FUNCCALL"}};
    itens[3] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_obter_codigo_filhos}};
}

static void inicializar_producao_ATRIBSTAT_QUOTE_NEW(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "ALLOCEXPRESSION"}};
}

static void inicializar_producao_FUNCCALL_CALL(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "CALL"}};
    itens[1] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "OPEN_PARENTHESIS"}};
    itens[2] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "IDENT"}};
    itens[3] = (ItemTabelaSDT){ACAO, {.acao = DECLARAR_VERIFICAR_acao1}};
    itens[4] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "OPEN_PARENTHESIS"}};
    itens[5] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "PARAMLISTCALL"}};
    itens[6] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "CLOSE_PARENTHESIS"}};
    itens[7] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "CLOSE_PARENTHESIS"}};
    itens[8] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_funccall}};
}

static void inicializar_producao_PARAMLISTCALL_IDENT(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "IDENT"}};
    itens[1] = (ItemTabelaSDT){ACAO, {.acao = DECLARAR_VERIFICAR_acao2}};
    itens[2] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "PARAMLISTCALL'"}};
    itens[3] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_paramlistcall}};
}

static void inicializar_producao_PARAMLISTCALL_CLOSE_PARENTHESIS(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "ε"}};
}

static void inicializar_producao_PARAMLISTCALL_QUOTE_CLOSE_PARENTHESIS(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "ε"}};
}

static void inicializar_producao_PARAMLISTCALL_QUOTE_COMMA(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "COMMA"}};
    itens[1] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "PARAMLISTCALL"}};
    itens[2] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_obter_params}};
    itens[3] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_obter_codigo_filhos}};
}

static void inicializar_producao_PRINTSTAT_PRINT(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "PRINT"}};
    itens[1] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "EXPRESSION"}};
    itens[2] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_handle_printstat}};
}

static void inicializar_producao_READSTAT_READ(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "READ"}};
    itens[1] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "LVALUE"}};
    itens[2] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_handle_readstat}};
}

static void inicializar_producao_RETURNSTAT_RETURN(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "RETURN"}};
    itens[1] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "RETURNSTAT'"}};
    itens[2] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_handle_return}};
}

static void inicializar_producao_RETURNSTAT_QUOTE_IDENT(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "LVALUE"}};
    itens[1] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_handle_return2}};
}

static void inicializar_producao_RETURNSTAT_QUOTE_SEMICOLON(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "ε"}};
}

static void inicializar_producao_IFSTAT_IF(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_herdar_proximo}};
    itens[1] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "IF"}};
    itens[2] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "OPEN_PARENTHESIS"}};
    itens[3] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "EXPRESSION"}};
    itens[4] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "CLOSE_PARENTHESIS"}};
    itens[5] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "OPEN_BRACE"}};
    itens[6] = (ItemTabelaSDT){ACAO, {.acao = ESCOPO_acao5}};
    itens[7] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "STATEMENT"}};
    itens[8] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "CLOSE_BRACE"}};
    itens[9] = (ItemTabelaSDT){ACAO, {.acao = ESCOPO_acao2}};
    itens[10] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "IFSTAT'"}};
    itens[11] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_if_action_s}};
}

static void inicializar_producao_IFSTAT_QUOTE_IDENT(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_empty_else_action}};
    itens[1] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "ε"}};
}

static void inicializar_producao_IFSTAT_QUOTE_ELSE(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_else_action_h}};
    itens[1] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "ELSE"}};
    itens[2] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "OPEN_BRACE"}};
    itens[3] = (ItemTabelaSDT){ACAO, {.acao = ESCOPO_acao6}};
    itens[4] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "STATEMENT"}};
    itens[5] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "CLOSE_BRACE"}};
    itens[6] = (ItemTabelaSDT){ACAO, {.acao = ESCOPO_acao2}};
    itens[7] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_else_action_s}};
}

static void inicializar_producao_FORSTAT_FOR(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_herdar_proximo}};
    itens[1] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_herdar_for_proximo}};
    itens[2] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "FOR"}};
    itens[3] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "OPEN_PARENTHESIS"}};
    itens[4] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "ATRIBSTAT"}};
    itens[5] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "SEMICOLON"}};
    itens[6] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "EXPRESSION"}};
    itens[7] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "SEMICOLON"}};
    itens[8] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "ATRIBSTAT"}};
    itens[9] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "CLOSE_PARENTHESIS"}};
    itens[10] = (ItemTabelaSDT){ACAO, {.acao = ESCOPO_acao7}};
    itens[11] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "STATEMENT"}};
    itens[12] = (ItemTabelaSDT){ACAO, {.acao = ESCOPO_acao2}};
    itens[13] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_for_action}};
}

static void inicializar_producao_STATELIST_IDENT(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_herdar_proximo}};
    itens[1] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_set_rotulo}};
    itens[2] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "STATEMENT"}};
    itens[3] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "STATELIST'"}};
    itens[4] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_obter_codigo_filhos_2}};
}

static void inicializar_producao_STATELIST_QUOTE_IDENT(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_herdar_proximo}};
    itens[1] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "STATELIST"}};
    itens[2] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_obter_codigo_filhos}};
}

static void inicializar_producao_STATELIST_QUOTE_CLOSE_BRACE(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_herdar_proximo}};
    itens[1] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "ε"}};
    itens[2] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_obter_codigo_filhos}};
}

static void inicializar_producao_ALLOCAUX_CLOSE_PARENTHESIS(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "ε"}};
}

static void inicializar_producao_ALLOCAUX_OPEN_BRACKET(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "OPEN_BRACKET"}};
    itens[1] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "NUMEXPRESSION"}};
    itens[2] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "CLOSE_BRACKET"}};
    itens[3] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "ALLOCAUX"}};
    itens[4] = (ItemTabelaSDT){ACAO, {.acao = EXPA_printExpression1}};
    itens[5] = (ItemTabelaSDT){ACAO, {.acao = EXPA_arrayCounter}};
}

static void inicializar_producao_ALLOCEXPRESSION_NEW(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "NEW"}};
    itens[1] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "ALLOCEXPRESSION'"}};
}

static void inicializar_producao_ALLOCEXPRESSION_QUOTE_INT(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "INT"}};
    itens[1] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "OPEN_BRACKET"}};
    itens[2] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "NUMEXPRESSION"}};
    itens[3] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "CLOSE_BRACKET"}};
    itens[4] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "ALLOCAUX"}};
    itens[5] = (ItemTabelaSDT){ACAO, {.acao = EXPA_printExpression2}};
}

static void inicializar_producao_ALLOCEXPRESSION_QUOTE_FLOAT(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "FLOAT"}};
    itens[1] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "OPEN_BRACKET"}};
    itens[2] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "NUMEXPRESSION"}};
    itens[3] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "CLOSE_BRACKET"}};
    itens[4] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "ALLOCAUX"}};
    itens[5] = (ItemTabelaSDT){ACAO, {.acao = EXPA_printExpression2}};
}

static void inicializar_producao_ALLOCEXPRESSION_QUOTE_STRING(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "STRING"}};
    itens[1] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "OPEN_BRACKET"}};
    itens[2] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "NUMEXPRESSION"}};
    itens[3] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "CLOSE_BRACKET"}};
    itens[4] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "ALLOCAUX"}};
    itens[5] = (ItemTabelaSDT){ACAO, {.acao = EXPA_printExpression2}};
}

static void inicializar_producao_EXPRESSION_IDENT(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "NUMEXPRESSION"}};
    itens[1] = (ItemTabelaSDT){ACAO, {.acao = EXPA_printExpression0_h}};
    itens[2] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "EXPRESSION'"}};
    itens[3] = (ItemTabelaSDT){ACAO, {.acao = EXPA_printExpression0}};
}

static void inicializar_producao_EXPRESSION_QUOTE_CLOSE_PARENTHESIS(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "ε"}};
}

static void inicializar_producao_EXPRESSION_QUOTE_RELOP(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "RELOP"}};
    itens[1] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "NUMEXPRESSION"}};
    itens[2] = (ItemTabelaSDT){ACAO, {.acao = EXPA_printExpression1}};
    itens[3] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_relop_action}};
}

static void inicializar_producao_NUMEXPRESSIONAUX_CLOSE_PARENTHESIS(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "ε"}};
}

static void inicializar_producao_NUMEXPRESSIONAUX_PLUS(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "PLUS"}};
    itens[1] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "NUMEXPRESSION"}};
    itens[2] = (ItemTabelaSDT){ACAO, {.acao = EXPA_setOperation}};
}

static void inicializar_producao_NUMEXPRESSIONAUX_MINUS(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "MINUS"}};
    itens[1] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "NUMEXPRESSION"}};
    itens[2] = (ItemTabelaSDT){ACAO, {.acao = EXPA_setOperation}};
}

static void inicializar_producao_NUMEXPRESSION_IDENT(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "TERM"}};
    itens[1] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "NUMEXPRESSIONAUX"}};
    itens[2] = (ItemTabelaSDT){ACAO, {.acao = EXPA_generateNode}};
}

static void inicializar_producao_TERM_IDENT(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "UNARYEXPR"}};
    itens[1] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "UNARYEXPRAUX"}};
    itens[2] = (ItemTabelaSDT){ACAO, {.acao = EXPA_term}};
}

static void inicializar_producao_UNARYEXPRAUX_CLOSE_PARENTHESIS(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "ε"}};
}

static void inicializar_producao_UNARYEXPRAUX_MULTIPLICATION(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "MULTIPLICATION"}};
    itens[1] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "TERM"}};
    itens[2] = (ItemTabelaSDT){ACAO, {.acao = EXPA_setOperation2}};
}

static void inicializar_producao_UNARYEXPRAUX_DIVISION(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "DIVISION"}};
    itens[1] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "TERM"}};
    itens[2] = (ItemTabelaSDT){ACAO, {.acao = EXPA_setOperation2}};
}

static void inicializar_producao_UNARYEXPRAUX_MODULUS(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "MODULUS"}};
    itens[1] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "TERM"}};
    itens[2] = (ItemTabelaSDT){ACAO, {.acao = EXPA_setOperation2}};
}

static void inicializar_producao_UNARYEXPR_IDENT(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "FACTOR"}};
    itens[1] = (ItemTabelaSDT){ACAO, {.acao = EXPA_valueUp}};
}

static void inicializar_producao_UNARYEXPR_PLUS(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "PLUS"}};
    itens[1] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "FACTOR"}};
    itens[2] = (ItemTabelaSDT){ACAO, {.acao = EXPA_secondChildValueUp}};
}

static void inicializar_producao_UNARYEXPR_MINUS(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "MINUS"}};
    itens[1] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "FACTOR"}};
    itens[2] = (ItemTabelaSDT){ACAO, {.acao = EXPA_secondChildValueUp}};
}

static void inicializar_producao_FACTOR_IDENT(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "LVALUE"}};
    itens[1] = (ItemTabelaSDT){ACAO, {.acao = EXPA_identUp}};
}

static void inicializar_producao_FACTOR_OPEN_PARENTHESIS(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "OPEN_PARENTHESIS"}};
    itens[1] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "NUMEXPRESSION"}};
    itens[2] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "CLOSE_PARENTHESIS"}};
    itens[3] = (ItemTabelaSDT){ACAO, {.acao = EXPA_passNum}};
}

static void inicializar_producao_FACTOR_NI(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "NI"}};
    itens[1] = (ItemTabelaSDT){ACAO, {.acao = EXPA_lexemeToValue}};
}

static void inicializar_producao_FACTOR_NPF(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "NPF"}};
    itens[1] = (ItemTabelaSDT){ACAO, {.acao = EXPA_lexemeToValue}};
}

static void inicializar_producao_FACTOR_STRC(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "STRC"}};
    itens[1] = (ItemTabelaSDT){ACAO, {.acao = EXPA_lexemeToValue}};
}

static void inicializar_producao_FACTOR_NULL(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "NULL"}};
}

#ifndef TABELA_SDT_AUXILIAR_H
#define TABELA_STD_AUXILIAR_H

#include "item_pilha_parser.h"
#include "acoes_semanticas.h"

typedef enum {
    ACAO,
    SIMBOLO
} TipoItemPilha;

typedef struct {
    TipoItemPilha tipo;
    union {
        void (*acao)(NoAST*, GerenciadorEscopos*);
        char* simbolo;
    } valor;
} ItemTabelaSDT;

typedef struct {
    char* chave;
    ItemTabelaSDT* itens;
    int quantidade;
} EntradaTabelaSDT;

typedef struct {
    EntradaTabelaSDT* entradas;
    int quantidade;
} TabelaSDT;

// Funções públicas
TabelaSDT* criar_tabela_sdt();
const ItemTabelaSDT* buscar_producao(const TabelaSDT* tabela, const char* nao_terminal, const char* token);
void liberar_tabela_sdt(TabelaSDT* tabela);

#endif

#include "tabela_simbolos.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define CAPACIDADE_INICIAL 16

TabelaSimbolos* criar_tabela_simbolos() {
    TabelaSimbolos* tabela = malloc(sizeof(TabelaSimbolos));
    tabela->capacidade = CAPACIDADE_INICIAL;
    tabela->tamanho = 0;
    tabela->entradas = malloc(CAPACIDADE_INICIAL * sizeof(EntradaTabela*));
    tabela->chaves = malloc(CAPACIDADE_INICIAL * sizeof(char*));
    return tabela;
}

static int buscar_indice(TabelaSimbolos* tabela, const char* lexema) {
    for (int i = 0; i < tabela->tamanho; i++) {
        if (strcmp(tabela->chaves[i], lexema) == 0) {
            return i;
        }
    }
    return -1;
}

static void redimensionar(TabelaSimbolos* tabela) {
    int nova_capacidade = tabela->capacidade * 2;
    EntradaTabela** novas_entradas = malloc(nova_capacidade * sizeof(EntradaTabela*));
    char** novas_chaves = malloc(nova_capacidade * sizeof(char*));
    
    for (int i = 0; i < tabela->tamanho; i++) {
        novas_entradas[i] = tabela->entradas[i];
        novas_chaves[i] = tabela->chaves[i];
    }
    
    free(tabela->entradas);
    free(tabela->chaves);
    
    tabela->entradas = novas_entradas;
    tabela->chaves = novas_chaves;
    tabela->capacidade = nova_capacidade;
}

void adicionar_simbolo(TabelaSimbolos* tabela, Token* token) {
    int indice = buscar_indice(tabela, token->lexema);
    
    // Símbolo já existe, apenas adiciona a linha
    if (indice != -1) {
        EntradaTabela* entrada = tabela->entradas[indice];
        entrada->num_linhas++;
        entrada->linhas = realloc(entrada->linhas, entrada->num_linhas * sizeof(int));
        entrada->linhas[entrada->num_linhas - 1] = token->linha;
        return;
    }
    
    // Tabela cheia, redimensionar
    if (tabela->tamanho >= tabela->capacidade) {
        redimensionar(tabela);
    }
    
    // Criar nova entrada
    EntradaTabela* nova_entrada = malloc(sizeof(EntradaTabela));
    nova_entrada->tipo = strdup("");
    nova_entrada->num_linhas = 1;
    nova_entrada->linhas = malloc(sizeof(int));
    nova_entrada->linhas[0] = token->linha;
    
    // Adicionar à tabela
    tabela->chaves[tabela->tamanho] = strdup(token->lexema);
    tabela->entradas[tabela->tamanho] = nova_entrada;
    tabela->tamanho++;
}

void definir_tipo_simbolo(TabelaSimbolos* tabela, Token* token, const char* tipo) {
    int indice = buscar_indice(tabela, token->lexema);
    if (indice == -1) return;
    
    if (tabela->entradas[indice]->tipo) {
        free(tabela->entradas[indice]->tipo);
    }
    tabela->entradas[indice]->tipo = strdup(tipo);
}

bool simbolo_existe(TabelaSimbolos* tabela, const char* lexema) {
    return buscar_indice(tabela, lexema) != -1;
}

bool simbolo_e_tipo(TabelaSimbolos* tabela, Token* token, const char* tipo) {
    int indice = buscar_indice(tabela, token->lexema);
    if (indice == -1) return false;
    
    return strcmp(tabela->entradas[indice]->tipo, tipo) == 0;
}

char* obter_tipo_simbolo(TabelaSimbolos* tabela, Token* token) {
    int indice = buscar_indice(tabela, token->lexema);
    if (indice == -1) {
        // Tratar erro conforme necessário
        return strdup("");
    }
    return strdup(tabela->entradas[indice]->tipo);
}

void imprimir_tabela(TabelaSimbolos* tabela, FILE* saida) {
    for (int i = 0; i < tabela->tamanho; i++) {
        fprintf(saida, "\tSímbolo: %s\n", tabela->chaves[i]);
        fprintf(saida, "\t\tTipo: %s\n", tabela->entradas[i]->tipo);
        
        fprintf(saida, "\t\tLinhas: [");
        for (int j = 0; j < tabela->entradas[i]->num_linhas; j++) {
            fprintf(saida, "%d", tabela->entradas[i]->linhas[j]);
            if (j < tabela->entradas[i]->num_linhas - 1) {
                fprintf(saida, ", ");
            }
        }
        fprintf(saida, "]\n");
    }
}

void liberar_entrada(EntradaTabela* entrada) {
    if (entrada) {
        free(entrada->tipo);
        free(entrada->linhas);
        free(entrada);
    }
}

void liberar_tabela_simbolos(TabelaSimbolos* tabela) {
    if (!tabela) return;
    
    for (int i = 0; i < tabela->tamanho; i++) {
        free(tabela->chaves[i]);
        liberar_entrada(tabela->entradas[i]);
    }
    
    free(tabela->entradas);
    free(tabela->chaves);
    free(tabela);
}

#ifndef TABELA_SIMBOLOS_H
#define TABELA_SIMBOLOS_H

#include "token.h"
#include <stdbool.h>

// Estrutura para uma entrada na tabela de símbolos
typedef struct {
    char* tipo;
    int* linhas;
    int num_linhas;
} EntradaTabela;

// Estrutura principal da tabela de símbolos
typedef struct {
    EntradaTabela** entradas;
    char** chaves;
    int capacidade;
    int tamanho;
} TabelaSimbolos;

// Cria uma nova tabela de símbolos
TabelaSimbolos* criar_tabela_simbolos();

// Adiciona um novo símbolo à tabela
void adicionar_simbolo(TabelaSimbolos* tabela, Token* token);

// Define o tipo de um símbolo
void definir_tipo_simbolo(TabelaSimbolos* tabela, Token* token, const char* tipo);

// Verifica se um lexema está na tabela
bool simbolo_existe(TabelaSimbolos* tabela, const char* lexema);

// Verifica se um símbolo é de um tipo específico
bool simbolo_e_tipo(TabelaSimbolos* tabela, Token* token, const char* tipo);

// Obtém o tipo de um símbolo
char* obter_tipo_simbolo(TabelaSimbolos* tabela, Token* token);

// Imprime o conteúdo da tabela
void imprimir_tabela(TabelaSimbolos* tabela, FILE* saida);

// Libera a memória da tabela
void liberar_tabela_simbolos(TabelaSimbolos* tabela);

#endif

#include "tipo_enum.h"
#include <stdio.h>
#include "erros.h" // Supondo que temos um header de erros

const char* tipo_para_string(Tipo tipo) {
    switch(tipo) {
        case TIPO_FUNCAO:    return "FUNCAO";
        case TIPO_INTEIRO:    return "INTEIRO";
        case TIPO_FLOAT:     return "FLOAT";
        case TIPO_STRING:    return "STRING";
        default:             
            erro_semantico("Tipo proibido");
            return "DESCONHECIDO";
    }
}

void imprimir_tipo(FILE* saida, Tipo tipo) {
    fprintf(saida, "%s", tipo_para_string(tipo));
}

#ifndef TIPO_ENUM_H
#define TIPO_ENUM_H

typedef enum {
    TIPO_FUNCAO,
    TIPO_INTEIRO,
    TIPO_FLOAT,
    TIPO_STRING
} Tipo;

// Função para converter enum em string
const char* tipo_para_string(Tipo tipo);

// Função para imprimir o tipo (equivalente ao operador <<)
void imprimir_tipo(FILE* saida, Tipo tipo);

#endif // TIPO_ENUM_H

#include "token.h"
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

// Cria um novo token
Token* criar_token(const char* lexema, const char* id, int linha, int coluna) {
    Token* token = (Token*)malloc(sizeof(Token));
    token->lexema = strdup(lexema);
    token->id = strdup(id);
    token->linha = linha;
    token->coluna = coluna;
    return token;
}

// Libera a memória de um token
void liberar_token(Token* token) {
    if (token) {
        free(token->lexema);
        free(token->id);
        free(token);
    }
}

// Verifica se um caractere é uma letra
bool eh_letra(char c) {
    return isalpha((unsigned char)c) || c == '_';
}

// Verifica se um caractere é um dígito
bool eh_digito(char c) {
    return isdigit((unsigned char)c);
}

// Verifica se um caractere é espaço em branco
bool eh_espaco_branco(char c) {
    return isspace((unsigned char)c);
}

#ifndef TOKEN_H
#define TOKEN_H

#include <stdbool.h>

// Estrutura para representar um token
typedef struct {
    char* lexema;
    char* id;
    int linha;
    int coluna;
} Token;

// Protótipos das funções
Token* criar_token(const char* lexema, const char* id, int linha, int coluna);
void liberar_token(Token* token);
bool eh_letra(char c);
bool eh_digito(char c);
bool eh_espaco_branco(char c);

#endif // TOKEN_H



