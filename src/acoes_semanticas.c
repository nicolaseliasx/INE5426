#define _DEFAULT_SOURCE

#include "acoes_semanticas.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "erros.h"
#include "gerenciador_escopo.h"
#include "no_ast.h"
#include "resolvedor_expressao.h"
#include "tabela_simbolos.h"
#include "token.h"

// Cria uma instância única do resolvedor para este arquivo.
static ResolvedorExpressao* resolvedor_global = NULL;

// Função para inicializar o resolvedor (chame no início do seu programa)
void inicializar_resolvedor_global()
{
    if (resolvedor_global == NULL)
    {
        resolvedor_global = criar_resolvedor_expressao();
    }
}

// Garante que o primeiro operando de uma expressão esteja em uma variável.
static char* garantir_primeiro_operando_em_var(NoAST* node, ListaString* codigo)
{
    if (node->res_var_codigo.var && strlen(node->res_var_codigo.var) > 0)
    {
        return strdup(node->res_var_codigo.var);
    }

    const char* valor = node->sdt_mat.no->valor;
    if (isalpha(valor[0]) || valor[0] == '$')
    {
        return strdup(valor);
    }

    char* nova_var = gerar_variavel_temporaria(resolvedor_global);
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "%s = %s", nova_var, valor);
    adicionar_string(codigo, buffer);
    return nova_var;
}

// Obtém o valor de um operando à direita, que pode ser um literal ou variável.
static char* obter_valor_rhs(NoAST* node)
{
    if (node->res_var_codigo.var && strlen(node->res_var_codigo.var) > 0)
    {
        return strdup(node->res_var_codigo.var);
    }
    return strdup(node->sdt_mat.no->valor);
}

// =================================================================
// Implementações do namespace CODIGO (Geração de código)
// =================================================================

void CODIGO_relop_action(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
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
    snprintf(buffer, sizeof(buffer), "%s = %s %s %s", no_pai->res_var_codigo.var,
             no_pai->res_var_codigo.var, relop->token->lexema, numexpression->res_var_codigo.var);
    adicionar_string(no_pai->codigo, buffer);
}

void CODIGO_lidar_break(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    // STATEMENT -> break ;
    // STATEMENT.code = goto STATEMENT.for_nxt [0]
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "goto %s", no_pai->para_proximo);
    adicionar_string(no_pai->codigo, buffer);
}

void CODIGO_lidar_leitura(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    // READSTAT -> read LVALUE
    // READSTAT.code = read LVALUE.var [1]
    NoAST* lvalue = no_pai->filhos[1];
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "read %s", lvalue->res_var_codigo.var);
    adicionar_string(no_pai->codigo, buffer);
}

void CODIGO_lidar_impressao(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    // PRINTSTAT -> print EXPRESSION
    // PRINTSTAT.code = EXPRESSION.code || print EXPRESSION.var [0]
    NoAST* expressao = no_pai->filhos[1];

    lista_codigo_adicionar_lista(no_pai->codigo, expressao->codigo);

    char buffer[256];
    snprintf(buffer, sizeof(buffer), "print %s", expressao->res_var_codigo.var);
    adicionar_string(no_pai->codigo, buffer);
}

void CODIGO_obter_variavel_lvalue(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    // LVALUE -> ident ALLOCAUX
    // LVALUE.var = ident.lexema [0]
    NoAST* ident = no_pai->filhos[0];
    no_pai->res_var_codigo.var = strdup(ident->token->lexema);
}

void CODIGO_obter_valor_retorno(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    // ATRIBSTAT' -> FUNCCALL
    no_pai->res_var_codigo.var = strdup("$rv");
}

void CODIGO_lidar_retorno(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    if (no_pai->filhos[1] && no_pai->filhos[1]->codigo)
    {
        lista_codigo_adicionar_lista(no_pai->codigo, no_pai->filhos[1]->codigo);
    }

    if (no_pai->proximo)
    {
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "goto %s", no_pai->proximo);
        adicionar_string(no_pai->codigo, buffer);
    }
}

void CODIGO_lidar_retorno2(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    // RETURNSTAT' -> EXPRESSION
    if (no_pai->filhos[0]->res_var_codigo.var)
    {
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "$rv = %s", no_pai->filhos[0]->res_var_codigo.var);
        adicionar_string(no_pai->codigo, buffer);
    }
}

void CODIGO_obter_parametros(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    // PARAMLISTCALL' -> , PARAMLISTCALL
    // PARAMLISTCALL'.cnt = PARAMLISTCALL.cnt [0]
    NoAST* paramlist = no_pai->filhos[1];
    no_pai->res_var_codigo.contador_param = paramlist->res_var_codigo.contador_param;
}

