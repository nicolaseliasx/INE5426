#define _DEFAULT_SOURCE
#include "tabela_sdt_auxiliar.h"
#include "acoes_semanticas.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

EntradaTabelaAuxSDT tabela_sdt_entries[79];
const size_t num_entradas_sdt = 79;

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


void inicializar_tabela_sdt_entries() {
    // PROGRAM-DEF
    tabela_sdt_entries[0].chave = strdup("PROGRAM-DEF");
    tabela_sdt_entries[0].quantidade = 7;
    tabela_sdt_entries[0].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 7);
    inicializar_producao_PROGRAM_DEF(tabela_sdt_entries[0].itens);
    
    // PROGRAM-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR
    tabela_sdt_entries[1].chave = strdup("PROGRAM-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR");
    tabela_sdt_entries[1].quantidade = 6;
    tabela_sdt_entries[1].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 6);
    inicializar_producao_PROGRAM_IDENT(tabela_sdt_entries[1].itens);

    // PROGRAM-$
    tabela_sdt_entries[2].chave = strdup("PROGRAM-$");
    tabela_sdt_entries[2].quantidade = 2;
    tabela_sdt_entries[2].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 2);
    inicializar_producao_PROGRAM_DOLAR(tabela_sdt_entries[2].itens);

    // PROGRAM'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR
    tabela_sdt_entries[3].chave = strdup("PROGRAM'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR");
    tabela_sdt_entries[3].quantidade = 3;
    tabela_sdt_entries[3].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 3);
    inicializar_producao_PROGRAM_QUOTE_IDENT(tabela_sdt_entries[3].itens);

    // PROGRAM'-$
    tabela_sdt_entries[4].chave = strdup("PROGRAM'-$");
    tabela_sdt_entries[4].quantidade = 1;
    tabela_sdt_entries[4].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 1);
    inicializar_producao_PROGRAM_QUOTE_DOLAR(tabela_sdt_entries[4].itens);

    // FUNCLIST-DEF
    tabela_sdt_entries[5].chave = strdup("FUNCLIST-DEF");
    tabela_sdt_entries[5].quantidade = 4;
    tabela_sdt_entries[5].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 4);
    inicializar_producao_FUNCLIST_DEF(tabela_sdt_entries[5].itens);

    // FUNCLIST'-DEF
    tabela_sdt_entries[6].chave = strdup("FUNCLIST'-DEF");
    tabela_sdt_entries[6].quantidade = 3;
    tabela_sdt_entries[6].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 3);
    inicializar_producao_FUNCLIST_QUOTE_DEF(tabela_sdt_entries[6].itens);

    // FUNCLIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$
    tabela_sdt_entries[7].chave = strdup("FUNCLIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$");
    tabela_sdt_entries[7].quantidade = 1;
    tabela_sdt_entries[7].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 1);
    inicializar_producao_FUNCLIST_QUOTE_IDENT(tabela_sdt_entries[7].itens);

    // FUNCDEF-DEF
    tabela_sdt_entries[8].chave = strdup("FUNCDEF-DEF");
    tabela_sdt_entries[8].quantidade = 14;
    tabela_sdt_entries[8].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 14);
    inicializar_producao_FUNCDEF_DEF(tabela_sdt_entries[8].itens);

    // PARAMLIST-CLOSE_PARENTHESIS
    tabela_sdt_entries[9].chave = strdup("PARAMLIST-CLOSE_PARENTHESIS");
    tabela_sdt_entries[9].quantidade = 1;
    tabela_sdt_entries[9].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 1);
    inicializar_producao_PARAMLIST_CLOSE_PARENTHESIS(tabela_sdt_entries[9].itens);

    // PARAMLIST-INT
    tabela_sdt_entries[10].chave = strdup("PARAMLIST-INT");
    tabela_sdt_entries[10].quantidade = 5;
    tabela_sdt_entries[10].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 5);
    inicializar_producao_PARAMLIST_INT(tabela_sdt_entries[10].itens);

    // PARAMLIST-FLOAT
    tabela_sdt_entries[11].chave = strdup("PARAMLIST-FLOAT");
    tabela_sdt_entries[11].quantidade = 5;
    tabela_sdt_entries[11].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 5);
    inicializar_producao_PARAMLIST_FLOAT(tabela_sdt_entries[11].itens);

    // PARAMLIST-STRING
    tabela_sdt_entries[12].chave = strdup("PARAMLIST-STRING");
    tabela_sdt_entries[12].quantidade = 5;
    tabela_sdt_entries[12].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 5);
    inicializar_producao_PARAMLIST_STRING(tabela_sdt_entries[12].itens);

    // PARAMLIST'-CLOSE_PARENTHESIS
    tabela_sdt_entries[13].chave = strdup("PARAMLIST'-CLOSE_PARENTHESIS");
    tabela_sdt_entries[13].quantidade = 1;
    tabela_sdt_entries[13].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 1);
    inicializar_producao_PARAMLIST_QUOTE_CLOSE_PARENTHESIS(tabela_sdt_entries[13].itens);

    // PARAMLIST'-COMMA
    tabela_sdt_entries[14].chave = strdup("PARAMLIST'-COMMA");
    tabela_sdt_entries[14].quantidade = 2;
    tabela_sdt_entries[14].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 2);
    inicializar_producao_PARAMLIST_QUOTE_COMMA(tabela_sdt_entries[14].itens);

    // STATEMENT-IDENT
    tabela_sdt_entries[15].chave = strdup("STATEMENT-IDENT");
    tabela_sdt_entries[15].quantidade = 4;
    tabela_sdt_entries[15].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 4);
    inicializar_producao_STATEMENT_IDENT(tabela_sdt_entries[15].itens);

    // STATEMENT-OPEN_BRACE
    tabela_sdt_entries[16].chave = strdup("STATEMENT-OPEN_BRACE");
    tabela_sdt_entries[16].quantidade = 7;
    tabela_sdt_entries[16].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 7);
    inicializar_producao_STATEMENT_OPEN_BRACE(tabela_sdt_entries[16].itens);

    // STATEMENT-INT-FLOAT-STRING
    tabela_sdt_entries[17].chave = strdup("STATEMENT-INT-FLOAT-STRING");
    tabela_sdt_entries[17].quantidade = 3;
    tabela_sdt_entries[17].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 3);
    inicializar_producao_STATEMENT_INT_FLOAT_STRING(tabela_sdt_entries[17].itens);

    // STATEMENT-SEMICOLON
    tabela_sdt_entries[18].chave = strdup("STATEMENT-SEMICOLON");
    tabela_sdt_entries[18].quantidade = 1;
    tabela_sdt_entries[18].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 1);
    inicializar_producao_STATEMENT_SEMICOLON(tabela_sdt_entries[18].itens);

    // STATEMENT-BREAK
    tabela_sdt_entries[19].chave = strdup("STATEMENT-BREAK");
    tabela_sdt_entries[19].quantidade = 4;
    tabela_sdt_entries[19].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 4);
    inicializar_producao_STATEMENT_BREAK(tabela_sdt_entries[19].itens);

    // STATEMENT-PRINT
    tabela_sdt_entries[20].chave = strdup("STATEMENT-PRINT");
    tabela_sdt_entries[20].quantidade = 4;
    tabela_sdt_entries[20].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 4);
    inicializar_producao_STATEMENT_PRINT(tabela_sdt_entries[20].itens);

    // STATEMENT-READ
    tabela_sdt_entries[21].chave = strdup("STATEMENT-READ");
    tabela_sdt_entries[21].quantidade = 4;
    tabela_sdt_entries[21].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 4);
    inicializar_producao_STATEMENT_READ(tabela_sdt_entries[21].itens);

    // STATEMENT-RETURN
    tabela_sdt_entries[22].chave = strdup("STATEMENT-RETURN");
    tabela_sdt_entries[22].quantidade = 4;
    tabela_sdt_entries[22].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 4);
    inicializar_producao_STATEMENT_RETURN(tabela_sdt_entries[22].itens);

    // STATEMENT-IF
    tabela_sdt_entries[23].chave = strdup("STATEMENT-IF");
    tabela_sdt_entries[23].quantidade = 3;
    tabela_sdt_entries[23].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 3);
    inicializar_producao_STATEMENT_IF(tabela_sdt_entries[23].itens);

    // STATEMENT-FOR
    tabela_sdt_entries[24].chave = strdup("STATEMENT-FOR");
    tabela_sdt_entries[24].quantidade = 3;
    tabela_sdt_entries[24].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 3);
    inicializar_producao_STATEMENT_FOR(tabela_sdt_entries[24].itens);

    // INDEX-SEMICOLON
    tabela_sdt_entries[25].chave = strdup("INDEX-SEMICOLON");
    tabela_sdt_entries[25].quantidade = 2;
    tabela_sdt_entries[25].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 2);
    inicializar_producao_INDEX_SEMICOLON(tabela_sdt_entries[25].itens);

    // INDEX-OPEN_BRACKET
    tabela_sdt_entries[26].chave = strdup("INDEX-OPEN_BRACKET");
    tabela_sdt_entries[26].quantidade = 6;
    tabela_sdt_entries[26].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 6);
    inicializar_producao_INDEX_OPEN_BRACKET(tabela_sdt_entries[26].itens);

    // VARDECL-INT
    tabela_sdt_entries[27].chave = strdup("VARDECL-INT");
    tabela_sdt_entries[27].quantidade = 7;
    tabela_sdt_entries[27].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 7);
    inicializar_producao_VARDECL_INT(tabela_sdt_entries[27].itens);

    // VARDECL-FLOAT
    tabela_sdt_entries[28].chave = strdup("VARDECL-FLOAT");
    tabela_sdt_entries[28].quantidade = 7;
    tabela_sdt_entries[28].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 7);
    inicializar_producao_VARDECL_FLOAT(tabela_sdt_entries[28].itens);

    // VARDECL-STRING
    tabela_sdt_entries[29].chave = strdup("VARDECL-STRING");
    tabela_sdt_entries[29].quantidade = 7;
    tabela_sdt_entries[29].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 7);
    inicializar_producao_VARDECL_STRING(tabela_sdt_entries[29].itens);

    // ATRIBSTAT-IDENT
    tabela_sdt_entries[30].chave = strdup("ATRIBSTAT-IDENT");
    tabela_sdt_entries[30].quantidade = 5;
    tabela_sdt_entries[30].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 5);
    inicializar_producao_ATRIBSTAT_IDENT(tabela_sdt_entries[30].itens);

    // ATRIBSTAT'-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL
    tabela_sdt_entries[31].chave = strdup("ATRIBSTAT'-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL");
    tabela_sdt_entries[31].quantidade = 2;
    tabela_sdt_entries[31].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 2);
    inicializar_producao_ATRIBSTAT_QUOTE_IDENT(tabela_sdt_entries[31].itens);

    // ATRIBSTAT'-CALL
    tabela_sdt_entries[32].chave = strdup("ATRIBSTAT'-CALL");
    tabela_sdt_entries[32].quantidade = 4;
    tabela_sdt_entries[32].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 4);
    inicializar_producao_ATRIBSTAT_QUOTE_CALL(tabela_sdt_entries[32].itens);

    // ATRIBSTAT'-NEW
    tabela_sdt_entries[33].chave = strdup("ATRIBSTAT'-NEW");
    tabela_sdt_entries[33].quantidade = 1;
    tabela_sdt_entries[33].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 1);
    inicializar_producao_ATRIBSTAT_QUOTE_NEW(tabela_sdt_entries[33].itens);

    // FUNCCALL-CALL
    tabela_sdt_entries[34].chave = strdup("FUNCCALL-CALL");
    tabela_sdt_entries[34].quantidade = 9;
    tabela_sdt_entries[34].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 9);
    inicializar_producao_FUNCCALL_CALL(tabela_sdt_entries[34].itens);

    // PARAMLISTCALL-IDENT
    tabela_sdt_entries[35].chave = strdup("PARAMLISTCALL-IDENT");
    tabela_sdt_entries[35].quantidade = 4;
    tabela_sdt_entries[35].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 4);
    inicializar_producao_PARAMLISTCALL_IDENT(tabela_sdt_entries[35].itens);

    // PARAMLISTCALL-CLOSE_PARENTHESIS
    tabela_sdt_entries[36].chave = strdup("PARAMLISTCALL-CLOSE_PARENTHESIS");
    tabela_sdt_entries[36].quantidade = 1;
    tabela_sdt_entries[36].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 1);
    inicializar_producao_PARAMLISTCALL_CLOSE_PARENTHESIS(tabela_sdt_entries[36].itens);

    // PARAMLISTCALL'-CLOSE_PARENTHESIS
    tabela_sdt_entries[37].chave = strdup("PARAMLISTCALL'-CLOSE_PARENTHESIS");
    tabela_sdt_entries[37].quantidade = 1;
    tabela_sdt_entries[37].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 1);
    inicializar_producao_PARAMLISTCALL_QUOTE_CLOSE_PARENTHESIS(tabela_sdt_entries[37].itens);

    // PARAMLISTCALL'-COMMA
    tabela_sdt_entries[38].chave = strdup("PARAMLISTCALL'-COMMA");
    tabela_sdt_entries[38].quantidade = 4;
    tabela_sdt_entries[38].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 4);
    inicializar_producao_PARAMLISTCALL_QUOTE_COMMA(tabela_sdt_entries[38].itens);

    // PRINTSTAT-PRINT
    tabela_sdt_entries[39].chave = strdup("PRINTSTAT-PRINT");
    tabela_sdt_entries[39].quantidade = 3;
    tabela_sdt_entries[39].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 3);
    inicializar_producao_PRINTSTAT_PRINT(tabela_sdt_entries[39].itens);

    // READSTAT-READ
    tabela_sdt_entries[40].chave = strdup("READSTAT-READ");
    tabela_sdt_entries[40].quantidade = 3;
    tabela_sdt_entries[40].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 3);
    inicializar_producao_READSTAT_READ(tabela_sdt_entries[40].itens);

    // RETURNSTAT-RETURN
    tabela_sdt_entries[41].chave = strdup("RETURNSTAT-RETURN");
    tabela_sdt_entries[41].quantidade = 3;
    tabela_sdt_entries[41].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 3);
    inicializar_producao_RETURNSTAT_RETURN(tabela_sdt_entries[41].itens);

    // RETURNSTAT'-IDENT
    tabela_sdt_entries[42].chave = strdup("RETURNSTAT'-IDENT");
    tabela_sdt_entries[42].quantidade = 2;
    tabela_sdt_entries[42].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 2);
    inicializar_producao_RETURNSTAT_QUOTE_IDENT(tabela_sdt_entries[42].itens);

    // RETURNSTAT'-SEMICOLON
    tabela_sdt_entries[43].chave = strdup("RETURNSTAT'-SEMICOLON");
    tabela_sdt_entries[43].quantidade = 1;
    tabela_sdt_entries[43].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 1);
    inicializar_producao_RETURNSTAT_QUOTE_SEMICOLON(tabela_sdt_entries[43].itens);

    // IFSTAT-IF
    tabela_sdt_entries[44].chave = strdup("IFSTAT-IF");
    tabela_sdt_entries[44].quantidade = 12;
    tabela_sdt_entries[44].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 12);
    inicializar_producao_IFSTAT_IF(tabela_sdt_entries[44].itens);

    // IFSTAT'-IDENT-OPEN_BRACE-CLOSE_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$
    tabela_sdt_entries[45].chave = strdup("IFSTAT'-IDENT-OPEN_BRACE-CLOSE_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$");
    tabela_sdt_entries[45].quantidade = 2;
    tabela_sdt_entries[45].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 2);
    inicializar_producao_IFSTAT_QUOTE_IDENT(tabela_sdt_entries[45].itens);

    // IFSTAT'-ELSE
    tabela_sdt_entries[46].chave = strdup("IFSTAT'-ELSE");
    tabela_sdt_entries[46].quantidade = 8;
    tabela_sdt_entries[46].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 8);
    inicializar_producao_IFSTAT_QUOTE_ELSE(tabela_sdt_entries[46].itens);

    // FORSTAT-FOR
    tabela_sdt_entries[47].chave = strdup("FORSTAT-FOR");
    tabela_sdt_entries[47].quantidade = 13;
    tabela_sdt_entries[47].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 13);
    inicializar_producao_FORSTAT_FOR(tabela_sdt_entries[47].itens);

    // STATELIST-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR
    tabela_sdt_entries[48].chave = strdup("STATELIST-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR");
    tabela_sdt_entries[48].quantidade = 5;
    tabela_sdt_entries[48].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 5);
    inicializar_producao_STATELIST_IDENT(tabela_sdt_entries[48].itens);

    // STATELIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR
    tabela_sdt_entries[49].chave = strdup("STATELIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR");
    tabela_sdt_entries[49].quantidade = 3;
    tabela_sdt_entries[49].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 3);
    inicializar_producao_STATELIST_QUOTE_IDENT(tabela_sdt_entries[49].itens);

    // STATELIST'-CLOSE_BRACE
    tabela_sdt_entries[50].chave = strdup("STATELIST'-CLOSE_BRACE");
    tabela_sdt_entries[50].quantidade = 3;
    tabela_sdt_entries[50].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 3);
    inicializar_producao_STATELIST_QUOTE_CLOSE_BRACE(tabela_sdt_entries[50].itens);

    // ALLOCAUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_BRACKET-ASSIGN-RELOP-PLUS-MINUS-MULTIPLICATION-DIVISION-MODULUS
    tabela_sdt_entries[51].chave = strdup("ALLOCAUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_BRACKET-ASSIGN-RELOP-PLUS-MINUS-MULTIPLICATION-DIVISION-MODULUS");
    tabela_sdt_entries[51].quantidade = 1;
    tabela_sdt_entries[51].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 1);
    inicializar_producao_ALLOCAUX_CLOSE_PARENTHESIS(tabela_sdt_entries[51].itens);

    // ALLOCAUX-OPEN_BRACKET
    tabela_sdt_entries[52].chave = strdup("ALLOCAUX-OPEN_BRACKET");
    tabela_sdt_entries[52].quantidade = 6;
    tabela_sdt_entries[52].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 6);
    inicializar_producao_ALLOCAUX_OPEN_BRACKET(tabela_sdt_entries[52].itens);

    // ALLOCEXPRESSION-NEW
    tabela_sdt_entries[53].chave = strdup("ALLOCEXPRESSION-NEW");
    tabela_sdt_entries[53].quantidade = 2;
    tabela_sdt_entries[53].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 2);
    inicializar_producao_ALLOCEXPRESSION_NEW(tabela_sdt_entries[53].itens);

    // ALLOCEXPRESSION'-INT
    tabela_sdt_entries[54].chave = strdup("ALLOCEXPRESSION'-INT");
    tabela_sdt_entries[54].quantidade = 6;
    tabela_sdt_entries[54].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 6);
    inicializar_producao_ALLOCEXPRESSION_QUOTE_INT(tabela_sdt_entries[54].itens);

    // ALLOCEXPRESSION'-FLOAT
    tabela_sdt_entries[55].chave = strdup("ALLOCEXPRESSION'-FLOAT");
    tabela_sdt_entries[55].quantidade = 6;
    tabela_sdt_entries[55].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 6);
    inicializar_producao_ALLOCEXPRESSION_QUOTE_FLOAT(tabela_sdt_entries[55].itens);

    // ALLOCEXPRESSION'-STRING
    tabela_sdt_entries[56].chave = strdup("ALLOCEXPRESSION'-STRING");
    tabela_sdt_entries[56].quantidade = 6;
    tabela_sdt_entries[56].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 6);
    inicializar_producao_ALLOCEXPRESSION_QUOTE_STRING(tabela_sdt_entries[56].itens);
    
    // EXPRESSION-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL
    tabela_sdt_entries[57].chave = strdup("EXPRESSION-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL");
    tabela_sdt_entries[57].quantidade = 4;
    tabela_sdt_entries[57].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 4);
    inicializar_producao_EXPRESSION_IDENT(tabela_sdt_entries[57].itens);

    // EXPRESSION'-CLOSE_PARENTHESIS-SEMICOLON
    tabela_sdt_entries[58].chave = strdup("EXPRESSION'-CLOSE_PARENTHESIS-SEMICOLON");
    tabela_sdt_entries[58].quantidade = 1;
    tabela_sdt_entries[58].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 1);
    inicializar_producao_EXPRESSION_QUOTE_CLOSE_PARENTHESIS(tabela_sdt_entries[58].itens);

    // EXPRESSION'-RELOP
    tabela_sdt_entries[59].chave = strdup("EXPRESSION'-RELOP");
    tabela_sdt_entries[59].quantidade = 4;
    tabela_sdt_entries[59].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 4);
    inicializar_producao_EXPRESSION_QUOTE_RELOP(tabela_sdt_entries[59].itens);

    // NUMEXPRESSIONAUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_BRACKET-RELOP
    tabela_sdt_entries[60].chave = strdup("NUMEXPRESSIONAUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_BRACKET-RELOP");
    tabela_sdt_entries[60].quantidade = 1;
    tabela_sdt_entries[60].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 1);
    inicializar_producao_NUMEXPRESSIONAUX_CLOSE_PARENTHESIS(tabela_sdt_entries[60].itens);

    // NUMEXPRESSIONAUX-PLUS
    tabela_sdt_entries[61].chave = strdup("NUMEXPRESSIONAUX-PLUS");
    tabela_sdt_entries[61].quantidade = 3;
    tabela_sdt_entries[61].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 3);
    inicializar_producao_NUMEXPRESSIONAUX_PLUS(tabela_sdt_entries[61].itens);

    // NUMEXPRESSIONAUX-MINUS
    tabela_sdt_entries[62].chave = strdup("NUMEXPRESSIONAUX-MINUS");
    tabela_sdt_entries[62].quantidade = 3;
    tabela_sdt_entries[62].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 3);
    inicializar_producao_NUMEXPRESSIONAUX_MINUS(tabela_sdt_entries[62].itens);

    // NUMEXPRESSION-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL
    tabela_sdt_entries[63].chave = strdup("NUMEXPRESSION-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL");
    tabela_sdt_entries[63].quantidade = 3;
    tabela_sdt_entries[63].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 3);
    inicializar_producao_NUMEXPRESSION_IDENT(tabela_sdt_entries[63].itens);

    // TERM-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL
    tabela_sdt_entries[64].chave = strdup("TERM-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL");
    tabela_sdt_entries[64].quantidade = 3;
    tabela_sdt_entries[64].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 3);
    inicializar_producao_TERM_IDENT(tabela_sdt_entries[64].itens);

    // UNARYEXPRAUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_BRACKET-RELOP-PLUS-MINUS
    tabela_sdt_entries[65].chave = strdup("UNARYEXPRAUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_BRACKET-RELOP-PLUS-MINUS");
    tabela_sdt_entries[65].quantidade = 1;
    tabela_sdt_entries[65].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 1);
    inicializar_producao_UNARYEXPRAUX_CLOSE_PARENTHESIS(tabela_sdt_entries[65].itens);

    // UNARYEXPRAUX-MULTIPLICATION
    tabela_sdt_entries[66].chave = strdup("UNARYEXPRAUX-MULTIPLICATION");
    tabela_sdt_entries[66].quantidade = 3;
    tabela_sdt_entries[66].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 3);
    inicializar_producao_UNARYEXPRAUX_MULTIPLICATION(tabela_sdt_entries[66].itens);

    // UNARYEXPRAUX-DIVISION
    tabela_sdt_entries[67].chave = strdup("UNARYEXPRAUX-DIVISION");
    tabela_sdt_entries[67].quantidade = 3;
    tabela_sdt_entries[67].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 3);
    inicializar_producao_UNARYEXPRAUX_DIVISION(tabela_sdt_entries[67].itens);

    // UNARYEXPRAUX-MODULUS
    tabela_sdt_entries[68].chave = strdup("UNARYEXPRAUX-MODULUS");
    tabela_sdt_entries[68].quantidade = 3;
    tabela_sdt_entries[68].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 3);
    inicializar_producao_UNARYEXPRAUX_MODULUS(tabela_sdt_entries[68].itens);

    // UNARYEXPR-IDENT-OPEN_PARENTHESIS-NI-NPF-STRC-NULL
    tabela_sdt_entries[69].chave = strdup("UNARYEXPR-IDENT-OPEN_PARENTHESIS-NI-NPF-STRC-NULL");
    tabela_sdt_entries[69].quantidade = 2;
    tabela_sdt_entries[69].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 2);
    inicializar_producao_UNARYEXPR_IDENT(tabela_sdt_entries[69].itens);

    // UNARYEXPR-PLUS
    tabela_sdt_entries[70].chave = strdup("UNARYEXPR-PLUS");
    tabela_sdt_entries[70].quantidade = 3;
    tabela_sdt_entries[70].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 3);
    inicializar_producao_UNARYEXPR_PLUS(tabela_sdt_entries[70].itens);

    // UNARYEXPR-MINUS
    tabela_sdt_entries[71].chave = strdup("UNARYEXPR-MINUS");
    tabela_sdt_entries[71].quantidade = 3;
    tabela_sdt_entries[71].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 3);
    inicializar_producao_UNARYEXPR_MINUS(tabela_sdt_entries[71].itens);

    // FACTOR-IDENT
    tabela_sdt_entries[72].chave = strdup("FACTOR-IDENT");
    tabela_sdt_entries[72].quantidade = 2;
    tabela_sdt_entries[72].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 2);
    inicializar_producao_FACTOR_IDENT(tabela_sdt_entries[72].itens);

    // FACTOR-OPEN_PARENTHESIS
    tabela_sdt_entries[73].chave = strdup("FACTOR-OPEN_PARENTHESIS");
    tabela_sdt_entries[73].quantidade = 4;
    tabela_sdt_entries[73].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 4);
    inicializar_producao_FACTOR_OPEN_PARENTHESIS(tabela_sdt_entries[73].itens);

    // FACTOR-NI
    tabela_sdt_entries[74].chave = strdup("FACTOR-NI");
    tabela_sdt_entries[74].quantidade = 2;
    tabela_sdt_entries[74].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 2);
    inicializar_producao_FACTOR_NI(tabela_sdt_entries[74].itens);

    // FACTOR-NPF
    tabela_sdt_entries[75].chave = strdup("FACTOR-NPF");
    tabela_sdt_entries[75].quantidade = 2;
    tabela_sdt_entries[75].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 2);
    inicializar_producao_FACTOR_NPF(tabela_sdt_entries[75].itens);

    // FACTOR-STRC
    tabela_sdt_entries[76].chave = strdup("FACTOR-STRC");
    tabela_sdt_entries[76].quantidade = 2;
    tabela_sdt_entries[76].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 2);
    inicializar_producao_FACTOR_STRC(tabela_sdt_entries[76].itens);

    // FACTOR-NULL
    tabela_sdt_entries[77].chave = strdup("FACTOR-NULL");
    tabela_sdt_entries[77].quantidade = 1;
    tabela_sdt_entries[77].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 1);
    inicializar_producao_FACTOR_NULL(tabela_sdt_entries[77].itens);

    // LVALUE-IDENT - The one you provided as the last entry
    tabela_sdt_entries[78].chave = strdup("LVALUE-IDENT");
    tabela_sdt_entries[78].quantidade = 6;
    tabela_sdt_entries[78].itens = (ItemTabelaSDT*)malloc(sizeof(ItemTabelaSDT) * 6);
    tabela_sdt_entries[78].itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "IDENT"}};
    tabela_sdt_entries[78].itens[1] = (ItemTabelaSDT){ACAO, {.acao = DECLARAR_VERIFICAR_acao2}};
    tabela_sdt_entries[78].itens[2] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_obter_variavel_lvalue}};
    tabela_sdt_entries[78].itens[3] = (ItemTabelaSDT){ACAO, {.acao = EXPA_avaliar_identificador}};
    tabela_sdt_entries[78].itens[4] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "ALLOCAUX"}};
    tabela_sdt_entries[78].itens[5] = (ItemTabelaSDT){ACAO, {.acao = EXPA_avaliar_identificador}};
}

