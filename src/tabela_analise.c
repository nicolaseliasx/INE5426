// Arquivo: tabela_analise.c (VERSÃO REATORADA E EFICIENTE)

#include "tabela_analise.h"

//
// TABELA DE ANÁLISE PREDITIVA (LL) APONTANDO PARA AS REGRAS UNIFICADAS
//
const EntradaTabelaAnalise tabela_analise[] = {
    {"PROGRAM", (MapeamentoTerminal[]){ 
        {"DEF", "PROG->FUNCLIST"}, 
        {"ONECHAR", "PROG->STATEMENT"}, {"IDENT", "PROG->STATEMENT"}, {"INT", "PROG->STATEMENT"}, 
        {"FLOAT", "PROG->STATEMENT"}, {"STRING", "PROG->STATEMENT"}, {"SEMICOLON", "PROG->STATEMENT"}, 
        {"BREAK", "PROG->STATEMENT"}, {"PRINT", "PROG->STATEMENT"}, {"READ", "PROG->STATEMENT"}, 
        {"RETURN", "PROG->STATEMENT"}, {"IF", "PROG->STATEMENT"}, {"FOR", "PROG->STATEMENT"}, 
        {"$", "PROG->EPSILON"} 
    }, 14 },
    {"PROGRAM'", (MapeamentoTerminal[]){ 
        {"DEF", "PROG'->FUNCLIST"}, 
        {"ONECHAR", "PROG'->STATEMENT"}, {"IDENT", "PROG'->STATEMENT"}, {"INT", "PROG'->STATEMENT"}, 
        {"FLOAT", "PROG'->STATEMENT"}, {"STRING", "PROG'->STATEMENT"}, {"SEMICOLON", "PROG'->STATEMENT"}, 
        {"BREAK", "PROG'->STATEMENT"}, {"PRINT", "PROG'->STATEMENT"}, {"READ", "PROG'->STATEMENT"}, 
        {"RETURN", "PROG'->STATEMENT"}, {"IF", "PROG'->STATEMENT"}, {"FOR", "PROG'->STATEMENT"}, 
        {"$", "PROG'->EPSILON"} 
    }, 14 },
    {"FUNCLIST", (MapeamentoTerminal[]){ {"DEF", "FUNCLIST->DEF"} }, 1 },
    {"FUNCLIST'", (MapeamentoTerminal[]){ 
        {"DEF", "FUNCLIST'->DEF"}, 
        {"ONECHAR", "FUNCLIST'->EPSILON"}, {"IDENT", "FUNCLIST'->EPSILON"}, {"INT", "FUNCLIST'->EPSILON"}, 
        {"FLOAT", "FUNCLIST'->EPSILON"}, {"STRING", "FUNCLIST'->EPSILON"}, {"SEMICOLON", "FUNCLIST'->EPSILON"}, 
        {"BREAK", "FUNCLIST'->EPSILON"}, {"PRINT", "FUNCLIST'->EPSILON"}, {"READ", "FUNCLIST'->EPSILON"}, 
        {"RETURN", "FUNCLIST'->EPSILON"}, {"IF", "FUNCLIST'->EPSILON"}, {"FOR", "FUNCLIST'->EPSILON"}, 
        {"$", "FUNCLIST'->EPSILON"} 
    }, 14 },
    {"FUNCDEF", (MapeamentoTerminal[]){ {"DEF", "FUNCDEF->DEF"} }, 1 },
    {"PARAMLIST", (MapeamentoTerminal[]){ 
        {"CLOSE_PARENTHESIS", "PARAMLIST->EPSILON"}, 
        {"INT", "PARAMLIST->TYPE"}, // Chave unificada
        {"FLOAT", "PARAMLIST->TYPE"}, // Chave unificada
        {"STRING", "PARAMLIST->TYPE"} // Chave unificada
    }, 4 },
    {"PARAMLIST'", (MapeamentoTerminal[]){ 
        {"CLOSE_PARENTHESIS", "PARAMLIST'->EPSILON"}, 
        {"COMMA", "PARAMLIST'->COMMA"} 
    }, 2 },
    {"STATEMENT", (MapeamentoTerminal[]){ 
        {"IDENT", "STMT->ATRIBSTAT"}, {"ONECHAR", "STMT->BLOCK"}, 
        {"INT", "STMT->VARDECL"}, // Chave unificada
        {"FLOAT", "STMT->VARDECL"}, // Chave unificada
        {"STRING", "STMT->VARDECL"}, // Chave unificada
        {"SEMICOLON", "STMT->EMPTY"}, {"BREAK", "STMT->BREAK"}, {"PRINT", "STMT->PRINT"}, 
        {"READ", "STMT->READ"}, {"RETURN", "STMT->RETURN"}, {"IF", "STMT->IF"}, {"FOR", "STMT->FOR"} 
    }, 12 },
    {"VARDECL", (MapeamentoTerminal[]){ 
        {"INT", "VARDECL->INT"}, 
        {"FLOAT", "VARDECL->FLOAT"}, 
        {"STRING", "VARDECL->STRING"} 
    }, 3 },
    {"INDEX", (MapeamentoTerminal[]){ 
        {"SEMICOLON", "INDEX->EPSILON"}, 
        {"OPEN_BRACKET", "INDEX->BRACKET"} 
    }, 2 },
    {"ATRIBSTAT", (MapeamentoTerminal[]){ {"IDENT", "ATRIBSTAT->IDENT"} }, 1 },
    {"ATRIBSTAT'", (MapeamentoTerminal[]){ 
        {"IDENT", "ATRIB'->EXPRESSION"}, {"OPEN_PARENTHESIS", "ATRIB'->EXPRESSION"}, 
        {"NI", "ATRIB'->EXPRESSION"}, {"PLUS", "ATRIB'->EXPRESSION"}, {"MINUS", "ATRIB'->EXPRESSION"}, 
        {"NPF", "ATRIB'->EXPRESSION"}, {"STRC", "ATRIB'->EXPRESSION"}, {"NULL", "ATRIB'->EXPRESSION"}, 
        {"CALL", "ATRIB'->FUNCCALL"}, {"NEW", "ATRIB'->ALLOCEXPR"} 
    }, 10 },
    {"FUNCCALL", (MapeamentoTerminal[]){ {"CALL", "FUNCCALL->CALL"} }, 1 },
    {"PARAMLISTCALL", (MapeamentoTerminal[]){ 
        {"IDENT", "PARAMLISTCALL->IDENT"}, 
        {"CLOSE_PARENTHESIS", "PARAMLISTCALL->EPSILON"} 
    }, 2 },
    {"PARAMLISTCALL'", (MapeamentoTerminal[]){ 
        {"CLOSE_PARENTHESIS", "PARAMLISTCALL'->EPSILON"}, 
        {"COMMA", "PARAMLISTCALL'->COMMA"} 
    }, 2 },
    {"PRINTSTAT", (MapeamentoTerminal[]){ {"PRINT", "PRINTSTAT->PRINT"} }, 1 },
    {"READSTAT", (MapeamentoTerminal[]){ {"READ", "READSTAT->READ"} }, 1 },
    {"RETURNSTAT", (MapeamentoTerminal[]){ {"RETURN", "RETURNSTAT->RETURN"} }, 1 },
    {"RETURNSTAT'", (MapeamentoTerminal[]){ 
        {"IDENT", "RETURN'->LVALUE"}, 
        {"SEMICOLON", "RETURN'->EPSILON"} 
    }, 2 },
    {"IFSTAT", (MapeamentoTerminal[]){ {"IF", "IFSTAT->IF"} }, 1 },
    {"IFSTAT'", (MapeamentoTerminal[]){ 
        {"ELSE", "IFSTAT'->ELSE"}, 
        {"ONECHAR", "IFSTAT'->EPSILON"}, {"IDENT", "IFSTAT'->EPSILON"}, {"CLOSE_BRACE", "IFSTAT'->EPSILON"}, 
        {"INT", "IFSTAT'->EPSILON"}, {"FLOAT", "IFSTAT'->EPSILON"}, {"STRING", "IFSTAT'->EPSILON"}, 
        {"SEMICOLON", "IFSTAT'->EPSILON"}, {"BREAK", "IFSTAT'->EPSILON"}, {"PRINT", "IFSTAT'->EPSILON"}, 
        {"READ", "IFSTAT'->EPSILON"}, {"RETURN", "IFSTAT'->EPSILON"}, {"IF", "IFSTAT'->EPSILON"}, 
        {"FOR", "IFSTAT'->EPSILON"}, {"$", "IFSTAT'->EPSILON"} 
    }, 15 },
    {"FORSTAT", (MapeamentoTerminal[]){ {"FOR", "FORSTAT->FOR"} }, 1 },
    {"STATELIST", (MapeamentoTerminal[]){ 
        {"ONECHAR", "STATELIST->STMT"}, {"IDENT", "STATELIST->STMT"}, {"INT", "STATELIST->STMT"},
        {"FLOAT", "STATELIST->STMT"}, {"STRING", "STATELIST->STMT"}, {"SEMICOLON", "STATELIST->STMT"},
        {"BREAK", "STATELIST->STMT"}, {"PRINT", "STATELIST->STMT"}, {"READ", "STATELIST->STMT"},
        {"RETURN", "STATELIST->STMT"}, {"IF", "STATELIST->STMT"}, {"FOR", "STATELIST->STMT"}
    }, 12 },
    {"STATELIST'", (MapeamentoTerminal[]){ 
        {"CLOSE_BRACE", "STATELIST'->EPSILON"}, 
        {"ONECHAR", "STATELIST'->STMT"}, {"IDENT", "STATELIST'->STMT"}, {"INT", "STATELIST'->STMT"}, 
        {"FLOAT", "STATELIST'->STMT"}, {"STRING", "STATELIST'->STMT"}, {"SEMICOLON", "STATELIST'->STMT"}, 
        {"BREAK", "STATELIST'->STMT"}, {"PRINT", "STATELIST'->STMT"}, {"READ", "STATELIST'->STMT"}, 
        {"RETURN", "STATELIST'->STMT"}, {"IF", "STATELIST'->STMT"}, {"FOR", "STATELIST'->STMT"} 
    }, 13 },
    {"ALLOCEXPRESSION", (MapeamentoTerminal[]){ {"NEW", "ALLOCEXPR->NEW"} }, 1 },
    {"ALLOCEXPRESSION'", (MapeamentoTerminal[]){ 
        {"INT", "ALLOCEXPR'->TYPE"}, // Chave unificada
        {"FLOAT", "ALLOCEXPR'->TYPE"}, // Chave unificada
        {"STRING", "ALLOCEXPR'->TYPE"} // Chave unificada
    }, 3 },
    {"ALLOCAUX", (MapeamentoTerminal[]){ 
        {"OPEN_BRACKET", "ALLOCAUX->BRACKET"}, 
        {"CLOSE_PARENTHESIS", "ALLOCAUX->EPSILON"}, {"SEMICOLON", "ALLOCAUX->EPSILON"}, 
        {"CLOSE_BRACKET", "ALLOCAUX->EPSILON"}, {"ASSIGN", "ALLOCAUX->EPSILON"}, {"RELOP", "ALLOCAUX->EPSILON"}, 
        {"PLUS", "ALLOCAUX->EPSILON"}, {"MINUS", "ALLOCAUX->EPSILON"}, {"MULTIPLICATION", "ALLOCAUX->EPSILON"}, 
        {"DIVISION", "ALLOCAUX->EPSILON"}, {"MODULUS", "ALLOCAUX->EPSILON"} 
    }, 11 },
    {"EXPRESSION", (MapeamentoTerminal[]){ 
        {"IDENT", "EXPR->NUMEXPR"}, {"OPEN_PARENTHESIS", "EXPR->NUMEXPR"}, {"NI", "EXPR->NUMEXPR"}, 
        {"PLUS", "EXPR->NUMEXPR"}, {"MINUS", "EXPR->NUMEXPR"}, {"NPF", "EXPR->NUMEXPR"}, 
        {"STRC", "EXPR->NUMEXPR"}, {"NULL", "EXPR->NUMEXPR"} 
    }, 8 },
    {"EXPRESSION'", (MapeamentoTerminal[]){ 
        {"CLOSE_PARENTHESIS", "EXPR'->EPSILON"}, 
        {"SEMICOLON", "EXPR'->EPSILON"}, 
        {"RELOP", "EXPR'->RELOP"} 
    }, 3 },
    {"NUMEXPRESSION", (MapeamentoTerminal[]){ 
        {"IDENT", "NUMEXPR->TERM"}, {"OPEN_PARENTHESIS", "NUMEXPR->TERM"}, {"NI", "NUMEXPR->TERM"}, 
        {"PLUS", "NUMEXPR->TERM"}, {"MINUS", "NUMEXPR->TERM"}, {"NPF", "NUMEXPR->TERM"}, 
        {"STRC", "NUMEXPR->TERM"}, {"NULL", "NUMEXPR->TERM"} 
    }, 8 },
    {"NUMEXPRESSIONAUX", (MapeamentoTerminal[]){ 
        {"PLUS", "NUMEXPR_AUX->OP"}, // Chave unificada
        {"MINUS", "NUMEXPR_AUX->OP"}, // Chave unificada
        {"CLOSE_PARENTHESIS", "NUMEXPR_AUX->EPSILON"}, {"SEMICOLON", "NUMEXPR_AUX->EPSILON"}, 
        {"CLOSE_BRACKET", "NUMEXPR_AUX->EPSILON"}, {"RELOP", "NUMEXPR_AUX->EPSILON"} 
    }, 6 },
    {"TERM", (MapeamentoTerminal[]){ 
        {"IDENT", "TERM->UNARY"}, {"OPEN_PARENTHESIS", "TERM->UNARY"}, {"NI", "TERM->UNARY"}, 
        {"PLUS", "TERM->UNARY"}, {"MINUS", "TERM->UNARY"}, {"NPF", "TERM->UNARY"}, 
        {"STRC", "TERM->UNARY"}, {"NULL", "TERM->UNARY"} 
    }, 8 },
    {"UNARYEXPRAUX", (MapeamentoTerminal[]){ 
        {"MULTIPLICATION", "UNARY_AUX->OP"}, // Chave unificada
        {"DIVISION", "UNARY_AUX->OP"}, // Chave unificada
        {"MODULUS", "UNARY_AUX->OP"}, // Chave unificada
        {"CLOSE_PARENTHESIS", "UNARY_AUX->EPSILON"}, {"SEMICOLON", "UNARY_AUX->EPSILON"}, 
        {"CLOSE_BRACKET", "UNARY_AUX->EPSILON"}, {"RELOP", "UNARY_AUX->EPSILON"}, 
        {"PLUS", "UNARY_AUX->EPSILON"}, {"MINUS", "UNARY_AUX->EPSILON"} 
    }, 9 },
    {"UNARYEXPR", (MapeamentoTerminal[]){ 
        {"PLUS", "UNARY->PLUS_MINUS"}, {"MINUS", "UNARY->PLUS_MINUS"}, 
        {"IDENT", "UNARY->FACTOR"}, {"OPEN_PARENTHESIS", "UNARY->FACTOR"}, {"NI", "UNARY->FACTOR"}, 
        {"NPF", "UNARY->FACTOR"}, {"STRC", "UNARY->FACTOR"}, {"NULL", "UNARY->FACTOR"} 
    }, 8 },
    {"FACTOR", (MapeamentoTerminal[]){ 
        {"IDENT", "FACTOR->LVALUE"}, {"OPEN_PARENTHESIS", "FACTOR->PARENTHESIS"}, 
        {"NI", "FACTOR->CONST"}, // Chave unificada
        {"NPF", "FACTOR->CONST"}, // Chave unificada
        {"STRC", "FACTOR->CONST"}, // Chave unificada
        {"NULL", "FACTOR->CONST_NULL"} 
    }, 6 },
    {"LVALUE", (MapeamentoTerminal[]){ {"IDENT", "LVALUE->IDENT"} }, 1 }
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