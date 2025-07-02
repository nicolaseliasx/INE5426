#include "identificadores_tokens.h"
#include <ctype.h> 
#include <string.h>
#include <stdlib.h>
#include "token.h"

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
    if (eof) return "final_consumido"; // Fim de arquivo, consome tudo
    if (eh_letra(c) || eh_digito(c) || c == '_') return "1";
    
    // Se o caractere não for válido, o token terminou.
    // Retornamos um estado que significa "sucesso, mas retroceda o cursor".
    return "final_retrocede";
}

// Identificador de números inteiros (NI)
static const char* estado_inteiro_0(char c, int eof) {
    if (eof) return "morto";
    if (eh_digito(c)) return "1";
    return "morto";
}

static const char* estado_inteiro_1(char c, int eof) {
    if (eof) return "final_consumido";
    if (eh_digito(c)) return "1";
    if (c == 'e') return "2";
    return "final_retrocede";
}

static const char* estado_inteiro_2(char c, int eof) {
    if (eof) return "morto";
    if (eh_digito(c)) return "3";
    if (c == '+' || c == '-') return "4";
    return "morto";
}

static const char* estado_inteiro_3(char c, int eof) {
    if (eof) return "final_consumido";
    if (eh_digito(c)) return "3";
    return "final_retrocede";
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
    if (eof) return "final_consumido";
    if (eh_digito(c)) return "2";
    if (c == 'e') return "3";
    return "final_retrocede";
}

static const char* estado_float_3(char c, int eof) {
    if (eof) return "morto";
    if (eh_digito(c)) return "4";
    if (c == '+' || c == '-') return "5";
    return "morto";
}

static const char* estado_float_4(char c, int eof) {
    if (eof) return "final_consumido";
    if (eh_digito(c)) return "4";
    return "final_retrocede";
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

static const char* estados_finais_ident[] = {"final_consumido", "final_retrocede"};
static const char* estados_mortos_ident[] = {"morto"};
static const char* estados_retrocesso_ident[] = {"final_retrocede"};

static MaquinaEstados maquina_ident = {
    .transicoes = transicoes_ident,
    .num_transicoes = sizeof(transicoes_ident) / sizeof(TransicaoEstado),
    .estado_inicial = "0",
    .estados_finais = estados_finais_ident,
    .num_estados_finais = sizeof(estados_finais_ident) / sizeof(const char*),
    .estados_mortos = estados_mortos_ident,
    .num_estados_mortos = sizeof(estados_mortos_ident) / sizeof(const char*),
    .estados_retrocesso = estados_retrocesso_ident,
    .num_estados_retrocesso = sizeof(estados_retrocesso_ident) / sizeof(const char*)
};

// FSM para números inteiros (NI)
static TransicaoEstado transicoes_inteiro[] = {
    {"0", estado_inteiro_0},
    {"1", estado_inteiro_1},
    {"2", estado_inteiro_2},
    {"3", estado_inteiro_3},
    {"4", estado_inteiro_4}
};

static const char* estados_finais_inteiro[] = {"final_consumido", "final_retrocede"};
static const char* estados_mortos_inteiro[] = {"morto"};
static const char* estados_retrocesso_inteiro[] = {"final_retrocede"};

static MaquinaEstados maquina_inteiro = {
    .transicoes = transicoes_inteiro,
    .num_transicoes = sizeof(transicoes_inteiro) / sizeof(TransicaoEstado),
    .estado_inicial = "0",
    .estados_finais = estados_finais_inteiro,
    .num_estados_finais = sizeof(estados_finais_inteiro) / sizeof(const char*),
    .estados_mortos = estados_mortos_inteiro,
    .num_estados_mortos = sizeof(estados_mortos_inteiro) / sizeof(const char*),
    .estados_retrocesso = estados_retrocesso_inteiro,
    .num_estados_retrocesso = sizeof(estados_retrocesso_inteiro) / sizeof(const char*)
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

static const char* estados_finais_float[] = {"final_consumido", "final_retrocede"};
static const char* estados_mortos_float[] = {"morto"};
static const char* estados_retrocesso_float[] = {"final_retrocede"};

static MaquinaEstados maquina_float = {
    .transicoes = transicoes_float,
    .num_transicoes = sizeof(transicoes_float) / sizeof(TransicaoEstado),
    .estado_inicial = "0",
    .estados_finais = estados_finais_float,
    .num_estados_finais = sizeof(estados_finais_float) / sizeof(const char*),
    .estados_mortos = estados_mortos_float,
    .num_estados_mortos = sizeof(estados_mortos_float) / sizeof(const char*),
    .estados_retrocesso = estados_retrocesso_float,
    .num_estados_retrocesso = sizeof(estados_retrocesso_float) / sizeof(const char*)
};

// FSM para operadores matemáticos (MATHOP)
static TransicaoEstado transicoes_mathop[] = {
    {"0", estado_mathop_0}
};

static const char* estados_finais_mathop[] = {"final"};
static const char* estados_mortos_mathop[] = {"morto"};

static MaquinaEstados maquina_mathop = {
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

static MaquinaEstados maquina_onechar = {
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

static MaquinaEstados maquina_strc = {
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

static const char* estados_retrocesso_relop[] = {"final_1"};

static MaquinaEstados maquina_relop = {
    transicoes_relop,
    sizeof(transicoes_relop) / sizeof(TransicaoEstado),
    "0",
    estados_finais_relop,
    sizeof(estados_finais_relop) / sizeof(const char*),
    estados_mortos_relop,
    sizeof(estados_mortos_relop) / sizeof(const char*),
    estados_retrocesso_relop,
    sizeof(estados_retrocesso_relop) / sizeof(const char*)
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