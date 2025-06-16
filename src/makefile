# Variáveis
CARGO = cargo
RUSTFLAGS = -C opt-level=3
TARGET = compiler
BUILD_MODE = release
TEST_FILE = tests/teste_completo.txt

# Regra padrão para construir o executável em modo release
all: release

# Compilação em modo release
release: BUILD_MODE = release
release: RUSTFLAGS += -C opt-level=3
release: build

# Compilação em modo debug
debug: BUILD_MODE = debug
debug: RUSTFLAGS += -g
debug: build

# Comando de build genérico
build:
	@RUSTFLAGS="$(RUSTFLAGS)" $(CARGO) build --$(BUILD_MODE)
	@cp target/$(BUILD_MODE)/$(TARGET) ./$(TARGET)

# Executar o compilador com o primeiro teste
run: release
	./$(TARGET) $(TEST_FILE)

# Executar em modo debug
run-debug: debug
	./$(TARGET) $(TEST_FILE)

# Executar testes unitários
test:
	$(CARGO) test

# Limpeza
clean:
	$(CARGO) clean
	rm -f $(TARGET)

# Formatar código
fmt:
	$(CARGO) fmt

# Verificar lint
clippy:
	$(CARGO) clippy

.PHONY: all release debug build run run-debug test clean fmt clippy