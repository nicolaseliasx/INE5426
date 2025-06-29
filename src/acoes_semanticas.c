#define _DEFAULT_SOURCE

#include "acoes_semanticas.h"

#include "gerenciador_escopo.h"
#include "no_ast.h"
#include "tabela_simbolos.h"
#include "token.h"
#include "resolvedor_expressao.h"
#include "erros.h" 

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

// Cria uma instância única do resolvedor para este arquivo.
static ResolvedorExpressao* resolvedor_global = NULL;

// Função para inicializar o resolvedor (chame no início do seu programa)
void inicializar_resolvedor_global() {
    if (resolvedor_global == NULL) {
        resolvedor_global = criar_resolvedor_expressao();
    }
}

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
    adicionar_string(no_pai->codigo, buffer);
}

void CODIGO_lidar_break(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // STATEMENT -> break ;
    // STATEMENT.code = goto STATEMENT.for_nxt [0]
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "goto %s", no_pai->para_proximo);
    adicionar_string(no_pai->codigo, buffer);
}

void CODIGO_lidar_leitura(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // READSTAT -> read LVALUE
    // READSTAT.code = read LVALUE.var [1]
    NoAST* lvalue = no_pai->filhos[1];
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "read %s", lvalue->res_var_codigo.var);
    adicionar_string(no_pai->codigo, buffer);
}

void CODIGO_lidar_impressao(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // PRINTSTAT -> print EXPRESSION
    // PRINTSTAT.code = EXPRESSION.code || print EXPRESSION.var [0]
    NoAST* expressao = no_pai->filhos[1];

    lista_codigo_adicionar_lista(no_pai->codigo, expressao->codigo);

    char buffer[256];
    snprintf(buffer, sizeof(buffer), "print %s", expressao->res_var_codigo.var);
    adicionar_string(no_pai->codigo, buffer);
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
    adicionar_string(no_pai->codigo, buffer);
}

void CODIGO_lidar_retorno2(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // RETURNSTAT' -> LVALUE
    // RETURNSTAT'.code = "$rv = LVALUE.var";
    NoAST* lvalue = no_pai->filhos[0];
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "$rv = %s", lvalue->res_var_codigo.var);
    adicionar_string(no_pai->codigo, buffer);
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
    adicionar_string(no_pai->codigo, buffer);
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
    adicionar_string(no_pai->codigo, buffer);

    snprintf(buffer, sizeof(buffer), "call %s %d",
             ident->token->lexema,
             paramlistcall->res_var_codigo.contador_param);
    adicionar_string(no_pai->codigo, buffer);

    snprintf(buffer, sizeof(buffer), "%s:", no_pai->res_var_codigo.ret);
    adicionar_string(no_pai->codigo, buffer);
}

void CODIGO_ir_para_principal(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // PROGRAM -> FUNCLIST PROGRAM'
    // PROGRAM.code -> goto MAIN || FUNCLIST.code || MAIN: || PROGRAM'.code [1]
    NoAST* funclist = no_pai->filhos[0];
    NoAST* program_linha = no_pai->filhos[1];

    adicionar_string(no_pai->codigo, strdup("goto MAIN"));
    lista_codigo_adicionar_lista(no_pai->codigo, funclist->codigo);
    adicionar_string(no_pai->codigo, strdup("MAIN:"));
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
    adicionar_string(no_pai->codigo, buffer);

    lista_codigo_adicionar_lista(no_pai->codigo, statelist->codigo);

    snprintf(buffer, sizeof(buffer), "goto %s", statelist->proximo);
    adicionar_string(no_pai->codigo, buffer);
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
    adicionar_string(no_pai->codigo, buffer);

    lista_codigo_adicionar_lista(no_pai->codigo, statement->codigo);

    snprintf(buffer, sizeof(buffer), "goto %s", statement->proximo);
    adicionar_string(no_pai->codigo, buffer);
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
    adicionar_string(no_pai->codigo, buffer);

    lista_codigo_adicionar_lista(no_pai->codigo, statement->codigo);

    snprintf(buffer, sizeof(buffer), "goto %s", statement->proximo);
    adicionar_string(no_pai->codigo, buffer);

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
    adicionar_string(no_pai->codigo, buffer);

    lista_codigo_adicionar_lista(no_pai->codigo, expressao->codigo);

    snprintf(buffer, sizeof(buffer), "if False %s goto %s",
             expressao->res_var_codigo.var,
             no_pai->proximo); // FORSTAT.next is no_pai->proximo
    adicionar_string(no_pai->codigo, buffer);

    lista_codigo_adicionar_lista(no_pai->codigo, statement->codigo);
    lista_codigo_adicionar_lista(no_pai->codigo, atribstat2->codigo);

    snprintf(buffer, sizeof(buffer), "goto %s", rotulo_inicio_laco);
    adicionar_string(no_pai->codigo, buffer);

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
        assign_code = buffer;
    } else {
        snprintf(buffer, sizeof(buffer), "%s = %s",
                 lvalue->filhos[0]->token->lexema, // Direct identifier lexeme
                 atribstat_linha->res_var_codigo.var);
        assign_code = buffer;
    }
    adicionar_string(no_pai->codigo, assign_code);
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
    for (size_t i = 0; i < no_pai->quantidade_filhos; i++) {
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
    adicionar_string(no_pai->codigo, buffer);

    lista_codigo_adicionar_lista(no_pai->codigo, statelist_linha->codigo);
}

