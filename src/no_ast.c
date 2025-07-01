#define _DEFAULT_SOURCE
#include "no_ast.h"
#include "erros.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


NoAST* criar_no_ast(const char* identificador, int* contador_rotulos) {
    NoAST* no = (NoAST*)malloc(sizeof(NoAST));
    if (!no) {
        // Lidar com erro de alocação
        exit(EXIT_FAILURE);
    }
    
    no->identificador = strdup(identificador);
    no->token = NULL;
    
    no->filhos = NULL;
    no->quantidade_filhos = 0;
    no->capacidade_filhos = 0;
    
    no->codigo = criar_lista_string();
    no->proximo = NULL;
    
    no->para_proximo = NULL;
    
    no->sdt_dec.tipo_inicial = NULL;
    no->sdt_dec.tipo = NULL;
    
    no->res_var_codigo.var = NULL;
    no->res_var_codigo.inicio = NULL;
    no->res_var_codigo.ret = NULL;
    no->res_var_codigo.valor_retorno = strdup("0");
    no->res_var_codigo.contador_param = 0;
    
    no->sdt_mat.contador_vetor = 0;
    no->sdt_mat.vetor_array_var = criar_lista_string();
    no->sdt_mat.operacao = '\0';
    no->sdt_mat.no = NULL;
    
    no->contador_rotulos = contador_rotulos;
    
    return no;
}

void adicionar_filho(NoAST* self, NoAST* filho) {
    if (self->quantidade_filhos >= self->capacidade_filhos) {
        int nova_capacidade = self->capacidade_filhos == 0 ? 4 : self->capacidade_filhos * 2;
        NoAST** novos_filhos = (NoAST**)realloc(self->filhos, sizeof(NoAST*) * nova_capacidade);
        if (!novos_filhos) {
            fprintf(stderr, "Erro de alocação de memória ao adicionar filho.\n");
            exit(EXIT_FAILURE);
        }
        self->filhos = novos_filhos;
        self->capacidade_filhos = nova_capacidade;
    }
    self->filhos[self->quantidade_filhos++] = filho;
}

void definir_filhos(NoAST* self, NoAST** novos_filhos, int quantidade) {
    if (self->filhos) {
        for(int i = 0; i < self->quantidade_filhos; ++i) {
            liberar_no_ast(self->filhos[i]);
        }
        free(self->filhos);
    }
    
    self->filhos = novos_filhos;
    self->quantidade_filhos = quantidade;
    self->capacidade_filhos = quantidade;
}

void definir_token(NoAST* self, Token* token) {
    self->token = token; // A posse do token é transferida para o nó
}

char* no_ast_gerar_novo_rotulo(NoAST* self) {
    (*self->contador_rotulos)++;
    char* rotulo = (char*)malloc(20 * sizeof(char));
    snprintf(rotulo, 20, "L%d", *self->contador_rotulos);
    return rotulo;
}

NoExpressao* criar_no_expressao_unario(char operacao, const char* valor, NoExpressao* filhoA) {
    NoExpressao* no = (NoExpressao*)malloc(sizeof(NoExpressao));
    no->operacao = operacao;
    no->valor = strdup(valor);
    no->filhoA = filhoA;
    no->filhoB = NULL;
    if (filhoA) {
        no->tipo = strdup(filhoA->tipo);
    } else {
        no->tipo = strdup("");
    }
    return no;
}

// Em no_ast.c
NoExpressao* criar_no_expressao_binario(char operacao, const char* valor, NoExpressao* filhoA, NoExpressao* filhoB) {
    if (!filhoA || !filhoB) {
        fprintf(stderr, "ERRO: Tentativa de criar nó binário com filhos nulos\n");
        return NULL;
    }

    NoExpressao* no = (NoExpressao*)malloc(sizeof(NoExpressao));
    if (!no) exit(EXIT_FAILURE);

    if (filhoA && filhoB && strcmp(filhoA->tipo, filhoB->tipo) != 0) {
        // Tipos são diferentes, reporta o erro e encerra.
        imprimir_erro_tipo(operacao, filhoA, filhoB);
        // Em C, usamos exit() para simular o comportamento de 'throw' que para a execução.
        exit(EXIT_FAILURE);
    }

    no->operacao = operacao;
    no->valor = strdup(valor);
    no->filhoA = filhoA;
    no->filhoB = filhoB;

    // Se passou na verificação, o tipo pode ser copiado do filho da esquerda.
    if (filhoA) {
        no->tipo = strdup(filhoA->tipo);
    } else {
        no->tipo = strdup("");
    }
    return no;
}

NoExpressao* criar_no_expressao_simples(char operacao, const char* tipo, const char* valor) {
    NoExpressao* no = (NoExpressao*)malloc(sizeof(NoExpressao));
    no->operacao = operacao;
    no->tipo = strdup(tipo);
    no->valor = strdup(valor);
    no->filhoA = NULL;
    no->filhoB = NULL;
    return no;
}

