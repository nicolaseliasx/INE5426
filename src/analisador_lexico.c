#define _DEFAULT_SOURCE
#include "analisador_lexico.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "erros.h"
#include "identificadores_tokens.h"
#include "maquina_estados.h"

static const MapeamentoInicial dados_mapa_inicial[] = {
    {"+", "PLUS"},      {"-", "MINUS"},       {"*", "MULTIPLICATION"},   {"/", "DIVISION"},
    {"%", "MODULUS"},   {"=", "ASSIGN"},      {"(", "OPEN_PARENTHESIS"}, {")", "CLOSE_PARENTHESIS"},
    {"{", "ONECHAR"},   {"}", "CLOSE_BRACE"}, {"[", "OPEN_BRACKET"},     {"]", "CLOSE_BRACKET"},
    {";", "SEMICOLON"}, {",", "COMMA"}};

static const int num_mapeamentos_inicial = sizeof(dados_mapa_inicial) / sizeof(MapeamentoInicial);

static int comparar_strings(const void* a, const void* b)
{
    // A função recebe ponteiros genéricos (void*).
    // Nós os convertemos de volta para o nosso tipo de dado (char**),
    // e então pegamos o valor (char*) para usar com strcmp.
    return strcmp(*(const char**)a, *(const char**)b);
}

static char* obter_id_token_final(AnalisadorLexico* analisador, const char* id, const char* lexema)
{
    // 1. Checa por palavras-chave de forma otimizada
    if (strcmp(id, "IDENT") == 0)
    {
        // Usa busca binária (O(log N)) em vez de busca linear (O(N)).
        // Isso é muito mais rápido para listas maiores.
        char** resultado = bsearch(&lexema, analisador->palavras_chave,
                                   analisador->num_palavras_chave, sizeof(char*), comparar_strings);

        if (resultado != NULL)
        {
            // Se bsearch encontrou, 'resultado' não é nulo. É uma palavra-chave.
            char* kw_id = strdup(lexema);
            for (char* p = kw_id; *p; p++)
                *p = toupper(*p);  // Converte para maiúsculas (ex: "if" -> "IF")
            return kw_id;
        }
    }

    // 2. Checa por operadores e símbolos (esta parte pode ser mantida como está)
    for (int i = 0; i < analisador->num_mapeamentos; i++)
    {
        if (strcmp(lexema, analisador->mapeamento_tokens[i].lexema) == 0)
        {
            return strdup(analisador->mapeamento_tokens[i].id);
        }
    }

    // 3. Se não for nenhum dos anteriores, retorna o ID genérico da máquina de estados (ex:
    // "IDENT")
    return strdup(id);
}

// Cria o analisador léxico
AnalisadorLexico* criar_analisador_lexico(FILE* arquivo, char** palavras_chave, int num_palavras)
{
    AnalisadorLexico* analisador = malloc(sizeof(AnalisadorLexico));
    analisador->arquivo = arquivo;
    analisador->linha = 1;
    analisador->coluna = 0;

    // Copia palavras-chave
    analisador->num_palavras_chave = num_palavras;
    analisador->palavras_chave = malloc(num_palavras * sizeof(char*));
    for (int i = 0; i < num_palavras; i++)
    {
        analisador->palavras_chave[i] = strdup(palavras_chave[i]);
    }

    // Aponta para as máquinas de estado globais
    analisador->num_identificadores = 7;
    analisador->identificadores_token =
        malloc(analisador->num_identificadores * sizeof(IdentificadorToken*));
    analisador->identificadores_token[0] = &identTokenIdentifier;
    analisador->identificadores_token[1] = &integerTokenIdentifier;
    analisador->identificadores_token[2] = &floatTokenIdentifier;
    analisador->identificadores_token[3] = &mathOperatorTokenIdentifier;
    analisador->identificadores_token[4] = &relopTokenIdentifier;
    analisador->identificadores_token[5] = &oneCharTokenIdentifier;
    analisador->identificadores_token[6] = &stringConstCharTokenIdentifier;

    analisador->num_mapeamentos = num_mapeamentos_inicial;
    analisador->mapeamento_tokens =
        malloc(analisador->num_mapeamentos * sizeof(*analisador->mapeamento_tokens));
    for (int i = 0; i < analisador->num_mapeamentos; i++)
    {
        analisador->mapeamento_tokens[i].lexema = strdup(dados_mapa_inicial[i].lexema);
        analisador->mapeamento_tokens[i].id = strdup(dados_mapa_inicial[i].id);
    }

    return analisador;
}