void CODIGO_herdar_proximo(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // Generic action to inherit 'nxt' and 'for_nxt' attributes to children
    for (size_t i = 0; i < no_pai->quantidade_filhos; i++) {
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
    adicionar_string(no_pai->codigo, buffer);
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
    gerenciador_escopo_adicionar_simbolo(gerenciador, ident->token);
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
    gerenciador_definir_tipo_simbolo(gerenciador, ident->token, strdup("function"));
}

void DEC_acao2(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // PARAMLIST -> int ident PARAMLIST'
    // add_type(ident.val_lex, "int")
    NoAST* ident = no_pai->filhos[1];
    gerenciador_definir_tipo_simbolo(gerenciador, ident->token, strdup("int"));
}

void DEC_acao3(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // PARAMLIST -> float ident PARAMLIST'
    // add_type(ident.val_lex, "float")
    NoAST* ident = no_pai->filhos[1];
    gerenciador_definir_tipo_simbolo(gerenciador, ident->token, strdup("float"));
}

void DEC_acao4(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // PARAMLIST -> string ident PARAMLIST'
    // add_type(ident.val_lex, "string")
    NoAST* ident = no_pai->filhos[1];
    gerenciador_definir_tipo_simbolo(gerenciador, ident->token, strdup("string"));
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
    index->sdt_dec.tipo = buffer;
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
    gerenciador_definir_tipo_simbolo(gerenciador, ident->token, strdup(index->sdt_dec.tipo));
}

// =================================================================
// Implementações do namespace DECLARAR_VERIFICAR (Verificação de declarações)
// =================================================================

void DECLARAR_VERIFICAR_acao1(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // FUNCCALL -> call ( ident ( PARAMLISTCALL ) )
    // is_declared(ident.val_lex)
    // is_type(ident.val_lex, "function")
    NoAST* ident = no_pai->filhos[2];
    if (!gerenciador_simbolo_declarado(gerenciador, ident->token)) {
        char msg_error[512];
        snprintf(msg_error, sizeof(msg_error), "%s usado antes da declaracao na linha %d:%d",
                 ident->token->lexema, ident->token->linha, ident->token->coluna);
        // Assuming your 'erros.h' has a function like `criar_erro_semantico`
        criar_erro_semantico(msg_error);
    }
    if (!gerenciador_simbolo_e_tipo(gerenciador, ident->token, "function")) {
        char msg_error[512];
        snprintf(msg_error, sizeof(msg_error), "%s na linha %d:%d nao e uma funcao",
                 ident->token->lexema, ident->token->linha, ident->token->coluna);
        criar_erro_semantico(msg_error);
    }
}

void DECLARAR_VERIFICAR_acao2(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // PARAMLISTCALL -> ident PARAMLISTCALL'
    // LVALUE -> ident ALLOCAUX
    // is_declared(ident.val_lex)
    NoAST* ident = no_pai->filhos[0]; // Assuming ident is always the first child
    if (!gerenciador_simbolo_declarado(gerenciador, ident->token)) {
        char msg_error[512];
        snprintf(msg_error, sizeof(msg_error), "%s usado antes da declaracao na linha %d:%d",
                 ident->token->lexema, ident->token->linha, ident->token->coluna);
        criar_erro_semantico(msg_error);
    }
}

// =================================================================
// Implementações do namespace BREAK (Verificação de break)
// =================================================================

void BREAK_acao1(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // STATEMENT -> break ;
    if (!gerenciador_esta_no_escopo(gerenciador, "for")) {
        NoAST* break_t = no_pai->filhos[0];
        char msg_error[512];
        snprintf(msg_error, sizeof(msg_error), "break usado fora de um comando for na linha %d:%d",
                 break_t->token->linha, break_t->token->coluna);
        criar_erro_semantico(msg_error);
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
        criar_erro_semantico(msg_error);
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
        criar_erro_semantico("tentativa de operacoes com arrays com numero incorreto de dimensoes");
        free(final_tipo); // Free if it was allocated
        return NULL;
    }
    return final_tipo; // Caller must free this
}

int* AUXILIAR_obter_tamanhos_vetor(const char* tipo_str, int* num_elementos) {
    if (!tipo_str || !num_elementos) {
        *num_elementos = 0;
        return NULL;
    }

    // Aloca um buffer temporário para os tamanhos.
    // É improvável ter mais de, digamos, 32 dimensões em um array.
    int temp_values[32]; 
    int count = 0;
    
    const char* ptr = tipo_str;
    while (*ptr != '\0' && count < 32) {
        // Se encontrarmos um dígito, começamos a ler um número.
        if (*ptr >= '0' && *ptr <= '9') {
            // Usa strtol para converter a substring para um número longo.
            // O segundo argumento (endptr) nos dirá onde o número termina.
            char* endptr;
            long val = strtol(ptr, &endptr, 10);
            
            // Armazena o valor encontrado.
            temp_values[count++] = (int)val;
            
            // Avança o ponteiro principal para depois do número que acabamos de ler.
            ptr = endptr; 
        } else {
            // Se não for um dígito, apenas avança.
            ptr++;
        }
    }

    *num_elementos = count;
    if (count == 0) {
        return NULL; // Nenhum tamanho de dimensão foi encontrado.
    }

    // Aloca a memória exata necessária para os valores encontrados.
    int* values = (int*)malloc(sizeof(int) * count);
    if (values == NULL) {
        // Lidar com erro de alocação de memória.
        criar_erro_semantico("Erro de alocacao de memoria para tamanhos de vetor.");
        return NULL;
    }

    // Copia os valores do buffer temporário para o array final.
    memcpy(values, temp_values, sizeof(int) * count);

    return values; // IMPORTANTE: O chamador desta função é responsável por usar free(values)!
}


// =================================================================
// Implementações do namespace EXPA (Ações para expressões)
// =================================================================

void EXPA_avaliar_identificador(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // LVALUE -> IDENT ALLOCAUX
    NoAST* lvalue_node = no_pai;
    NoAST* ident_node = no_pai->filhos[0];
    NoAST* allocaux_node = no_pai->filhos[1];

    char* type_str = gerenciador_obter_tipo_simbolo(gerenciador, ident_node->token);
    int num_elements;
    int* max_dims = AUXILIAR_obter_tamanhos_vetor(type_str, &num_elements);

    lvalue_node->sdt_mat.contador_vetor = allocaux_node->sdt_mat.contador_vetor;

    lista_codigo_adicionar_lista(lvalue_node->codigo, allocaux_node->codigo);

    if (lvalue_node->sdt_mat.contador_vetor > 0) {
        // MUDANÇA: 'VetorStrings*' foi trocado por 'ListaString*', que é o tipo correto.
        ListaString* array_indexation_vars = allocaux_node->sdt_mat.vetor_array_var;

        // Supondo que gerar_variavel_temporaria(resolvedor_global) existe e aloca memória
        char* somador_temp_var = gerar_variavel_temporaria(NULL); // Ajuste se necessário
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "%s = 0", somador_temp_var);
        adicionar_string(lvalue_node->codigo, buffer);

        int counter = 0;
        // MUDANÇA: Usar '->tamanho' e '->itens[i]' para acessar a lista
        for (int i = 0; i < array_indexation_vars->tamanho; i++) {
            char* k_var = array_indexation_vars->itens[i];
            char* current_temp_var = gerar_variavel_temporaria(NULL);

            snprintf(buffer, sizeof(buffer), "%s = %s", current_temp_var, k_var);
            adicionar_string(lvalue_node->codigo, buffer);

            for (int j = 0; j < counter; ++j) {
                // Certifica que max_dims não está fora dos limites
                if (j < num_elements) {
                    snprintf(buffer, sizeof(buffer), "%s = %s * %d", current_temp_var, current_temp_var, max_dims[j]);
                    adicionar_string(lvalue_node->codigo, buffer);
                }
            }
            snprintf(buffer, sizeof(buffer), "%s = %s + %s", somador_temp_var, somador_temp_var, current_temp_var);
            adicionar_string(lvalue_node->codigo, buffer);
            
            free(current_temp_var); // Libera a variável temporária do loop interno
            counter++;
        }

        snprintf(buffer, sizeof(buffer), "%s[%s]", ident_node->token->lexema, somador_temp_var);
        // Supondo que criar_no_expressao_simples foi substituído por criar_no_expressao_simples/basico
        lvalue_node->sdt_mat.no = criar_no_expressao_simples('n', type_str, buffer);
        
        free(somador_temp_var); // Libera a variável temporária do somador
    } else {
        lvalue_node->sdt_mat.no = criar_no_expressao_simples('n', type_str, ident_node->token->lexema);
    }

    free(type_str);
    free(max_dims);
}

