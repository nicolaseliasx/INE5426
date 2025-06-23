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