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
        snprintf(novo_prefixo, sizeof(novo_prefixo), "%s%s", prefixo, eh_esquerda ? "│   " : "     ");
        
        // Chamar recursivamente para filhos
        imprimir_arvore_prefixo(novo_prefixo, no->filhoA, 1);
        imprimir_arvore_prefixo(novo_prefixo, no->filhoB, 0);
    }
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