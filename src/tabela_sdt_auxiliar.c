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