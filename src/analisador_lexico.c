#include "analisador_lexico.h"

// Função auxiliar para verificar se um caractere é espaço em branco
static int eh_espaco_branco(char c) {
    return c == ' ' || c == '\t' || c == '\r';
}

// Função para encontrar o maior token dentre os possíveis
static PossivelToken* encontrar_maior_token(AnalisadorLexico* analisador) {
    PossivelToken* resposta = NULL;
    unsigned int maior_tamanho = 0;
    
    for (int i = 0; i < analisador->num_possiveis_tokens; i++) {
        unsigned int tamanho_token = strlen(analisador->possiveis_tokens[i]->token->lexema);
        if (i == 0 || tamanho_token > maior_tamanho) {
            maior_tamanho = tamanho_token;
            resposta = analisador->possiveis_tokens[i];
        }
    }
    return resposta;
}

// Função para obter o ID do token baseado no lexema
static char* obter_id_token(AnalisadorLexico* analisador, const char* id, const char* lexema) {
    // Verificar se é identificador e palavra-chave
    if (strcmp(id, "IDENT") == 0) {
        for (int i = 0; i < analisador->num_palavras_chave; i++) {
            if (strcmp(lexema, analisador->palavras_chave[i]) == 0) {
                // Converter para maiúsculas
                char* novo_id = strdup(analisador->palavras_chave[i]);
                for (char* p = novo_id; *p; p++) *p = toupper(*p);
                return novo_id;
            }
        }
    }
    
    // Verificar se é operador matemático ou caractere único
    if (strcmp(id, "MATHOP") != 0 && strcmp(id, "ONECHAR") != 0) {
        return strdup(id);
    }
    
    // Procurar no mapeamento
    for (int i = 0; i < analisador->num_mapeamentos; i++) {
        if (strcmp(lexema, analisador->mapeamento_tokens[i].lexema) == 0) {
            return strdup(analisador->mapeamento_tokens[i].id);
        }
    }
    
    return strdup(id); // Fallback
}

AnalisadorLexico* criar_analisador_lexico(FILE* arquivo, char** palavras_chave, int num_palavras) {
    AnalisadorLexico* analisador = malloc(sizeof(AnalisadorLexico));
    analisador->arquivo = arquivo;
    
    // Copiar palavras-chave
    analisador->num_palavras_chave = num_palavras;
    analisador->palavras_chave = malloc(num_palavras * sizeof(char*));
    for (int i = 0; i < num_palavras; i++) {
        analisador->palavras_chave[i] = strdup(palavras_chave[i]);
    }
    
    // Inicializar identificadores de token
    analisador->num_identificadores = 7;
    analisador->identificadores_token = malloc(analisador->num_identificadores * sizeof(IdentificadorToken*));
    analisador->identificadores_token[0] = criar_identificador_ident();
    analisador->identificadores_token[1] = criar_identificador_inteiro();
    analisador->identificadores_token[2] = criar_identificador_float();
    analisador->identificadores_token[3] = criar_identificador_operador_matematico();
    analisador->identificadores_token[4] = criar_identificador_relop();
    analisador->identificadores_token[5] = criar_identificador_um_caractere();
    analisador->identificadores_token[6] = criar_identificador_string_const();
    
    // Inicializar mapeamento de tokens
    analisador->num_mapeamentos = 14;
    analisador->mapeamento_tokens = malloc(analisador->num_mapeamentos * sizeof(*analisador->mapeamento_tokens));
    
    // Operadores e delimitadores
    char* mapeamentos[][2] = {
        {"+", "PLUS"},
        {"-", "MINUS"},
        {"*", "MULTIPLICATION"},
        {"/", "DIVISION"},
        {"%", "MODULUS"},
        {"=", "ASSIGN"},
        {"(", "OPEN_PARENTHESIS"},
        {")", "CLOSE_PARENTHESIS"},
        {"{", "OPEN_BRACE"},
        {"}", "CLOSE_BRACE"},
        {"[", "OPEN_BRACKET"},
        {"]", "CLOSE_BRACKET"},
        {";", "SEMICOLON"},
        {",", "COMMA"}
    };
    
    for (int i = 0; i < analisador->num_mapeamentos; i++) {
        analisador->mapeamento_tokens[i].lexema = strdup(mapeamentos[i][0]);
        analisador->mapeamento_tokens[i].id = strdup(mapeamentos[i][1]);
    }
    
    // Inicializar contadores
    analisador->linha = 1;
    analisador->coluna = 0;
    analisador->linhas_durante_token = 0;
    analisador->lendo_token = 0;
    
    // Inicializar listas vazias
    analisador->possiveis_tokens = NULL;
    analisador->num_possiveis_tokens = 0;
    analisador->tokens = NULL;
    analisador->num_tokens = 0;
    
    return analisador;
}

