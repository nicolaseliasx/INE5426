#include "erros.h"

// Função para criar erro léxico
Erro criar_erro_lexico(const char* mensagem) {
    Erro erro;
    erro.tipo = ERRO_LEXICO;
    snprintf(erro.mensagem, sizeof(erro.mensagem), "%s", mensagem);
    return erro;
}

// Função para criar erro sintático
Erro criar_erro_sintatico(const char* mensagem) {
    Erro erro;
    erro.tipo = ERRO_SINTATICO;
    snprintf(erro.mensagem, sizeof(erro.mensagem), "%s", mensagem);
    return erro;
}

// Função para criar erro semântico
Erro criar_erro_semantico(const char* mensagem) {
    Erro erro;
    erro.tipo = ERRO_SEMANTICO;
    snprintf(erro.mensagem, sizeof(erro.mensagem), "%s", mensagem);
    return erro;
}

// Funções para tratamento de erros
void tratar_erro(const Erro* erro) {
    fprintf(stderr, "%s\n", erro->mensagem);
    exit(EXIT_FAILURE);
}

void imprimir_erro(const Erro* erro) {
    const char* tipo_str = "";
    switch(erro->tipo) {
        case ERRO_LEXICO: tipo_str = "Léxico"; break;
        case ERRO_SINTATICO: tipo_str = "Sintático"; break;
        case ERRO_SEMANTICO: tipo_str = "Semântico"; break;
    }
    fprintf(stderr, "[ERRO %s] %s\n", tipo_str, erro->mensagem);
}