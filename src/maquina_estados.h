// Arquivo: maquina_estados.h (VERSÃO REATORADA E CORRETA)

#ifndef MAQUINA_ESTADOS_H
#define MAQUINA_ESTADOS_H

#include <stdbool.h>

// Enum para o status da máquina, mais detalhado
typedef enum
{
    MAQ_INICIAL,     // Pronta para começar, lexema vazio.
    MAQ_EXECUTANDO,  // Processando caracteres, ainda não há um match.
    MAQ_SUCESSO,     // Atingiu um estado final.
    MAQ_ERRO         // Atingiu um estado morto.
} EstadoMaquina;

// Ponteiro para uma função que define a lógica de uma transição de estado
typedef const char* (*FuncaoTransicao)(char c, int eof);

// Struct que mapeia um nome de estado para sua função de transição
typedef struct
{
    const char* estado;
    FuncaoTransicao funcao_transicao;
} TransicaoEstado;

// A struct principal da Máquina de Estados
typedef struct
{
    // --- PARTE DE CONFIGURAÇÃO (definida estaticamente em identificadores_tokens.c) ---
    const TransicaoEstado* transicoes;
    int num_transicoes;
    const char* estado_inicial;
    const char** estados_finais;
    int num_estados_finais;
    const char** estados_mortos;
    int num_estados_mortos;
    const char** estados_retrocesso;  // Estados que indicam que o último char não foi consumido
    int num_estados_retrocesso;

    // --- PARTE DE ESTADO INTERNO (controlado em tempo de execução) ---
    const char* estado_atual;
    char* lexema;
    int tamanho_lexema;
    EstadoMaquina status;
} MaquinaEstados;

// --- Funções Públicas ---

// Prepara a máquina para uma nova leitura de token
void reiniciar_maquina(MaquinaEstados* maq);

// Processa um caractere e atualiza o estado interno da máquina
void transicionar_maquina(MaquinaEstados* maq, char c, int eof);

// Verifica se o estado final atual requer retrocesso do cursor de leitura
bool deve_retroceder_cursor(MaquinaEstados* maq);

// Funções para obter o estado e o lexema
EstadoMaquina obter_status_maquina(MaquinaEstados* maq);
char* obter_lexema_maquina(MaquinaEstados* maq);

#endif