#ifndef IDENTIFICADORES_TOKENS_H
#define IDENTIFICADORES_TOKENS_H

#include "token.h"

// Tipo de função de transição de estado
typedef const char* (*FuncaoTransicao)(char c, int eof);

// Estrutura para uma transição de estado
typedef struct {
    const char* nome_estado;
    FuncaoTransicao funcao_transicao;
} TransicaoEstado;

// Máquina de estado finito
typedef struct {
    TransicaoEstado* transicoes;
    int num_transicoes;
    const char* estado_inicial;
    const char** estados_finais;
    int num_estados_finais;
    const char** estados_mortos;
    int num_estados_mortos;
} MaquinaEstado;

// Identificador de token
typedef struct {
    const char* id_token;
    MaquinaEstado* maquina_estado;
} IdentificadorToken;

// Declaração das funções de inicialização
void inicializar_identificadores();

// Declarações dos identificadores de token
extern IdentificadorToken identTokenIdentifier;
extern IdentificadorToken integerTokenIdentifier;
extern IdentificadorToken floatTokenIdentifier;
extern IdentificadorToken mathOperatorTokenIdentifier;
extern IdentificadorToken relopTokenIdentifier;
extern IdentificadorToken oneCharTokenIdentifier;
extern IdentificadorToken stringConstCharTokenIdentifier;

#endif // IDENTIFICADORES_TOKENS_H