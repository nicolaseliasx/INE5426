CC = gcc
CFLAGS = -Wall -Wextra -g -std=c99

SRC_DIR = src
OBJ_DIR = obj

CPPFLAGS = -I$(SRC_DIR)

EXEC = compilador

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

OBJS = $(addprefix $(OBJ_DIR)/, $(SRCS:.c=.o))

.PHONY: all
all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

.PHONY: run
run: $(EXEC)
	./$(EXEC) $(ARGS)

.PHONY: clean
clean:
	@echo "Limpando..."
	@rm -rf $(OBJ_DIR) $(EXEC)
	@echo "Limpeza concluída."