void EXPA_lexema_para_valor(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // FACTOR -> int_constant | float_constant | string_constant
    // Factor.Node = new Node(value)
    NoAST* factor_node = no_pai;
    NoAST* constant_node = no_pai->filhos[0];
    char* type_str = NULL;

    // CORREÇÃO: Trocar 'id' por 'identificador' para corresponder à definição da struct.
    if (strcmp(constant_node->identificador, "NI") == 0) {
        type_str = strdup("int");
    } else if (strcmp(constant_node->identificador, "NPF") == 0) {
        type_str = strdup("float");
    } else if (strcmp(constant_node->identificador, "STRC") == 0) {
        type_str = strdup("string");
    }
    
    // Libera a memória de type_str se ela foi alocada antes de uma nova atribuição
    // (embora neste caso seja seguro, é uma boa prática).
    if (factor_node->sdt_mat.no && factor_node->sdt_mat.no->tipo) {
        free(type_str);
    }

    factor_node->sdt_mat.no = criar_no_expressao_simples('n', type_str, constant_node->token->lexema);
    
    // O tipo agora pertence ao nó da expressão, não precisamos mais da cópia local.
    free(type_str);
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

    unaryexpr_node->res_var_codigo.var = gerar_variavel_temporaria(resolvedor_global);

    char buffer[256];
    snprintf(buffer, sizeof(buffer), "%s = %c%s",
             unaryexpr_node->res_var_codigo.var,
             symbol_node->token->lexema[0],
             factor_node->sdt_mat.no->valor);
    adicionar_string(unaryexpr_node->codigo, buffer);

    // Create a new node representing the unary operation
    unaryexpr_node->sdt_mat.no = criar_no_expressao_unario(symbol_node->token->lexema[0], "N", factor_node->sdt_mat.no);
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
            criar_erro_semantico("Incompatibilidade de tipos em operacao aritmetica/logica.");
        }
        free(term_type);
        free(aux_type);

        char* varA = (term_node->res_var_codigo.var != NULL && strlen(term_node->res_var_codigo.var) > 0) ? strdup(term_node->res_var_codigo.var) : strdup(term_node->sdt_mat.no->valor);
        char* varB = (numexpression_aux_node->res_var_codigo.var != NULL && strlen(numexpression_aux_node->res_var_codigo.var) > 0) ? strdup(numexpression_aux_node->res_var_codigo.var) : strdup(numexpression_aux_node->sdt_mat.no->valor);

        lista_codigo_adicionar_lista(numexpression_node->codigo, term_node->codigo);
        lista_codigo_adicionar_lista(numexpression_node->codigo, numexpression_aux_node->codigo);

        numexpression_node->res_var_codigo.var = gerar_variavel_temporaria(resolvedor_global);
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "%s = %s %c %s",
                 numexpression_node->res_var_codigo.var,
                 varA,
                 numexpression_aux_node->sdt_mat.operacao,
                 varB);
        adicionar_string(numexpression_node->codigo, buffer);
        free(varA);
        free(varB);

        numexpression_node->sdt_mat.no = criar_no_expressao_binario(
            numexpression_aux_node->sdt_mat.operacao,
            "N",
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

        term_node->res_var_codigo.var = gerar_variavel_temporaria(resolvedor_global);
        char buffer[256];
        char* varA_val = (unaryexpr_node->res_var_codigo.var != NULL && strlen(unaryexpr_node->res_var_codigo.var) > 0) ?
                         strdup(unaryexpr_node->res_var_codigo.var) : strdup(unaryexpr_node->sdt_mat.no->valor);

        // Handle array dereferencing if it's an array and not fully indexed
        if (unaryexpr_node->sdt_mat.contador_vetor > 0) {
            snprintf(buffer, sizeof(buffer), "%s = %s", term_node->res_var_codigo.var, varA_val);
        } else {
            snprintf(buffer, sizeof(buffer), "%s = %s", term_node->res_var_codigo.var, varA_val);
        }
        adicionar_string(term_node->codigo, buffer);
        free(varA_val);

    } else {
        char* unary_type = AUXILIAR_obter_tipo(unaryexpr_node->sdt_mat.no->tipo, unaryexpr_node->sdt_mat.contador_vetor);
        char* aux_type = AUXILIAR_obter_tipo(unaryexpr_aux_node->sdt_mat.no->tipo, unaryexpr_aux_node->sdt_mat.contador_vetor);

        // Type checking
        if (strcmp(unary_type, aux_type) != 0 &&
            !( (strcmp(unary_type, "int") == 0 && strcmp(aux_type, "float") == 0) ||
               (strcmp(unary_type, "float") == 0 && strcmp(aux_type, "int") == 0) ) ) {
            criar_erro_semantico("Incompatibilidade de tipos em operacao aritmetica/logica.");
        }
        free(unary_type);
        free(aux_type);

        char* varA = (unaryexpr_node->res_var_codigo.var != NULL && strlen(unaryexpr_node->res_var_codigo.var) > 0) ? strdup(unaryexpr_node->res_var_codigo.var) : strdup(unaryexpr_node->sdt_mat.no->valor);
        char* varB = (unaryexpr_aux_node->res_var_codigo.var != NULL && strlen(unaryexpr_aux_node->res_var_codigo.var) > 0) ? strdup(unaryexpr_aux_node->res_var_codigo.var) : strdup(unaryexpr_aux_node->sdt_mat.no->valor);

        lista_codigo_adicionar_lista(term_node->codigo, unaryexpr_node->codigo);
        lista_codigo_adicionar_lista(term_node->codigo, unaryexpr_aux_node->codigo);

        term_node->res_var_codigo.var = gerar_variavel_temporaria(resolvedor_global);
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "%s = %s %c %s",
                 term_node->res_var_codigo.var,
                 varA,
                 unaryexpr_aux_node->sdt_mat.operacao,
                 varB);
        adicionar_string(term_node->codigo, buffer);
        free(varA);
        free(varB);

        term_node->sdt_mat.no = criar_no_expressao_binario(
            unaryexpr_aux_node->sdt_mat.operacao,
            "N",
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

    // This calls a C function `imprimir_arvore`
    imprimir_arvore(numexpression_node->sdt_mat.no);
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
    imprimir_arvore(numexpression_node->sdt_mat.no);
}