static void inicializar_producao_PROGRAM_DEF(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){ACAO, {.acao = ESCOPO_acao1}};
    itens[1] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_definir_rotulo_final}};
    itens[2] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_herdar_proximo}};
    itens[3] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "FUNCLIST"}};
    itens[4] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "PROGRAM'"}};
    itens[5] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_ir_para_principal}};
    itens[6] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_gerar_codigo_final}};
}

static void inicializar_producao_PROGRAM_IDENT(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){ACAO, {.acao = ESCOPO_acao1}};
    itens[1] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_definir_rotulo_final}};
    itens[2] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_herdar_proximo}};
    itens[3] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "STATEMENT"}};
    itens[4] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_obter_codigo_filhos}};
    itens[5] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_gerar_codigo_final}};
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
    itens[3] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_lidar_break}};
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
    itens[7] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_obter_codigo_filhos}};
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
    itens[4] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_atribuicao}};
}

static void inicializar_producao_ATRIBSTAT_QUOTE_IDENT(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "EXPRESSION"}};
    itens[1] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_definir_valor_expressao}};
}

static void inicializar_producao_ATRIBSTAT_QUOTE_CALL(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_obter_valor_retorno}};
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
    itens[8] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_chamada_funcao}};
}

static void inicializar_producao_PARAMLISTCALL_IDENT(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "IDENT"}};
    itens[1] = (ItemTabelaSDT){ACAO, {.acao = DECLARAR_VERIFICAR_acao2}};
    itens[2] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "PARAMLISTCALL'"}};
    itens[3] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_chamada_paramlist}};
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
    itens[2] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_obter_parametros}};
    itens[3] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_obter_codigo_filhos}};
}

