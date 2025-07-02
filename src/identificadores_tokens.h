#ifndef IDENTIFICADORES_TOKENS_H
#define IDENTIFICADORES_TOKENS_H

#include "maquina_estados.h"
#include "token.h"

// Identificador de token
typedef struct
{
    const char* id_token;
    MaquinaEstados* maquina_estado;
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

#endif  // IDENTIFICADORES_TOKENS_H