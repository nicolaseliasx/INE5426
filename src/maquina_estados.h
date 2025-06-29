#ifndef MAQUINA_ESTADOS_H
#define MAQUINA_ESTADOS_H

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Estados da máquina
typedef enum {
    MAQ_OCIOSA,
    MAQ_EXECUTANDO,
    MAQ_SUCESSO,
    MAQ_ERRO
} EstadoMaquina;

typedef const char* (*FuncaoTransicao)(char c, int is_eof);

typedef struct {
    const char* estado;
    FuncaoTransicao funcao_transicao;
} TransicaoEstado;


typedef struct {
    // --- Configuração da Máquina (definida na inicialização) ---
    TransicaoEstado* transicoes;
    int num_transicoes;
    const char* estado_inicial;
    const char** estados_finais;
    int num_estados_finais;
    const char** estados_mortos;
    int num_estados_mortos;
    const char** estados_retrocesso;
    int num_estados_retrocesso;

    // --- Estado Interno da Máquina (controlado em tempo de execução) ---
    const char* estado_atual;
    char* lexema;
    int tamanho_lexema;
    EstadoMaquina status;
    bool retroceder_cursor;
} MaquinaEstados;


// Funções públicas (usando o nome padronizado MaquinaEstado)
MaquinaEstados* criar_maquina_estados(TransicaoEstado* transicoes, int num_trans,
                                      const char* estado_inicial, 
                                      const char** estados_finais,
                                      const char** estados_retrocesso);

void reiniciar_maquina(MaquinaEstados* maq);
EstadoMaquina transicionar_maquina(MaquinaEstados* maq, char c, int is_eof);
int deve_retroceder_cursor(MaquinaEstados* maq);
void liberar_maquina_estados(MaquinaEstados* maq);
EstadoMaquina obter_status_maquina(MaquinaEstados* maq);
const char* obter_lexema_maquina(MaquinaEstados* maq);

#endif // MAQUINA_ESTADOS_H