static void inicializar_producao_PRINTSTAT_PRINT(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "PRINT"}};
    itens[1] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "EXPRESSION"}};
    itens[2] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_lidar_impressao}};
}

static void inicializar_producao_READSTAT_READ(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "READ"}};
    itens[1] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "LVALUE"}};
    itens[2] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_lidar_leitura}};
}

static void inicializar_producao_RETURNSTAT_RETURN(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "RETURN"}};
    itens[1] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "RETURNSTAT'"}};
    itens[2] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_lidar_retorno}};
}

static void inicializar_producao_RETURNSTAT_QUOTE_IDENT(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "LVALUE"}};
    itens[1] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_lidar_retorno2}};
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
    itens[11] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_acao_if_s}};
}

static void inicializar_producao_IFSTAT_QUOTE_IDENT(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_acao_else_vazio}};
    itens[1] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "ε"}};
}

static void inicializar_producao_IFSTAT_QUOTE_ELSE(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_acao_else_h}};
    itens[1] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "ELSE"}};
    itens[2] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "OPEN_BRACE"}};
    itens[3] = (ItemTabelaSDT){ACAO, {.acao = ESCOPO_acao6}};
    itens[4] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "STATEMENT"}};
    itens[5] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "CLOSE_BRACE"}};
    itens[6] = (ItemTabelaSDT){ACAO, {.acao = ESCOPO_acao2}};
    itens[7] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_acao_else_s}};
}