void EXPA_imprimir_expressao2(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // ALLOCEXPRESSION' -> TYPE OPEN_BRACKET NUMEXPRESSION CLOSE_BRACKET ALLOCAUX
    NoAST* numexpression_node = no_pai->filhos[2];
    imprimir_arvore(numexpression_node->sdt_mat.no);
}

void EXPA_contador_vetor(NoAST* no_pai, GerenciadorEscopo* gerenciador) {
    // ALLOCAUX -> [ NUMEXPRESSION ] ALLOCAUX1
    NoAST* allocaux_node = no_pai;
    NoAST* numexpression_node = no_pai->filhos[1];
    NoAST* allocaux1_node = no_pai->filhos[3];

    // Ensure vetor_array_var is initialized
    if (allocaux_node->sdt_mat.vetor_array_var == NULL) {
        // CORREÇÃO: Usando a função que realmente existe
        allocaux_node->sdt_mat.vetor_array_var = criar_lista_string();
    }
    
    // CORREÇÃO: Usando a função que realmente existe
    adicionar_string(allocaux_node->sdt_mat.vetor_array_var, strdup(numexpression_node->res_var_codigo.var));
    
    // CORREÇÃO: Usando a função que realmente existe para juntar as listas
    lista_codigo_adicionar_lista(allocaux_node->sdt_mat.vetor_array_var, allocaux1_node->sdt_mat.vetor_array_var); // Splice the vector

    allocaux_node->sdt_mat.contador_vetor = allocaux1_node->sdt_mat.contador_vetor + 1;

    lista_codigo_adicionar_lista(allocaux_node->codigo, numexpression_node->codigo);
    lista_codigo_adicionar_lista(allocaux_node->codigo, allocaux1_node->codigo);
}