// Libera a memória do analisador
void destruir_analisador_lexico(AnalisadorLexico* analisador)
{
    if (!analisador) return;

    for (int i = 0; i < analisador->num_palavras_chave; i++)
    {
        free(analisador->palavras_chave[i]);
    }
    free(analisador->palavras_chave);

    for (int i = 0; i < analisador->num_mapeamentos; i++)
    {
        free(analisador->mapeamento_tokens[i].lexema);
        free(analisador->mapeamento_tokens[i].id);
    }
    free(analisador->mapeamento_tokens);
    free(analisador->identificadores_token);
    free(analisador);
}

// A função principal que lê o arquivo e retorna o próximo token
Token* proximo_token(AnalisadorLexico* analisador)
{
    if (!analisador || !analisador->arquivo || feof(analisador->arquivo))
    {
        return NULL;
    }

    // 1. Pular espaços em branco
    int c = ' ';
    while (eh_espaco_branco(c) && !feof(analisador->arquivo))
    {
        c = fgetc(analisador->arquivo);
        if (c == '\n')
        {
            analisador->linha++;
            analisador->coluna = 0;
        }
        else
        {
            analisador->coluna++;
        }
    }
    if (feof(analisador->arquivo)) return NULL;
    ungetc(c, analisador->arquivo);
    analisador->coluna--;

    // 2. Marcar posição inicial e reiniciar as máquinas
    int linha_inicio = analisador->linha;
    int coluna_inicio = analisador->coluna + 1;
    long posicao_inicio = ftell(analisador->arquivo);

    for (int i = 0; i < analisador->num_identificadores; i++)
    {
        reiniciar_maquina(analisador->identificadores_token[i]->maquina_estado);
    }

    char* lexema_maior_token = NULL;
    IdentificadorToken* id_maior_token = NULL;
    long posicao_final = posicao_inicio;
    size_t maior_tamanho = 0;

    while (1)
    {
        long pos_leitura_atual = ftell(analisador->arquivo);
        c = fgetc(analisador->arquivo);
        bool eof = (c == EOF);
        int maquinas_ativas = 0;

        for (int i = 0; i < analisador->num_identificadores; i++)
        {
            IdentificadorToken* id_token = analisador->identificadores_token[i];
            if (obter_status_maquina(id_token->maquina_estado) == MAQ_ERRO) continue;

            transicionar_maquina(id_token->maquina_estado, c, eof);

            if (obter_status_maquina(id_token->maquina_estado) != MAQ_ERRO)
            {
                maquinas_ativas++;
                if (obter_status_maquina(id_token->maquina_estado) == MAQ_SUCESSO)
                {
                    // LÓGICA DO LONGEST MATCH AQUI
                    char* lexema_atual = obter_lexema_maquina(id_token->maquina_estado);
                    size_t tamanho_atual = strlen(lexema_atual);

                    if (tamanho_atual > maior_tamanho)
                    {
                        maior_tamanho = tamanho_atual;
                        free(lexema_maior_token);
                        lexema_maior_token = strdup(lexema_atual);
                        id_maior_token = id_token;
                        posicao_final = pos_leitura_atual;
                    }
                }
            }
        }
        if (maquinas_ativas == 0 || eof) break;
    }

    // 4. Criar o token ou reportar erro
    if (id_maior_token)
    {
        fseek(analisador->arquivo, posicao_final, SEEK_SET);
        if (!deve_retroceder_cursor(id_maior_token->maquina_estado))
        {
            fgetc(analisador->arquivo);
        }
        analisador->coluna = coluna_inicio + strlen(lexema_maior_token) - 1;

        char* id_final =
            obter_id_token_final(analisador, id_maior_token->id_token, lexema_maior_token);
        Token* token_final = criar_token(lexema_maior_token, id_final, linha_inicio, coluna_inicio);

        free(lexema_maior_token);
        free(id_final);
        return token_final;
    }
    else
    {
        c = fgetc(analisador->arquivo);
        char erro_msg[100];
        snprintf(erro_msg, sizeof(erro_msg), "Caractere inesperado '%c' na linha %d:%d", c,
                 linha_inicio, coluna_inicio);
        LANCAR_ERRO_LEXICO(erro_msg);
        return NULL;
    }
}