static void inicializar_producao_FORSTAT_FOR(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_herdar_proximo}};
    itens[1] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_herdar_proximo_for}};
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
    itens[13] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_acao_for}};
}

static void inicializar_producao_STATELIST_IDENT(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_herdar_proximo}};
    itens[1] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_definir_rotulo}};
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
    itens[4] = (ItemTabelaSDT){ACAO, {.acao = EXPA_imprimir_expressao1}};
    itens[5] = (ItemTabelaSDT){ACAO, {.acao = EXPA_contador_vetor}};
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
    itens[5] = (ItemTabelaSDT){ACAO, {.acao = EXPA_imprimir_expressao2}};
}

static void inicializar_producao_ALLOCEXPRESSION_QUOTE_FLOAT(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "FLOAT"}};
    itens[1] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "OPEN_BRACKET"}};
    itens[2] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "NUMEXPRESSION"}};
    itens[3] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "CLOSE_BRACKET"}};
    itens[4] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "ALLOCAUX"}};
    itens[5] = (ItemTabelaSDT){ACAO, {.acao = EXPA_imprimir_expressao2}};
}

static void inicializar_producao_ALLOCEXPRESSION_QUOTE_STRING(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "STRING"}};
    itens[1] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "OPEN_BRACKET"}};
    itens[2] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "NUMEXPRESSION"}};
    itens[3] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "CLOSE_BRACKET"}};
    itens[4] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "ALLOCAUX"}};
    itens[5] = (ItemTabelaSDT){ACAO, {.acao = EXPA_imprimir_expressao2}};
}

