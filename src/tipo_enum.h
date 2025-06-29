#ifndef TIPO_ENUM_H
#define TIPO_ENUM_H

#include <stdio.h>

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