void CODIGO_chamada_paramlist(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
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

void CODIGO_chamada_funcao(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    // FUNCCALL -> call ( ident ( PARAMLISTCALL ) )
    NoAST* ident = no_pai->filhos[2];
    NoAST* paramlistcall = no_pai->filhos[4];

    no_pai->res_var_codigo.ret = no_ast_gerar_novo_rotulo(no_pai);

    lista_codigo_adicionar_lista(no_pai->codigo, paramlistcall->codigo);

    char buffer[256];  // Declarado uma única vez

    // 1. Guardar endereço de retorno
    snprintf(buffer, sizeof(buffer), "$r = %s", no_pai->res_var_codigo.ret);
    adicionar_string(no_pai->codigo, buffer);

    // 2. Chamar função
    snprintf(buffer, sizeof(buffer), "call %s %d", ident->token->lexema,
             paramlistcall->res_var_codigo.contador_param);
    adicionar_string(no_pai->codigo, buffer);

    // 3. Rótulo de retorno
    snprintf(buffer, sizeof(buffer), "%s:", no_pai->res_var_codigo.ret);
    adicionar_string(no_pai->codigo, buffer);

    // 4. Atribuir valor de retorno
    snprintf(buffer, sizeof(buffer), "%s = $rv", no_pai->res_var_codigo.var);
    adicionar_string(no_pai->codigo, buffer);
}

void CODIGO_ir_para_principal(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    // PROGRAM -> FUNCLIST PROGRAM'
    // PROGRAM.code -> goto MAIN || FUNCLIST.code || MAIN: || PROGRAM'.code [1]
    NoAST* funclist = no_pai->filhos[0];
    NoAST* program_linha = no_pai->filhos[1];

    adicionar_string(no_pai->codigo, strdup("goto MAIN"));
    lista_codigo_adicionar_lista(no_pai->codigo, funclist->codigo);
    adicionar_string(no_pai->codigo, strdup("MAIN:"));
    lista_codigo_adicionar_lista(no_pai->codigo, program_linha->codigo);
}

void CODIGO_funcdef_h(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    // FUNCDEF -> def ident ( PARAMLIST ) { STATELIST }
    // begin = newlabel
    // STATELIST.nxt = 'return'
    // FUNCDEF.cod = begin: || STATELIST.code || goto return
    NoAST* ident = no_pai->filhos[1];
    no_pai->res_var_codigo.inicio = strdup(ident->token->lexema);
    NoAST* statelist = no_pai->filhos[6];
    statelist->proximo = strdup("$r");
}

void CODIGO_funcdef_s(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
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

void CODIGO_acao_else_vazio(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    // IFSTAT' -> '' (epsilon)
    // IFSTAT'.begin = IFSTAT'.nxt
    no_pai->res_var_codigo.inicio = strdup(no_pai->proximo);
}

void CODIGO_acao_else_h(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    // IFSTAT' -> else { STATEMENT }
    // begin = newlabel()
    // STATEMENT.nxt = IFSTAT'.nxt
    // IFSTAT'.code = begin: || statement.code || goto STATEMENT.nxt
    NoAST* statement = no_pai->filhos[2];
    statement->proximo = strdup(no_pai->proximo);
    no_pai->res_var_codigo.inicio = no_ast_gerar_novo_rotulo(no_pai);
}

void CODIGO_acao_else_s(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
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

void CODIGO_acao_if_s(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
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
    snprintf(buffer, sizeof(buffer), "if False %s goto %s", expressao->res_var_codigo.var,
             ifstat_linha->res_var_codigo.inicio);
    adicionar_string(no_pai->codigo, buffer);

    lista_codigo_adicionar_lista(no_pai->codigo, statement->codigo);

    snprintf(buffer, sizeof(buffer), "goto %s", statement->proximo);
    adicionar_string(no_pai->codigo, buffer);

    lista_codigo_adicionar_lista(no_pai->codigo, ifstat_linha->codigo);
}

void CODIGO_acao_for(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
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

    snprintf(buffer, sizeof(buffer), "if False %s goto %s", expressao->res_var_codigo.var,
             no_pai->proximo);  // FORSTAT.next is no_pai->proximo
    adicionar_string(no_pai->codigo, buffer);

    lista_codigo_adicionar_lista(no_pai->codigo, statement->codigo);
    lista_codigo_adicionar_lista(no_pai->codigo, atribstat2->codigo);

    snprintf(buffer, sizeof(buffer), "goto %s", rotulo_inicio_laco);
    adicionar_string(no_pai->codigo, buffer);
}

void CODIGO_herdar_proximo_for(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    // FORSTAT -> for ( ATRIBSTAT1 ; EXPRESSION ; ATRIBSTAT2 ) STATEMENT
    NoAST* statement = no_pai->filhos[8];
    statement->para_proximo = strdup(no_pai->proximo);  // Inherit for_nxt from father
}

void CODIGO_atribuicao(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    // ATRIBSTAT -> LVALUE = ATRIBSTAT'
    // ATRIBSTAT.code = ATRSBSTAT'.code || LVALUE.lexema = ATRIBSTAT'.var
    NoAST* lvalue = no_pai->filhos[0];
    NoAST* atribstat_linha = no_pai->filhos[2];
    char buffer[256];
    char* assign_code;

    // Splice code from lvalue and atribstat_linha
    lista_codigo_adicionar_lista(no_pai->codigo, lvalue->codigo);
    lista_codigo_adicionar_lista(no_pai->codigo, atribstat_linha->codigo);

    if (lvalue->sdt_mat.contador_vetor > 0)
    {
        // Assuming mathSDT.node->value holds the calculated array address
        // This needs a proper way to get the resolved array address.
        // For simplicity, directly using value for now.
        snprintf(buffer, sizeof(buffer), "%s = %s",
                 lvalue->sdt_mat.no->valor,  // This might need more resolution
                 atribstat_linha->res_var_codigo.var);
        assign_code = buffer;
    }
    else
    {
        snprintf(buffer, sizeof(buffer), "%s = %s",
                 lvalue->filhos[0]->token->lexema,  // Direct identifier lexeme
                 atribstat_linha->res_var_codigo.var);
        assign_code = buffer;
    }
    adicionar_string(no_pai->codigo, assign_code);
}

void CODIGO_definir_valor_expressao(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    NoAST* expressao = no_pai->filhos[0];
    no_pai->res_var_codigo.var = strdup(expressao->res_var_codigo.var);

    if (no_pai->codigo)
    {
        liberar_lista_string(no_pai->codigo);
    }

    no_pai->codigo = expressao->codigo;
    expressao->codigo = NULL;
}

void CODIGO_obter_codigo_filhos(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    // Generic action to splice code from all children to the father
    for (size_t i = 0; i < no_pai->quantidade_filhos; i++)
    {
        lista_codigo_adicionar_lista(no_pai->codigo, no_pai->filhos[i]->codigo);
    }
}

void CODIGO_combinar_codigo_para_lista(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    if (no_pai->quantidade_filhos != 2) return;

    NoAST* statement = no_pai->filhos[0];
    NoAST* statelist_prime = no_pai->filhos[1];

    // Inicializa código do nó pai se necessário
    if (!no_pai->codigo)
    {
        no_pai->codigo = criar_lista_string();
    }

    // 1. Adiciona código do statement
    lista_codigo_adicionar_lista(no_pai->codigo, statement->codigo);

    // 2. Adiciona APENAS o rótulo de próximo do statement
    if (statement->proximo)
    {
        char rotulo[256];
        snprintf(rotulo, sizeof(rotulo), "%s:", statement->proximo);
        adicionar_string(no_pai->codigo, rotulo);
    }

    // 3. Adiciona código do restante da lista (STATELIST')
    lista_codigo_adicionar_lista(no_pai->codigo, statelist_prime->codigo);
}

void CODIGO_herdar_proximo(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    // Generic action to inherit 'nxt' and 'for_nxt' attributes to children
    for (size_t i = 0; i < no_pai->quantidade_filhos; i++)
    {
        if (no_pai->filhos[i])
        {  // Check for non-NULL child
            if (no_pai->proximo)
            {
                no_pai->filhos[i]->proximo = strdup(no_pai->proximo);
            }
            if (no_pai->para_proximo)
            {
                no_pai->filhos[i]->para_proximo = strdup(no_pai->para_proximo);
            }
        }
    }
}

void CODIGO_definir_proximo_filho(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    if (no_pai->quantidade_filhos > 0)
    {
        NoAST* statement = no_pai->filhos[0];

        // Libera rótulo anterior se existir
        if (statement->proximo)
        {
            free(statement->proximo);
        }

        // Gera novo rótulo para o final deste statement
        statement->proximo = no_ast_gerar_novo_rotulo(no_pai);
    }
}

void CODIGO_definir_rotulo_final(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    no_pai->proximo = strdup("END");
}

void CODIGO_gerar_codigo_final(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "%s:", no_pai->proximo);
    adicionar_string(no_pai->codigo, buffer);
}

void CODIGO_imprimir_codigo(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    // Print each line of code
    // Assuming lista_codigo_imprimir_tudo exists
    // Otherwise, iterate through no_pai->codigo and print each string
    lista_codigo_imprimir_tudo(no_pai->codigo);
}

// =================================================================
// Implementações do namespace ESCOPO (Gerenciamento de escopos)
// =================================================================

void ESCOPO_acao1(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    // PROGRAM  ->  STATEMENT
    // PROGRAM  ->  FUNCLIST PROGRAM'
    // PROGRAM  ->  ''
    // STATEMENT  ->  { STATELIST }
    gerenciador_abrir_escopo(gerenciador, "");  // Default scope type
}

void ESCOPO_acao2(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    // FUNCDEF  ->  def ident ( PARAMLIST ) { STATELIST }
    // STATEMENT  ->  { STATELIST }
    // IFSTAT  -> if ( EXPRESSION ) { STATEMENT } IFSTAT'
    // IFSTAT' -> else { STATEMENT }
    // FORSTAT  ->  for ( ATRIBSTAT ; EXPRESSION ; ATRIBSTAT ) STATEMENT
    gerenciador_fechar_escopo(gerenciador);
}

void ESCOPO_acao3(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
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

void ESCOPO_acao4(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    // FUNCDEF  ->  def ident ( PARAMLIST ) { STATELIST }
    gerenciador_abrir_escopo(gerenciador, "function");
}

void ESCOPO_acao5(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    // IFSTAT  -> if ( EXPRESSION ) { STATEMENT } IFSTAT'
    gerenciador_abrir_escopo(gerenciador, "if");
}

void ESCOPO_acao6(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    // IFSTAT' -> else { STATEMENT }
    gerenciador_abrir_escopo(gerenciador, "else");
}

void ESCOPO_acao7(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    // FORSTAT  ->  for ( ATRIBSTAT ; EXPRESSION ; ATRIBSTAT ) STATEMENT
    gerenciador_abrir_escopo(gerenciador, "for");
}

// =================================================================
// Implementações do namespace DEC (Declarações de tipos)
// =================================================================

void DEC_acao1(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    // FUNCDEF -> def ident ( PARAMLIST ) { STATELIST }
    // add_type(ident.val_lex, "function")
    NoAST* ident = no_pai->filhos[1];
    gerenciador_definir_tipo_simbolo(gerenciador, ident->token, strdup("function"));
}

void DEC_acao2(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    // PARAMLIST -> int ident PARAMLIST'
    // add_type(ident.val_lex, "int")
    NoAST* ident = no_pai->filhos[1];
    gerenciador_definir_tipo_simbolo(gerenciador, ident->token, strdup("int"));
}

void DEC_acao3(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    // PARAMLIST -> float ident PARAMLIST'
    // add_type(ident.val_lex, "float")
    NoAST* ident = no_pai->filhos[1];
    gerenciador_definir_tipo_simbolo(gerenciador, ident->token, strdup("float"));
}

void DEC_acao4(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    // PARAMLIST -> string ident PARAMLIST'
    // add_type(ident.val_lex, "string")
    NoAST* ident = no_pai->filhos[1];
    gerenciador_definir_tipo_simbolo(gerenciador, ident->token, strdup("string"));
}

void DEC_acao5(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    // INDEX -> [int_constant] INDEX1
    // INDEX1.initial_type = INDEX.initial_type
    NoAST* index = no_pai;
    NoAST* index1 = no_pai->filhos[3];
    index1->sdt_dec.tipo_inicial = strdup(index->sdt_dec.tipo_inicial);
}

void DEC_acao6(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    NoAST* index = no_pai;
    NoAST* index1 = no_pai->filhos[3];
    NoAST* int_const = no_pai->filhos[1];
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "array(%s, %s)", int_const->token->lexema,
             index1->sdt_dec.tipo);
    index->sdt_dec.tipo = strdup(buffer);
}

void DEC_acao7(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    // INDEX-> '' (epsilon)
    // INDEX.type = INDEX.initial_type
    NoAST* index = no_pai;
    index->sdt_dec.tipo = strdup(index->sdt_dec.tipo_inicial);
}

void DEC_acao8(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    // VARDECL -> int ident INDEX
    // INDEX.initial_type = "int"
    NoAST* index = no_pai->filhos[2];
    index->sdt_dec.tipo_inicial = strdup("int");
}

void DEC_acao9(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    // VARDECL -> float ident INDEX
    // INDEX.initial_type = "float"
    NoAST* index = no_pai->filhos[2];
    index->sdt_dec.tipo_inicial = strdup("float");
}

void DEC_acao10(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    // VARDECL -> string ident INDEX
    // INDEX.initial_type = "string"
    NoAST* index = no_pai->filhos[2];
    index->sdt_dec.tipo_inicial = strdup("string");
}

void DEC_acao11(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
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

void DECLARAR_VERIFICAR_acao1(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    // FUNCCALL -> call ( ident ( PARAMLISTCALL ) )
    // is_declared(ident.val_lex)
    // is_type(ident.val_lex, "function")
    NoAST* ident = no_pai->filhos[2];
    if (!gerenciador_simbolo_declarado(gerenciador, ident->token))
    {
        char msg_error[512];
        snprintf(msg_error, sizeof(msg_error), "%s usado antes da declaracao na linha %d:%d",
                 ident->token->lexema, ident->token->linha, ident->token->coluna);
        // Assuming your 'erros.h' has a function like `LANCAR_ERRO_SEMANTICO`
        LANCAR_ERRO_SEMANTICO(msg_error);
    }
    if (!gerenciador_simbolo_e_tipo(gerenciador, ident->token, "function"))
    {
        char msg_error[512];
        snprintf(msg_error, sizeof(msg_error), "%s na linha %d:%d nao e uma funcao",
                 ident->token->lexema, ident->token->linha, ident->token->coluna);
        LANCAR_ERRO_SEMANTICO(msg_error);
    }
}

void DECLARAR_VERIFICAR_acao2(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    // PARAMLISTCALL -> ident PARAMLISTCALL'
    // LVALUE -> ident ALLOCAUX
    // is_declared(ident.val_lex)
    NoAST* ident = no_pai->filhos[0];  // Assuming ident is always the first child
    if (!gerenciador_simbolo_declarado(gerenciador, ident->token))
    {
        char msg_error[512];
        snprintf(msg_error, sizeof(msg_error), "%s usado antes da declaracao na linha %d:%d",
                 ident->token->lexema, ident->token->linha, ident->token->coluna);
        LANCAR_ERRO_SEMANTICO(msg_error);
    }

    gerenciador_registrar_uso_simbolo(gerenciador, ident->token);
}

// =================================================================
// Implementações do namespace BREAK (Verificação de break)
// =================================================================

void BREAK_acao1(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    // STATEMENT -> break ;
    if (!gerenciador_esta_no_escopo(gerenciador, "for"))
    {
        NoAST* break_t = no_pai->filhos[0];
        char msg_error[512];
        snprintf(msg_error, sizeof(msg_error), "break usado fora de um comando for na linha %d:%d",
                 break_t->token->linha, break_t->token->coluna);
        LANCAR_ERRO_SEMANTICO(msg_error);
    }
}

// =================================================================
// Implementações do namespace AUXILIAR (Funções auxiliares)
// =================================================================

char* AUXILIAR_obter_tipo(const char* tipo_str, int contador_vetor)
{
    const char* f = tipo_str;
    const char* ultimo_tipo_encontrado = NULL;
    while (*f != '\0')
    {
        if (*f == 'i' && strncmp(f, "int", 3) == 0)
        {
            ultimo_tipo_encontrado = "int";
        }
        else if (*f == 'f' && strncmp(f, "float", 5) == 0)
        {
            ultimo_tipo_encontrado = "float";
        }
        else if (*f == 's' && strncmp(f, "string", 6) == 0)
        {
            ultimo_tipo_encontrado = "string";
        }
        f++;
    }

    if (ultimo_tipo_encontrado)
    {
        return strdup(ultimo_tipo_encontrado);
    }
    return strdup(tipo_str);
}

int* AUXILIAR_obter_tamanhos_vetor(const char* tipo_str, int* num_elementos)
{
    if (!tipo_str || !num_elementos)
    {
        *num_elementos = 0;
        return NULL;
    }

    int temp_values[32];  // Buffer temporário
    int count = 0;
    const char* ptr = tipo_str;

    while (*ptr != '\0' && count < 32)
    {
        if (*ptr >= '0' && *ptr <= '9')
        {
            // Itera dígito por dígito, imitando o bug do C++
            temp_values[count++] = *ptr - '0';
        }
        ptr++;
    }

    *num_elementos = count;
    if (count == 0)
    {
        return NULL;
    }

    int* values = (int*)malloc(sizeof(int) * count);
    if (!values) return NULL;
    memcpy(values, temp_values, sizeof(int) * count);

    return values;
}

// =================================================================
// Implementações do namespace EXPA (Ações para expressões)
// =================================================================
void EXPA_lexema_para_valor(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    // FACTOR -> int_constant | float_constant | string_constant
    // Factor.Node = new Node(value)
    NoAST* factor_node = no_pai;
    NoAST* constant_node = no_pai->filhos[0];
    char* type_str = NULL;

    if (strcmp(constant_node->identificador, "NI") == 0)
    {
        type_str = strdup("int");
    }
    else if (strcmp(constant_node->identificador, "NPF") == 0)
    {
        type_str = strdup("float");
    }
    else if (strcmp(constant_node->identificador, "STRC") == 0)
    {
        type_str = strdup("string");
    }

    if (factor_node->sdt_mat.no && factor_node->sdt_mat.no->tipo)
    {
        free(factor_node->sdt_mat.no->tipo);
    }

    factor_node->sdt_mat.no =
        criar_no_expressao_simples('n', type_str, constant_node->token->lexema);

    // O tipo agora pertence ao nó da expressão, não precisamos mais da cópia local.
    free(type_str);
}

void EXPA_definir_operacao(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    NoAST* numexpr_aux_node = no_pai;
    NoAST* operation_node = no_pai->filhos[0];
    NoAST* numexpression_node = no_pai->filhos[1];

    numexpr_aux_node->sdt_mat.operacao = operation_node->token->lexema[0];
    numexpr_aux_node->sdt_mat.no = numexpression_node->sdt_mat.no;

    numexpression_node->sdt_mat.no = NULL;

    numexpr_aux_node->sdt_mat.contador_vetor = numexpression_node->sdt_mat.contador_vetor;
    lista_codigo_adicionar_lista(numexpr_aux_node->codigo, numexpression_node->codigo);
    copiar_res_var_codigo(&numexpr_aux_node->res_var_codigo, &numexpression_node->res_var_codigo);
}

void EXPA_ident_para_cima(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    // FACTOR -> LVALUE
    NoAST* factor_node = no_pai;
    NoAST* lvalue_node = factor_node->filhos[0];

    // Filhos do LVALUE são IDENT e ALLOCAUX
    NoAST* ident_node = lvalue_node->filhos[0];
    NoAST* allocaux_node = lvalue_node->filhos[1];

    // 1. VERIFICAR SE O SÍMBOLO FOI DECLARADO
    if (!gerenciador_simbolo_declarado(gerenciador, ident_node->token))
    {
        char msg_error[512];
        snprintf(msg_error, sizeof(msg_error), "%s usado antes da declaracao na linha %d:%d",
                 ident_node->token->lexema, ident_node->token->linha, ident_node->token->coluna);
        LANCAR_ERRO_SEMANTICO(msg_error);
    }
    gerenciador_registrar_uso_simbolo(gerenciador, ident_node->token);

    // 2. OBTER TIPO DO SÍMBOLO
    char* tipo_str = gerenciador_obter_tipo_simbolo(gerenciador, ident_node->token);
    if (!tipo_str)
    {
        LANCAR_ERRO_SEMANTICO("Símbolo não encontrado ou sem tipo definido na expressão.");
        return;
    }

    // DECLARAÇÕES NECESSÁRIAS
    int num_elementos = 0;
    int* dimensoes_maximas = AUXILIAR_obter_tamanhos_vetor(tipo_str, &num_elementos);
    char* tipo_final = AUXILIAR_obter_tipo(tipo_str, factor_node->sdt_mat.contador_vetor);

    factor_node->sdt_mat.contador_vetor = allocaux_node->sdt_mat.contador_vetor;
    lista_codigo_adicionar_lista(factor_node->codigo, lvalue_node->codigo);

    if (factor_node->sdt_mat.contador_vetor > 0)
    {
        ListaString* variaveis_indices = allocaux_node->sdt_mat.vetor_array_var;
        char* somador_temp = gerar_variavel_temporaria(resolvedor_global);
        char buffer[256];

        // Inicializar somador
        snprintf(buffer, sizeof(buffer), "%s = 0", somador_temp);
        adicionar_string(factor_node->codigo, buffer);

        int contador = 0;
        for (int i = 0; i < variaveis_indices->tamanho; i++)
        {
            char* var_indice = variaveis_indices->itens[i];
            char* temp_atual = gerar_variavel_temporaria(resolvedor_global);

            // Calcular índice atual
            snprintf(buffer, sizeof(buffer), "%s = %s", temp_atual, var_indice);
            adicionar_string(factor_node->codigo, buffer);

            // Multiplicar pelas dimensões anteriores
            if (contador > 0)
            {
                for (int j = 0; j < contador; j++)
                {
                    if (j < num_elementos)
                    {
                        snprintf(buffer, sizeof(buffer), "%s = %s * %d", temp_atual, temp_atual,
                                 dimensoes_maximas[j]);
                        adicionar_string(factor_node->codigo, buffer);
                    }
                }
            }

            // Acumular no somador
            snprintf(buffer, sizeof(buffer), "%s = %s + %s", somador_temp, somador_temp,
                     temp_atual);
            adicionar_string(factor_node->codigo, buffer);

            free(temp_atual);
            contador++;
        }

        // Gerar acesso ao array
        char acesso_final[256];
        snprintf(acesso_final, sizeof(acesso_final), "%s[%s]", ident_node->token->lexema,
                 somador_temp);

        factor_node->sdt_mat.no = criar_no_expressao_simples('n', tipo_final, acesso_final);
        if (factor_node->res_var_codigo.var) free(factor_node->res_var_codigo.var);
        factor_node->res_var_codigo.var = strdup(acesso_final);

        free(somador_temp);
    }
    else
    {
        // Lógica para variável simples
        factor_node->sdt_mat.no =
            criar_no_expressao_simples('n', tipo_str, ident_node->token->lexema);
        if (factor_node->res_var_codigo.var) free(factor_node->res_var_codigo.var);
        factor_node->res_var_codigo.var = strdup(ident_node->token->lexema);
    }

    free(tipo_str);
    free(tipo_final);
    free(dimensoes_maximas);
}

void EXPA_valor_para_cima(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    // UNARYEXPR -> FACTOR
    NoAST* unaryexpr_node = no_pai;
    NoAST* factor_node = no_pai->filhos[0];
    unaryexpr_node->sdt_mat.contador_vetor = factor_node->sdt_mat.contador_vetor;
    unaryexpr_node->sdt_mat.no = factor_node->sdt_mat.no;
    factor_node->sdt_mat.no = NULL;
    copiar_res_var_codigo(&unaryexpr_node->res_var_codigo, &factor_node->res_var_codigo);
    lista_codigo_adicionar_lista(unaryexpr_node->codigo, factor_node->codigo);
}

void EXPA_valor_segundo_filho_para_cima(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    // UNARYEXPR -> PLUS/MINUS FACTOR
    NoAST* unaryexpr_node = no_pai;
    NoAST* symbol_node = no_pai->filhos[0];  // PLUS or MINUS
    NoAST* factor_node = no_pai->filhos[1];

    char* var_operando = obter_valor_rhs(factor_node);
    unaryexpr_node->res_var_codigo.var = gerar_variavel_temporaria(resolvedor_global);

    char buffer[256];
    snprintf(buffer, sizeof(buffer), "%s = %c%s", unaryexpr_node->res_var_codigo.var,
             symbol_node->token->lexema[0], var_operando);
    adicionar_string(unaryexpr_node->codigo, buffer);

    // Create a new node representing the unary operation
    unaryexpr_node->sdt_mat.no =
        criar_no_expressao_unario(symbol_node->token->lexema[0], "N", factor_node->sdt_mat.no);
    free(var_operando);
}

void EXPA_gerar_no(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    // EX: NUMEXPRESSION -> TERM NUMEXPRESSIONAUX
    NoAST* numexpr_node = no_pai;
    NoAST* term_node = numexpr_node->filhos[0];
    NoAST* aux_node = numexpr_node->filhos[1];

    // Lista para armazenar as operações intermediárias
    ListaString* codigo_operacoes = criar_lista_string();

    // Processa o primeiro operando (TERM)
    lista_codigo_adicionar_lista(numexpr_node->codigo, term_node->codigo);

    // Garante que o primeiro operando está em uma variável
    char* resultado_esquerda = garantir_primeiro_operando_em_var(term_node, numexpr_node->codigo);
    NoExpressao* no_resultado_esquerda = term_node->sdt_mat.no;
    term_node->sdt_mat.no = NULL;  // Transfere posse

    // Itera pela cadeia de operações (+, -)
    while (aux_node && aux_node->quantidade_filhos > 0)
    {
        NoAST* op_node = aux_node->filhos[0];
        NoAST* proximo_term_node = aux_node->filhos[1];

        // Processa o próximo operando
        lista_codigo_adicionar_lista(numexpr_node->codigo, proximo_term_node->codigo);

        // Obtém o valor do operando direito
        char* operando_direita = obter_valor_rhs(proximo_term_node);

        // Gera nova variável temporária para o resultado
        char* resultado_temp = gerar_variavel_temporaria(resolvedor_global);

        // Gera código da operação
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "%s = %s %s %s", resultado_temp, resultado_esquerda,
                 op_node->token->lexema, operando_direita);
        adicionar_string(codigo_operacoes, buffer);

        // Cria nó de expressão para a operação
        no_resultado_esquerda = criar_no_expressao_binario(
            op_node->token->lexema[0], "N", no_resultado_esquerda, proximo_term_node->sdt_mat.no);

        // Limpa nó do operando direito
        proximo_term_node->sdt_mat.no = NULL;

        // Atualiza variáveis para próxima operação
        free(resultado_esquerda);
        resultado_esquerda = resultado_temp;
        free(operando_direita);

        // Avança para o próximo nó auxiliar
        aux_node = aux_node->filhos[2];
    }

    // Adiciona todas as operações ao código principal
    lista_codigo_adicionar_lista(numexpr_node->codigo, codigo_operacoes);
    liberar_lista_string(codigo_operacoes);

    // Atualiza o nó de expressão final
    numexpr_node->sdt_mat.no = no_resultado_esquerda;

    // Atualiza a variável de resultado
    if (numexpr_node->res_var_codigo.var) free(numexpr_node->res_var_codigo.var);
    numexpr_node->res_var_codigo.var = resultado_esquerda;
}

void EXPA_definir_operacao2(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    NoAST* unaryexpr_aux_node = no_pai;
    NoAST* op_node = no_pai->filhos[0];
    NoAST* term_node = no_pai->filhos[1];

    unaryexpr_aux_node->sdt_mat.operacao = op_node->token->lexema[0];
    unaryexpr_aux_node->sdt_mat.no = term_node->sdt_mat.no;

    term_node->sdt_mat.no = NULL;

    unaryexpr_aux_node->sdt_mat.contador_vetor = term_node->sdt_mat.contador_vetor;
    lista_codigo_adicionar_lista(unaryexpr_aux_node->codigo, term_node->codigo);
    copiar_res_var_codigo(&unaryexpr_aux_node->res_var_codigo, &term_node->res_var_codigo);
}

void EXPA_termo(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    NoAST* term_node = no_pai;  // O nó TERM que está sendo processado
    NoAST* unary_node = term_node->filhos[0];
    NoAST* aux_node = term_node->filhos[1];

    ListaString* codigo_operacoes = criar_lista_string();

    // Processa o primeiro operando (UNARYEXPR)
    lista_codigo_adicionar_lista(term_node->codigo, unary_node->codigo);
    char* var_resultado_esq = garantir_primeiro_operando_em_var(unary_node, term_node->codigo);
    NoExpressao* no_resultado_esq = unary_node->sdt_mat.no;
    unary_node->sdt_mat.no = NULL;

    // Itera pela cadeia de operações (*, /, %)
    while (aux_node && aux_node->quantidade_filhos > 0)
    {
        NoAST* op_node = aux_node->filhos[0];
        NoAST* proximo_unary_node = aux_node->filhos[1];

        lista_codigo_adicionar_lista(term_node->codigo, proximo_unary_node->codigo);
        char* var_operando_dir = obter_valor_rhs(proximo_unary_node);

        char* var_novo_resultado = gerar_variavel_temporaria(resolvedor_global);
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "%s = %s %s %s", var_novo_resultado, var_resultado_esq,
                 op_node->token->lexema, var_operando_dir);
        adicionar_string(codigo_operacoes, buffer);

        no_resultado_esq = criar_no_expressao_binario(
            op_node->token->lexema[0], "N", no_resultado_esq, proximo_unary_node->sdt_mat.no);
        proximo_unary_node->sdt_mat.no = NULL;

        free(var_resultado_esq);
        var_resultado_esq = var_novo_resultado;
        free(var_operando_dir);

        aux_node = aux_node->filhos[2];
    }

    lista_codigo_adicionar_lista(term_node->codigo, codigo_operacoes);
    liberar_lista_string(codigo_operacoes);

    term_node->sdt_mat.no = no_resultado_esq;
    if (term_node->res_var_codigo.var) free(term_node->res_var_codigo.var);
    term_node->res_var_codigo.var = var_resultado_esq;
}

void EXPA_imprimir_expressao0_h(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    // EXPRESSION -> NUMEXPRESSION EXPRESSION'
    NoAST* numexpression_node = no_pai->filhos[0];
    NoAST* expression_linha_node = no_pai->filhos[1];
    if (numexpression_node->res_var_codigo.var)
    {
        expression_linha_node->res_var_codigo.var = strdup(numexpression_node->res_var_codigo.var);
    }
}

void EXPA_imprimir_expressao0(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    // EXPRESSION -> NUMEXPRESSION EXPRESSION'
    NoAST* expression_node = no_pai;
    NoAST* numexpression_node = no_pai->filhos[0];
    NoAST* expression_linha_node = no_pai->filhos[1];

    if (numexpression_node->res_var_codigo.var)
    {
        if (expression_node->res_var_codigo.var) free(expression_node->res_var_codigo.var);
        expression_node->res_var_codigo.var = strdup(numexpression_node->res_var_codigo.var);
    }

    lista_codigo_adicionar_lista(expression_node->codigo, numexpression_node->codigo);
    lista_codigo_adicionar_lista(expression_node->codigo, expression_linha_node->codigo);

    imprimir_arvore(numexpression_node->sdt_mat.no);
}

void EXPA_passar_numero(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    // FACTOR -> OPEN_PARENTHESIS NUMEXPRESSION CLOSE_PARENTHESIS
    NoAST* factor_node = no_pai;
    NoAST* numexpression_node = no_pai->filhos[1];

    if (factor_node->res_var_codigo.var) free(factor_node->res_var_codigo.var);
    factor_node->res_var_codigo.var = strdup(numexpression_node->res_var_codigo.var);

    lista_codigo_adicionar_lista(factor_node->codigo, numexpression_node->codigo);

    factor_node->sdt_mat.no = numexpression_node->sdt_mat.no;
    numexpression_node->sdt_mat.no = NULL;
}

void EXPA_imprimir_expressao1(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    // EXPRESSION' -> RELOP NUMEXPRESSION
    NoAST* numexpression_node = no_pai->filhos[1];
    imprimir_arvore(numexpression_node->sdt_mat.no);
}

void EXPA_imprimir_expressao2(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    // ALLOCEXPRESSION' -> TYPE OPEN_BRACKET NUMEXPRESSION CLOSE_BRACKET ALLOCAUX
    NoAST* numexpression_node = no_pai->filhos[2];
    imprimir_arvore(numexpression_node->sdt_mat.no);
}

void EXPA_calculo_indice(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    // ALLOCAUX -> [ NUMEXPRESSION ] ALLOCAUX1
    NoAST* numexpression = no_pai->filhos[1];
    NoAST* allocaux1 = no_pai->filhos[3];

    // 1. Obter as dimensões do array a partir do tipo
    char* tipo_simbolo = gerenciador_obter_tipo_simbolo(gerenciador, no_pai->token);
    int num_dimensoes = 0;
    int* dimensoes = AUXILIAR_obter_tamanhos_vetor(tipo_simbolo, &num_dimensoes);

    // 2. Inicializar variável para o índice calculado
    char* indice_calculado = gerar_variavel_temporaria(resolvedor_global);
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "%s = 0", indice_calculado);
    adicionar_string(no_pai->codigo, buffer);

    // 3. Gerar código para cálculo do índice linearizado
    char* temp_mult = gerar_variavel_temporaria(resolvedor_global);
    for (int i = 0; i < num_dimensoes; i++)
    {
        // Calcular produto das dimensões anteriores
        if (i > 0)
        {
            snprintf(buffer, sizeof(buffer), "%s = %d", temp_mult, dimensoes[i - 1]);
            adicionar_string(no_pai->codigo, buffer);

            for (int j = i - 2; j >= 0; j--)
            {
                snprintf(buffer, sizeof(buffer), "%s = %s * %d", temp_mult, temp_mult,
                         dimensoes[j]);
                adicionar_string(no_pai->codigo, buffer);
            }

            // Multiplicar índice atual pelo produto das dimensões
            snprintf(buffer, sizeof(buffer), "%s = %s * %s", temp_mult,
                     numexpression->res_var_codigo.var, temp_mult);
            adicionar_string(no_pai->codigo, buffer);

            // Adicionar ao índice calculado
            snprintf(buffer, sizeof(buffer), "%s = %s + %s", indice_calculado, indice_calculado,
                     temp_mult);
            adicionar_string(no_pai->codigo, buffer);
        }
        else
        {
            // Primeira dimensão
            snprintf(buffer, sizeof(buffer), "%s = %s", indice_calculado,
                     numexpression->res_var_codigo.var);
            adicionar_string(no_pai->codigo, buffer);
        }
    }

    // 4. Armazenar o índice calculado
    no_pai->res_var_codigo.var = strdup(indice_calculado);
    no_pai->sdt_mat.contador_vetor = num_dimensoes;

    // 5. Liberar memória
    free(dimensoes);
    free(tipo_simbolo);
}

void EXPA_inicializar_contador_vetor(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    no_pai->sdt_mat.contador_vetor = 0;
    no_pai->res_var_codigo.var = strdup("0");  // Índice padrão para variáveis escalares
}