static void inicializar_producao_EXPRESSION_IDENT(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "NUMEXPRESSION"}};
    itens[1] = (ItemTabelaSDT){ACAO, {.acao = EXPA_imprimir_expressao0_h}};
    itens[2] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "EXPRESSION'"}};
    itens[3] = (ItemTabelaSDT){ACAO, {.acao = EXPA_imprimir_expressao0_h}};
}

static void inicializar_producao_EXPRESSION_QUOTE_CLOSE_PARENTHESIS(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "ε"}};
}

static void inicializar_producao_EXPRESSION_QUOTE_RELOP(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "RELOP"}};
    itens[1] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "NUMEXPRESSION"}};
    itens[2] = (ItemTabelaSDT){ACAO, {.acao = EXPA_imprimir_expressao1}};
    itens[3] = (ItemTabelaSDT){ACAO, {.acao = CODIGO_relop_action}};
}

static void inicializar_producao_NUMEXPRESSIONAUX_CLOSE_PARENTHESIS(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "ε"}};
}

static void inicializar_producao_NUMEXPRESSIONAUX_PLUS(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "PLUS"}};
    itens[1] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "NUMEXPRESSION"}};
    itens[2] = (ItemTabelaSDT){ACAO, {.acao = EXPA_definir_operacao}};
}