NoExpressao* criar_no_expressao_basico(char operacao, const char* valor) {
    return criar_no_expressao_simples(operacao, "", valor);
}

ListaString* criar_lista_string() {
    ListaString* lista = (ListaString*)malloc(sizeof(ListaString));
    lista->itens = NULL;
    lista->tamanho = 0;
    lista->capacidade = 0;
    return lista;
}

void adicionar_string(ListaString* lista, const char* str) {
    if (lista->tamanho >= lista->capacidade) {
        int nova_capacidade = lista->capacidade == 0 ? 4 : lista->capacidade * 2;
        char** novos_itens = (char**)realloc(lista->itens, sizeof(char*) * nova_capacidade);
        if (!novos_itens) {
            fprintf(stderr, "Erro de alocação de memória ao adicionar string.\n");
            exit(EXIT_FAILURE);
        }
        lista->itens = novos_itens;
        lista->capacidade = nova_capacidade;
    }
    lista->itens[lista->tamanho++] = strdup(str);
}

void lista_codigo_adicionar_lista(ListaString* destino, ListaString* fonte) {
    if (!fonte || fonte->tamanho == 0) return;

    // Garante que o destino tem capacidade suficiente
    int nova_capacidade = destino->capacidade;
    while (destino->tamanho + fonte->tamanho > nova_capacidade) {
        nova_capacidade = nova_capacidade == 0 ? 4 : nova_capacidade * 2;
    }
    if (nova_capacidade > destino->capacidade) {
        char** novos_itens = (char**)realloc(destino->itens, sizeof(char*) * nova_capacidade);
        if(!novos_itens) { /* Tratar erro */ exit(1); }
        destino->itens = novos_itens;
        destino->capacidade = nova_capacidade;
    }

    // Copia os ponteiros de string da fonte para o destino
    memcpy(destino->itens + destino->tamanho, fonte->itens, fonte->tamanho * sizeof(char*));
    destino->tamanho += fonte->tamanho;

    // Esvazia a lista de origem sem liberar as strings (pois foram movidas)
    free(fonte->itens);
    fonte->itens = NULL;
    fonte->tamanho = 0;
    fonte->capacidade = 0;
}

void copiar_res_var_codigo(Codigo* destino, const Codigo* fonte) {
    if (!destino || !fonte) {
        return;
    }

    // Libera a memória antiga do destino para evitar memory leaks
    free(destino->var);
    free(destino->inicio);
    free(destino->ret);
    free(destino->valor_retorno);

    // Copia os ponteiros usando strdup para criar novas alocações
    destino->var = fonte->var ? strdup(fonte->var) : NULL;
    destino->inicio = fonte->inicio ? strdup(fonte->inicio) : NULL;
    destino->ret = fonte->ret ? strdup(fonte->ret) : NULL;
    destino->valor_retorno = fonte->valor_retorno ? strdup(fonte->valor_retorno) : NULL;
    
    // Copia o valor inteiro
    destino->contador_param = fonte->contador_param;
}

void lista_codigo_imprimir_tudo(ListaString* lista) {
    if (!lista) return;
    for (int i = 0; i < lista->tamanho; ++i) {
        printf("%s\n", lista->itens[i]);
    }
}

void liberar_lista_string(ListaString* lista) {
    if (lista) {
        for (int i = 0; i < lista->tamanho; i++) {
            free(lista->itens[i]);
        }
        free(lista->itens);
        free(lista);
    }
}

void liberar_no_expressao(NoExpressao* no) {
    if (no) {
        free(no->tipo);
        free(no->valor);
        liberar_no_expressao(no->filhoA);
        liberar_no_expressao(no->filhoB);
        free(no);
    }
}

void liberar_no_ast(NoAST* no) {
    if (no) {
        free(no->identificador);
        if (no->token) { // Libera o token se o nó for dono dele
            liberar_token(no->token);
        }
        for (int i = 0; i < no->quantidade_filhos; i++) {
            liberar_no_ast(no->filhos[i]);
        }
        free(no->filhos);
        
        free(no->sdt_dec.tipo_inicial);
        free(no->sdt_dec.tipo);
        
        free(no->res_var_codigo.var);
        free(no->res_var_codigo.inicio);
        free(no->res_var_codigo.ret);
        free(no->res_var_codigo.valor_retorno);
        
        liberar_no_expressao(no->sdt_mat.no);
        liberar_lista_string(no->sdt_mat.vetor_array_var);
        liberar_lista_string(no->codigo);
        
        free(no->proximo);
        free(no->para_proximo);
        
        free(no);
    }
}

void imprimir_erro_tipo(char operacao, NoExpressao* filhoA, NoExpressao* filhoB) {
    fprintf(stderr, "====================================================\n");
    fprintf(stderr, "Erro de tipo: Operação '%c' inválida entre os tipos '%s' e '%s'.\n", 
            operacao, filhoA ? filhoA->tipo : "nulo", filhoB ? filhoB->tipo : "nulo");
}