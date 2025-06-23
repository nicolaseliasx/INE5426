#ifndef MAQUINA_ESTADOS_H
#define MAQUINA_ESTADOS_H

#include <stdlib.h>
#include <string.h>

// Estados da máquina
typedef enum {
    MAQ_OCIOSA,
    MAQ_EXECUTANDO,
    MAQ_SUCESSO,
    MAQ_ERRO
} EstadoMaquina;

// Estrutura para entrada de transição
typedef struct {
    char* estado;
    char* (*funcao_transicao)(char c, int is_eof);
} EntradaTransicao;

// Estrutura principal da máquina de estados
typedef struct {
    EntradaTransicao* transicoes;
    int num_transicoes;
    
    char* estado_inicial;
    char** estados_finais;
    char** estados_retrocesso;
    
    char* estado_atual;
    char* lexema;
    EstadoMaquina status;
} MaquinaEstados;

// Funções públicas
MaquinaEstados* criar_maquina_estados(EntradaTransicao* transicoes, int num_trans,
                                      const char* estado_inicial, 
                                      char** estados_finais,
                                      char** estados_retrocesso);

void reiniciar_maquina(MaquinaEstados* maq);
EstadoMaquina transicionar_maquina(MaquinaEstados* maq, char c, int is_eof);
int deve_retroceder_cursor(MaquinaEstados* maq);
void liberar_maquina_estados(MaquinaEstados* maq);

// Funções auxiliares
EstadoMaquina obter_status_maquina(MaquinaEstados* maq);
const char* obter_lexema_maquina(MaquinaEstados* maq);

#endif