static void inicializar_producao_NUMEXPRESSIONAUX_MINUS(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "MINUS"}};
    itens[1] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "NUMEXPRESSION"}};
    itens[2] = (ItemTabelaSDT){ACAO, {.acao = EXPA_definir_operacao}};
}

static void inicializar_producao_NUMEXPRESSION_IDENT(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "TERM"}};
    itens[1] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "NUMEXPRESSIONAUX"}};
    itens[2] = (ItemTabelaSDT){ACAO, {.acao = EXPA_gerar_no}};
}

static void inicializar_producao_TERM_IDENT(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "UNARYEXPR"}};
    itens[1] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "UNARYEXPRAUX"}};
    itens[2] = (ItemTabelaSDT){ACAO, {.acao = EXPA_termo}};
}

static void inicializar_producao_UNARYEXPRAUX_CLOSE_PARENTHESIS(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "ε"}};
}

static void inicializar_producao_UNARYEXPRAUX_MULTIPLICATION(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "MULTIPLICATION"}};
    itens[1] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "TERM"}};
    itens[2] = (ItemTabelaSDT){ACAO, {.acao = EXPA_definir_operacao2}};
}

static void inicializar_producao_UNARYEXPRAUX_DIVISION(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "DIVISION"}};
    itens[1] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "TERM"}};
    itens[2] = (ItemTabelaSDT){ACAO, {.acao = EXPA_definir_operacao2}};
}

