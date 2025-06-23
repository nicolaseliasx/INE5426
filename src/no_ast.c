#include "no_ast.h"
#include "erros.h"

// Implementação das funções

NoAST* criar_no_ast(const char* identificador, int* contador_rotulos) {
    NoAST* no = (NoAST*)malloc(sizeof(NoAST));
    no->identificador = strdup(identificador);
    no->token = NULL;
    
    no->filhos = NULL;
    no->quantidade_filhos = 0;
    no->capacidade_filhos = 0;
    
    no->codigo = criar_lista_string();
    no->proximo = NULL;
    no->proximo_para_for = NULL;
    
    no->decSDT.tipo_inicial = NULL;
    no->decSDT.tipo = NULL;
    
    no->codigo_res_var.variavel = NULL;
    no->codigo_res_var.inicio = NULL;
    no->codigo_res_var.retorno = NULL;
    no->codigo_res_var.valor_retorno = strdup("0");
    no->codigo_res_var.contador_parametros = 0;
    
    no->mathSDT.contador_arrays = 0;
    no->mathSDT.variaveis_array = criar_lista_string();
    no->mathSDT.operacao = '\0';
    no->mathSDT.no = NULL;
    
    no->contador_rotulos = contador_rotulos;
    
    return no;
}

void adicionar_filho(NoAST* self, NoAST* filho) {
    if (self->quantidade_filhos == self->capacidade_filhos) {
        int nova_capacidade = self->capacidade_filhos == 0 ? 1 : self->capacidade_filhos * 2;
        NoAST** novos_filhos = (NoAST**)realloc(self->filhos, sizeof(NoAST*) * nova_capacidade);
        if (!novos_filhos) {
            fprintf(stderr, "Erro de alocação de memória\n");
            exit(EXIT_FAILURE);
        }
        self->filhos = novos_filhos;
        self->capacidade_filhos = nova_capacidade;
    }
    self->filhos[self->quantidade_filhos++] = filho;
}

void definir_filhos(NoAST* self, NoAST** novos_filhos, int quantidade) {
    // Liberar filhos antigos se existirem
    if (self->filhos) {
        free(self->filhos);
    }
    
    self->filhos = novos_filhos;
    self->quantidade_filhos = quantidade;
    self->capacidade_filhos = quantidade;
}

void definir_token(NoAST* self, Token* token) {
    self->token = token;
}

char* novo_rotulo(NoAST* self) {
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
    no->tipo = filhoA->tipo; // Compartilha o mesmo tipo
    
    return no;
}

NoExpressao* criar_no_expressao_binario(char operacao, const char* valor, NoExpressao* filhoA, NoExpressao* filhoB) {
    if (strcmp(filhoA->tipo, filhoB->tipo) == 0) {
        NoExpressao* no = (NoExpressao*)malloc(sizeof(NoExpressao));
        no->operacao = operacao;
        no->valor = strdup(valor);
        no->filhoA = filhoA;
        no->filhoB = filhoB;
        no->tipo = strdup(filhoA->tipo); // Duplica o tipo
        
        return no;
    } else {
        imprimir_erro_tipo(operacao, filhoA, filhoB);
        erro_semantico("Erro de tipo na construção da árvore de expressão");
        return NULL;
    }
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
    NoExpressao* no = (NoExpressao*)malloc(sizeof(NoExpressao));
    no->operacao = operacao;
    no->valor = strdup(valor);
    no->tipo = NULL;
    no->filhoA = NULL;
    no->filhoB = NULL;
    return no;
}

ListaString* criar_lista_string() {
    ListaString* lista = (ListaString*)malloc(sizeof(ListaString));
    lista->itens = NULL;
    lista->tamanho = 0;
    lista->capacidade = 0;
    return lista;
}

void adicionar_string(ListaString* lista, const char* str) {
    if (lista->tamanho == lista->capacidade) {
        int nova_capacidade = lista->capacidade == 0 ? 4 : lista->capacidade * 2;
        char** novos_itens = (char**)realloc(lista->itens, sizeof(char*) * nova_capacidade);
        if (!novos_itens) {
            fprintf(stderr, "Erro de alocação de memória\n");
            exit(EXIT_FAILURE);
        }
        lista->itens = novos_itens;
        lista->capacidade = nova_capacidade;
    }
    lista->itens[lista->tamanho++] = strdup(str);
}

// Funções de liberação de memória
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
        
        // Liberar filhos
        for (int i = 0; i < no->quantidade_filhos; i++) {
            liberar_no_ast(no->filhos[i]);
        }
        free(no->filhos);
        
        // Liberar SDTs
        free(no->decSDT.tipo_inicial);
        free(no->decSDT.tipo);
        
        free(no->codigo_res_var.variavel);
        free(no->codigo_res_var.inicio);
        free(no->codigo_res_var.retorno);
        free(no->codigo_res_var.valor_retorno);
        
        liberar_no_expressao(no->mathSDT.no);
        liberar_lista_string(no->mathSDT.variaveis_array);
        liberar_lista_string(no->codigo);
        
        free(no->proximo);
        free(no->proximo_para_for);
        
        free(no);
    }
}

void imprimir_erro_tipo(char operacao, NoExpressao* filhoA, NoExpressao* filhoB) {
    fprintf(stderr, "====================================================\n");
    fprintf(stderr, "Erro: operação '%c' entre tipos incompatíveis\n", operacao);
    fprintf(stderr, "Tipo do primeiro operando: %s\n", filhoA->tipo);
    fprintf(stderr, "Tipo do segundo operando: %s\n", filhoB->tipo);
}