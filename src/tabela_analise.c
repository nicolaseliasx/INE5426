#include "tabela_analise.h"

//
// TABELA DE ANÁLISE PREDITIVA (LL) APONTANDO PARA AS REGRAS UNIFICADAS
//
const EntradaTabelaAnalise tabela_analise[] = {
{"ALLOCAUX", (MapeamentoTerminal[]){
    {"ASSIGN", "ALLOCAUX->EPSILON"},
    {"CLOSE_BRACKET", "ALLOCAUX->EPSILON"},
    {"CLOSE_PARENTHESIS", "ALLOCAUX->EPSILON"},
    {"DIVISION", "ALLOCAUX->EPSILON"},
    {"MINUS", "ALLOCAUX->EPSILON"},
    {"MODULUS", "ALLOCAUX->EPSILON"},
    {"MULTIPLICATION", "ALLOCAUX->EPSILON"},
    {"OPEN_BRACKET", "ALLOCAUX->BRACKET"},
    {"PLUS", "ALLOCAUX->EPSILON"},
    {"RELOP", "ALLOCAUX->EPSILON"},
    {"SEMICOLON", "ALLOCAUX->EPSILON"}
}, 11},
{"ALLOCEXPRESSION", (MapeamentoTerminal[]){ {"NEW", "ALLOCEXPR->NEW"} }, 1},
{"ALLOCEXPRESSION'", (MapeamentoTerminal[]){
    {"FLOAT", "ALLOCEXPR'->TYPE"},
    {"INT", "ALLOCEXPR'->TYPE"},
    {"STRING", "ALLOCEXPR'->TYPE"}
}, 3},
{"ATRIBSTAT", (MapeamentoTerminal[]){ {"IDENT", "ATRIBSTAT->IDENT"} }, 1},
{"ATRIBSTAT'", (MapeamentoTerminal[]){
    {"CALL", "ATRIB'->FUNCCALL"},
    {"IDENT", "ATRIB'->EXPRESSION"},
    {"MINUS", "ATRIB'->EXPRESSION"},
    {"NEW", "ATRIB'->ALLOCEXPR"},
    {"NI", "ATRIB'->EXPRESSION"},
    {"NPF", "ATRIB'->EXPRESSION"},
    {"NULL", "ATRIB'->EXPRESSION"},
    {"OPEN_PARENTHESIS", "ATRIB'->EXPRESSION"},
    {"PLUS", "ATRIB'->EXPRESSION"},
    {"STRC", "ATRIB'->EXPRESSION"}
}, 10},
{"CLOSE_BRACE", (MapeamentoTerminal[]){ {"$", "CLOSE_BRACE->EPSILON"} }, 1},
{"EXPRESSION", (MapeamentoTerminal[]){
    {"IDENT", "EXPR->NUMEXPR"},
    {"MINUS", "EXPR->NUMEXPR"},
    {"NI", "EXPR->NUMEXPR"},
    {"NPF", "EXPR->NUMEXPR"},
    {"NULL", "EXPR->NUMEXPR"},
    {"OPEN_PARENTHESIS", "EXPR->NUMEXPR"},
    {"PLUS", "EXPR->NUMEXPR"},
    {"STRC", "EXPR->NUMEXPR"}
}, 8},
{"EXPRESSION'", (MapeamentoTerminal[]){
    {"CLOSE_PARENTHESIS", "EXPR'->EPSILON"},
    {"RELOP", "EXPR'->RELOP"},
    {"SEMICOLON", "EXPR'->EPSILON"}
}, 3},
{"FACTOR", (MapeamentoTerminal[]){
    {"IDENT", "FACTOR->LVALUE"},
    {"NI", "FACTOR->CONST_NI"},
    {"NPF", "FACTOR->CONST_NPF"},
    {"NULL", "FACTOR->CONST_NULL"},
    {"OPEN_PARENTHESIS", "FACTOR->PARENTHESIS"},
    {"STRC", "FACTOR->CONST_STRC"}
}, 6},
{"FORSTAT", (MapeamentoTerminal[]){ {"FOR", "FORSTAT->FOR"} }, 1},
{"FUNCCALL", (MapeamentoTerminal[]){ {"CALL", "FUNCCALL->CALL"} }, 1},
{"FUNCDEF", (MapeamentoTerminal[]){ {"DEF", "FUNCDEF->DEF"} }, 1},
{"FUNCLIST", (MapeamentoTerminal[]){ {"DEF", "FUNCLIST->DEF"} }, 1},
{"FUNCLIST'", (MapeamentoTerminal[]){
    {"$", "FUNCLIST'->EPSILON"},
    {"BREAK", "FUNCLIST'->EPSILON"},
    {"DEF", "FUNCLIST'->DEF"},
    {"FLOAT", "FUNCLIST'->EPSILON"},
    {"FOR", "FUNCLIST'->EPSILON"},
    {"IDENT", "FUNCLIST'->EPSILON"},
    {"IF", "FUNCLIST'->EPSILON"},
    {"INT", "FUNCLIST'->EPSILON"},
    {"ONECHAR", "FUNCLIST'->EPSILON"},
    {"PRINT", "FUNCLIST'->EPSILON"},
    {"READ", "FUNCLIST'->EPSILON"},
    {"RETURN", "FUNCLIST'->EPSILON"},
    {"SEMICOLON", "FUNCLIST'->EPSILON"},
    {"STRING", "FUNCLIST'->EPSILON"}
}, 14},
{"IFSTAT", (MapeamentoTerminal[]){ {"IF", "IFSTAT->IF"} }, 1},
{"IFSTAT'", (MapeamentoTerminal[]){
    {"$", "IFSTAT'->EPSILON"},
    {"BREAK", "IFSTAT'->EPSILON"},
    {"CLOSE_BRACE", "IFSTAT'->EPSILON"},
    {"ELSE", "IFSTAT'->ELSE"},
    {"FLOAT", "IFSTAT'->EPSILON"},
    {"FOR", "IFSTAT'->EPSILON"},
    {"IDENT", "IFSTAT'->EPSILON"},
    {"IF", "IFSTAT'->EPSILON"},
    {"INT", "IFSTAT'->EPSILON"},
    {"ONECHAR", "IFSTAT'->EPSILON"},
    {"PRINT", "IFSTAT'->EPSILON"},
    {"READ", "IFSTAT'->EPSILON"},
    {"RETURN", "IFSTAT'->EPSILON"},
    {"SEMICOLON", "IFSTAT'->EPSILON"},
    {"STRING", "IFSTAT'->EPSILON"}
}, 15},
{"INDEX", (MapeamentoTerminal[]){ {"OPEN_BRACKET", "INDEX->BRACKET"}, {"SEMICOLON", "INDEX->EPSILON"} }, 2},
{"LVALUE", (MapeamentoTerminal[]){ {"IDENT", "LVALUE->IDENT"} }, 1},
{"NUMEXPRESSION", (MapeamentoTerminal[]){
    {"IDENT", "NUMEXPR->TERM"},
    {"MINUS", "NUMEXPR->TERM"},
    {"NI", "NUMEXPR->TERM"},
    {"NPF", "NUMEXPR->TERM"},
    {"NULL", "NUMEXPR->TERM"},
    {"OPEN_PARENTHESIS", "NUMEXPR->TERM"},
    {"PLUS", "NUMEXPR->TERM"},
    {"STRC", "NUMEXPR->TERM"}
}, 8},
{"NUMEXPRESSIONAUX", (MapeamentoTerminal[]){
    {"CLOSE_BRACKET", "NUMEXPR_AUX->EPSILON"},
    {"CLOSE_PARENTHESIS", "NUMEXPR_AUX->EPSILON"},
    {"MINUS", "NUMEXPR_AUX->MINUS"},
    {"PLUS", "NUMEXPR_AUX->PLUS"},
    {"RELOP", "NUMEXPR_AUX->EPSILON"},
    {"SEMICOLON", "NUMEXPR_AUX->EPSILON"}
}, 6},
{"PARAMLIST", (MapeamentoTerminal[]){
    {"CLOSE_PARENTHESIS", "PARAMLIST->EPSILON"},
    {"FLOAT", "PARAMLIST->TYPE"},
    {"INT", "PARAMLIST->TYPE"},
    {"STRING", "PARAMLIST->TYPE"}
}, 4},
{"PARAMLIST'", (MapeamentoTerminal[]){ {"CLOSE_PARENTHESIS", "PARAMLIST'->EPSILON"}, {"COMMA", "PARAMLIST'->COMMA"} }, 2},
{"PARAMLISTCALL", (MapeamentoTerminal[]){ {"CLOSE_PARENTHESIS", "PARAMLISTCALL->EPSILON"}, {"IDENT", "PARAMLISTCALL->IDENT"} }, 2},
{"PARAMLISTCALL'", (MapeamentoTerminal[]){ {"CLOSE_PARENTHESIS", "PARAMLISTCALL'->EPSILON"}, {"COMMA", "PARAMLISTCALL'->COMMA"} }, 2},
{"PRINTSTAT", (MapeamentoTerminal[]){ {"PRINT", "PRINTSTAT->PRINT"} }, 1},
{"PROGRAM", (MapeamentoTerminal[]){
    {"$", "PROG->EPSILON"},
    {"BREAK", "PROG->STATEMENT"},
    {"DEF", "PROG->FUNCLIST"},
    {"FLOAT", "PROG->STATEMENT"},
    {"FOR", "PROG->STATEMENT"},
    {"IDENT", "PROG->STATEMENT"},
    {"IF", "PROG->STATEMENT"},
    {"INT", "PROG->STATEMENT"},
    {"ONECHAR", "PROG->STATEMENT"},
    {"PRINT", "PROG->STATEMENT"},
    {"READ", "PROG->STATEMENT"},
    {"RETURN", "PROG->STATEMENT"},
    {"SEMICOLON", "PROG->STATEMENT"},
    {"STRING", "PROG->STATEMENT"}
}, 14},
{"PROGRAM'", (MapeamentoTerminal[]){
    {"$", "PROG'->EPSILON"},
    {"BREAK", "PROG'->STATEMENT"},
    {"DEF", "PROG'->FUNCLIST"},
    {"FLOAT", "PROG'->STATEMENT"},
    {"FOR", "PROG'->STATEMENT"},
    {"IDENT", "PROG'->STATEMENT"},
    {"IF", "PROG'->STATEMENT"},
    {"INT", "PROG'->STATEMENT"},
    {"ONECHAR", "PROG'->STATEMENT"},
    {"PRINT", "PROG'->STATEMENT"},
    {"READ", "PROG'->STATEMENT"},
    {"RETURN", "PROG'->STATEMENT"},
    {"SEMICOLON", "PROG'->STATEMENT"},
    {"STRING", "PROG'->STATEMENT"}
}, 14},
{"READSTAT", (MapeamentoTerminal[]){ {"READ", "READSTAT->READ"} }, 1},
{"RETURNSTAT", (MapeamentoTerminal[]){ {"RETURN", "RETURNSTAT->RETURN"} }, 1},
{"RETURNSTAT'", (MapeamentoTerminal[]){ {"IDENT", "RETURN'->LVALUE"}, {"SEMICOLON", "RETURN'->EPSILON"} }, 2},
{"STATELIST", (MapeamentoTerminal[]){
    {"$", "STATELIST->EPSILON"},
    {"BREAK", "STATELIST->STMT"},
    {"FLOAT", "STATELIST->STMT"},
    {"FOR", "STATELIST->STMT"},
    {"IDENT", "STATELIST->STMT"},
    {"IF", "STATELIST->STMT"},
    {"INT", "STATELIST->STMT"},
    {"ONECHAR", "STATELIST->STMT"},
    {"PRINT", "STATELIST->STMT"},
    {"READ", "STATELIST->STMT"},
    {"RETURN", "STATELIST->STMT"},
    {"SEMICOLON", "STATELIST->STMT"},
    {"STRING", "STATELIST->STMT"}
}, 13},
{"STATELIST'", (MapeamentoTerminal[]){
    {"$", "STATELIST'->EPSILON"},
    {"BREAK", "STATELIST'->STMT"},
    {"CLOSE_BRACE", "STATELIST'->EPSILON"},
    {"FLOAT", "STATELIST'->STMT"},
    {"FOR", "STATELIST'->STMT"},
    {"IDENT", "STATELIST'->STMT"},
    {"IF", "STATELIST'->STMT"},
    {"INT", "STATELIST'->STMT"},
    {"ONECHAR", "STATELIST'->STMT"},
    {"PRINT", "STATELIST'->STMT"},
    {"READ", "STATELIST'->STMT"},
    {"RETURN", "STATELIST'->STMT"},
    {"SEMICOLON", "STATELIST'->STMT"},
    {"STRING", "STATELIST'->STMT"}
}, 14},
{"STATEMENT", (MapeamentoTerminal[]){
    {"BREAK", "STMT->BREAK"},
    {"FLOAT", "STMT->VARDECL"},
    {"FOR", "STMT->FOR"},
    {"IDENT", "STMT->ATRIBSTAT"},
    {"IF", "STMT->IF"},
    {"INT", "STMT->VARDECL"},
    {"ONECHAR", "STMT->BLOCK"},
    {"PRINT", "STMT->PRINT"},
    {"READ", "STMT->READ"},
    {"RETURN", "STMT->RETURN"},
    {"SEMICOLON", "STMT->EMPTY"},
    {"STRING", "STMT->VARDECL"}
}, 12},
{"TERM", (MapeamentoTerminal[]){
    {"IDENT", "TERM->UNARY"},
    {"MINUS", "TERM->UNARY"},
    {"NI", "TERM->UNARY"},
    {"NPF", "TERM->UNARY"},
    {"NULL", "TERM->UNARY"},
    {"OPEN_PARENTHESIS", "TERM->UNARY"},
    {"PLUS", "TERM->UNARY"},
    {"STRC", "TERM->UNARY"}
}, 8},
{"UNARYEXPR", (MapeamentoTerminal[]){
    {"IDENT", "UNARY->FACTOR"},
    {"MINUS", "UNARY->PLUS_MINUS"},
    {"NI", "UNARY->FACTOR"},
    {"NPF", "UNARY->FACTOR"},
    {"NULL", "UNARY->FACTOR"},
    {"OPEN_PARENTHESIS", "UNARY->FACTOR"},
    {"PLUS", "UNARY->PLUS_MINUS"},
    {"STRC", "UNARY->FACTOR"}
}, 8},
{"UNARYEXPRAUX", (MapeamentoTerminal[]){
    {"CLOSE_BRACKET", "UNARY_AUX->EPSILON"},
    {"CLOSE_PARENTHESIS", "UNARY_AUX->EPSILON"},
    {"DIVISION", "UNARY_AUX->DIV"},
    {"MINUS", "UNARY_AUX->EPSILON"},
    {"MODULUS", "UNARY_AUX->MOD"},
    {"MULTIPLICATION", "UNARY_AUX->MULT"},
    {"PLUS", "UNARY_AUX->EPSILON"},
    {"RELOP", "UNARY_AUX->EPSILON"},
    {"SEMICOLON", "UNARY_AUX->EPSILON"}
}, 9},
{"VARDECL", (MapeamentoTerminal[]){
    {"FLOAT", "VARDECL->FLOAT"},
    {"INT", "VARDECL->INT"},
    {"STRING", "VARDECL->STRING"}
}, 3}
};

const size_t num_entradas_tabela = sizeof(tabela_analise) / sizeof(EntradaTabelaAnalise);

const EntradaTabelaSDT* buscar_entrada_sdt(const char* chave) {
if (!chave) return NULL;
for (size_t i = 0; i < num_entradas_sdt; i++) {
    if (strcmp(tabela_sdt_entries[i].chave, chave) == 0) {
        return &tabela_sdt_entries[i];
    }
}
return NULL;
}