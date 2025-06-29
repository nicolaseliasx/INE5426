# Makefile para o Projeto de Compilador

# Compilador e Flags
CC = gcc
CFLAGS = -Wall -Wextra -g -std=c99

# --- Diretórios ---
SRC_DIR = src
OBJ_DIR = obj

# Adiciona o diretório de includes aos flags
CPPFLAGS = -I$(SRC_DIR)

# --- Arquivos ---
# Nome do executável final
EXEC = compilador

# Lista dos NOMES BASE dos ficheiros de código fonte (.c)
SRCS = \
    main.c \
    acoes_semanticas.c \
    analisador_lexico.c \
    analisador_sintatico.c \
    erros.c \
    gerenciador_escopo.c \
    identificadores_tokens.c \
    item_pilha_analise.c \
    maquina_estados.c \
    no_ast.c \
    pilha.c \
    resolvedor_expressao.c \
    tabela_analise.c \
    tabela_sdt_auxiliar.c \
    tabela_simbolos.c \
    tipo_enum.c \
    token.c

# Gera automaticamente a lista de ficheiros de objeto (.o) com o caminho para o diretório obj/
OBJS = $(addprefix $(OBJ_DIR)/, $(SRCS:.c=.o))


# --- Regras do Make ---

# A regra 'all' é a regra padrão.
.PHONY: all
all: $(EXEC)

# Regra para ligar (link) todos os ficheiros objeto e criar o executável final.
$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $^

# MUDANÇA PRINCIPAL:
# Regra para compilar um ficheiro .c de src/ para um .o em obj/
# A dependência `| $(OBJ_DIR)` é uma "dependência de ordem". 
# Ela garante que a pasta obj/ seja criada ANTES de tentar compilar o .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

# Regra para criar o diretório de objetos, se ele não existir.
# O @ na frente do comando esconde a sua execução no terminal (deixa a saída mais limpa).
$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

# A regra 'run' executa o programa compilado.
.PHONY: run
run: $(EXEC)
	./$(EXEC) $(ARGS) # ARGS permite passar argumentos da linha de comando, ex: make run ARGS="test.txt"

# A regra 'clean' remove o diretório obj e o executável.
.PHONY: clean
clean:
	@echo "Limpando ficheiros compilados..."
	@rm -rf $(OBJ_DIR) $(EXEC)
	@echo "Limpeza concluída."