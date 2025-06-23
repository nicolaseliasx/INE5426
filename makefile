# Makefile para o Projeto de Compilador
# Este Makefile compila todos os ficheiros .c e os liga num único executável.

# Compilador a ser usado (GNU C Compiler)
CC = gcc

# Flags do compilador:
# -Wall -Wextra: Ativa quase todos os avisos do compilador, o que é bom para apanhar erros.
# -g: Adiciona informação de depuração ao executável (essencial para usar com gdb).
# -std=c99: Especifica a versão do padrão da linguagem C.
CFLAGS = -Wall -Wextra -g -std=c99

# --- CONFIGURAÇÃO DOS DIRETÓRIOS ---
# Diretório onde os ficheiros de código-fonte (.c e .h) estão localizados
SRC_DIR = src
# Adiciona o diretório de cabeçalhos aos flags para que o compilador encontre os #includes
CPPFLAGS = -I$(SRC_DIR)

# Nome do executável final
EXEC = compilador

# Lista dos NOMES BASE dos ficheiros de código fonte (.c), sem o caminho.
# Adicionado: pilha.c
SRCS = \
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

# Gera automaticamente a lista de ficheiros de objeto (.o) a partir da lista de fontes.
# Os ficheiros .o serão criados no diretório raiz, onde o 'make' é executado.
OBJS = $(SRCS:.c=.o)

# Lista dos NOMES BASE dos ficheiros de cabeçalho (.h)
# Adicionado: pilha.h
HEADERS = \
    acoes_semanticas.h \
    analisador_lexico.h \
    analisador_sintatico.h \
    erros.h \
    gerenciador_escopo.h \
    identificadores_tokens.h \
    item_pilha_analise.h \
    maquina_estados.h \
    no_ast.h \
    pilha.h \
    resolvedor_expressao.h \
    tabela_analise.h \
    tabela_sdt_auxiliar.h \
    tabela_simbolos.h \
    tipo_enum.h \
    token.h

# Gera a lista completa de dependências de cabeçalho com o caminho
DEPS = $(addprefix $(SRC_DIR)/, $(HEADERS))

# A regra 'all' é a regra padrão. Executar 'make' ou 'make all' irá construir o projeto.
.PHONY: all
all: $(EXEC)

# Regra para ligar (link) todos os ficheiros objeto e criar o executável final.
$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJS)

# Regra genérica para compilar um ficheiro .c num ficheiro .o.
# Para criar um ficheiro .o na raiz (ex: acoes_semanticas.o),
# esta regra diz ao 'make' para usar o .c correspondente que está dentro de $(SRC_DIR).
%.o: $(SRC_DIR)/%.c $(DEPS)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

# A regra 'run' executa o programa compilado.
.PHONY: run
run: $(EXEC)
	./$(EXEC)

# A regra 'clean' remove todos os ficheiros compilados (objetos e o executável) da raiz.
.PHONY: clean
clean:
	rm -f $(OBJS) $(EXEC)
