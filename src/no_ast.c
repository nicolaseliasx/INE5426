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

NoExpressao* criar_no_expressao_binario(char operacao, const char* valor, NoExpressao* filhoA, NoExpressao* filhoB) {
    // A verificação de tipo é feita na ação semântica
    NoExpressao* no = (NoExpressao*)malloc(sizeof(NoExpressao));
    no->operacao = operacao;
    no->valor = strdup(valor);
    no->filhoA = filhoA;
    no->filhoB = filhoB;
    if (filhoA) { // O tipo do nó binário geralmente depende da operação
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
    if (!fonte) return;
    for (int i = 0; i < fonte->tamanho; ++i) {
        adicionar_string(destino, fonte->itens[i]);
    }
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