static void inicializar_producao_UNARYEXPRAUX_MODULUS(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "MODULUS"}};
    itens[1] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "TERM"}};
    itens[2] = (ItemTabelaSDT){ACAO, {.acao = EXPA_definir_operacao2}};
}

static void inicializar_producao_UNARYEXPR_IDENT(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "FACTOR"}};
    itens[1] = (ItemTabelaSDT){ACAO, {.acao = EXPA_valor_para_cima}};
}

static void inicializar_producao_UNARYEXPR_PLUS(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "PLUS"}};
    itens[1] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "FACTOR"}};
    itens[2] = (ItemTabelaSDT){ACAO, {.acao = EXPA_valor_segundo_filho_para_cima}};
}

static void inicializar_producao_UNARYEXPR_MINUS(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "MINUS"}};
    itens[1] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "FACTOR"}};
    itens[2] = (ItemTabelaSDT){ACAO, {.acao = EXPA_valor_segundo_filho_para_cima}};
}

static void inicializar_producao_FACTOR_IDENT(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "LVALUE"}};
    itens[1] = (ItemTabelaSDT){ACAO, {.acao = EXPA_ident_para_cima}};
}

static void inicializar_producao_FACTOR_OPEN_PARENTHESIS(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "OPEN_PARENTHESIS"}};
    itens[1] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "NUMEXPRESSION"}};
    itens[2] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "CLOSE_PARENTHESIS"}};
    itens[3] = (ItemTabelaSDT){ACAO, {.acao = EXPA_passar_numero}};
}

static void inicializar_producao_FACTOR_NI(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "NI"}};
    itens[1] = (ItemTabelaSDT){ACAO, {.acao = EXPA_lexema_para_valor}};
}

static void inicializar_producao_FACTOR_NPF(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "NPF"}};
    itens[1] = (ItemTabelaSDT){ACAO, {.acao = EXPA_lexema_para_valor}};
}

static void inicializar_producao_FACTOR_STRC(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "STRC"}};
    itens[1] = (ItemTabelaSDT){ACAO, {.acao = EXPA_lexema_para_valor}};
}

static void inicializar_producao_FACTOR_NULL(ItemTabelaSDT* itens) {
    itens[0] = (ItemTabelaSDT){SIMBOLO, {.simbolo = "NULL"}};
}