Token* proximo_token(AnalisadorLexico* analisador) {
    if (analisador->arquivo) {
        while (!feof(analisador->arquivo)) {
            analisador->caractere_atual = fgetc(analisador->arquivo);
            int eof = feof(analisador->arquivo);
            
            if (eof && !analisador->lendo_token) break;
            
            analisador->coluna++;
            if (analisador->caractere_atual == '\n') {
                analisador->linha++;
                analisador->coluna = 0;
                if (analisador->lendo_token) analisador->linhas_durante_token++;
            }

            // Ignorar espaços em branco fora de tokens
            if (!eof && eh_espaco_branco(analisador->caractere_atual) && !analisador->lendo_token) {
                continue;
            }

            int em_execucao = 0;
            for (int i = 0; i < analisador->num_identificadores; i++) {
                IdentificadorToken* id = analisador->identificadores_token[i];
                
                if (identificador_token_sucedeu(id)) continue;
                
                int resultado = analisar_caractere(id, analisador->caractere_atual, eof);
                
                if (resultado == MAQ_EXECUTANDO) {
                    em_execucao++;
                    continue;
                }
                
                if (resultado == MAQ_SUCESSO) {
                    char* lexema = obter_lexema(id);
                    char* token_id = obter_id_token(analisador, obter_identificador(id), lexema);
                    
                    Token* token = criar_token(lexema, token_id, obter_linha_inicio(id), obter_coluna_inicio(id));
                    
                    // Adicionar aos tokens possíveis
                    analisador->num_possiveis_tokens++;
                    analisador->possiveis_tokens = realloc(analisador->possiveis_tokens, 
                        analisador->num_possiveis_tokens * sizeof(PossivelToken*));
                    
                    PossivelToken* pt = malloc(sizeof(PossivelToken));
                    pt->token = token;
                    pt->inicio = obter_posicao_inicio(id);
                    analisador->possiveis_tokens[analisador->num_possiveis_tokens - 1] = pt;
                }
            }

            analisador->lendo_token = em_execucao > 0;

            if (em_execucao > 0) continue;

            // Nenhum token válido encontrado
            if (analisador->num_possiveis_tokens == 0) {
                char erro_msg[100];
                snprintf(erro_msg, sizeof(erro_msg), "Erro começando em %d:%d", analisador->linha, analisador->coluna);
                erro_lexico(erro_msg);
            }

            // Selecionar o maior token
            PossivelToken* token_escolhido = encontrar_maior_token(analisador);
            Token* token = token_escolhido->token;
            
            // Adicionar à lista de tokens
            analisador->num_tokens++;
            analisador->tokens = realloc(analisador->tokens, 
                analisador->num_tokens * sizeof(Token*));
            analisador->tokens[analisador->num_tokens - 1] = token;

            // Atualizar posição
            analisador->coluna = token->coluna + strlen(token->lexema) - 1;
            if (analisador->linhas_durante_token) {
                analisador->linha -= analisador->linhas_durante_token;
                analisador->linhas_durante_token = 0;
            }

            // Reposicionar cursor
            int proxima_posicao = token_escolhido->inicio + strlen(token->lexema);
            fseek(analisador->arquivo, proxima_posicao, SEEK_SET);

            // Limpar tokens possíveis e resetar identificadores
            for (int i = 0; i < analisador->num_possiveis_tokens; i++) {
                free(analisador->possiveis_tokens[i]);
            }
            free(analisador->possiveis_tokens);
            analisador->possiveis_tokens = NULL;
            analisador->num_possiveis_tokens = 0;
            
            for (int i = 0; i < analisador->num_identificadores; i++) {
                resetar_identificador(analisador->identificadores_token[i]);
            }

            return token;
        }
    }
    return NULL;
}

void destruir_analisador_lexico(AnalisadorLexico* analisador) {
    if (!analisador) return;
    
    // Liberar palavras-chave
    for (int i = 0; i < analisador->num_palavras_chave; i++) {
        free(analisador->palavras_chave[i]);
    }
    free(analisador->palavras_chave);
    
    // Liberar identificadores de token
    for (int i = 0; i < analisador->num_identificadores; i++) {
        destruir_identificador(analisador->identificadores_token[i]);
    }
    free(analisador->identificadores_token);
    
    // Liberar mapeamento
    for (int i = 0; i < analisador->num_mapeamentos; i++) {
        free(analisador->mapeamento_tokens[i].lexema);
        free(analisador->mapeamento_tokens[i].id);
    }
    free(analisador->mapeamento_tokens);
    
    // Liberar tokens
    for (int i = 0; i < analisador->num_tokens; i++) {
        destruir_token(analisador->tokens[i]);
    }
    free(analisador->tokens);
    
    // Liberar tokens possíveis
    for (int i = 0; i < analisador->num_possiveis_tokens; i++) {
        free(analisador->possiveis_tokens[i]);
    }
    free(analisador->possiveis_tokens);
    
    free(analisador);
}