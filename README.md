# Compilador para a Linguagem ConvCC-2025-1

Este repositório contém o código-fonte de um compilador completo desenvolvido para a disciplina de **INE5426 - Construção de Compiladores**. O projeto implementa todas as fases de um compilador moderno, desde a análise léxica até a geração de código intermediário de três endereços.

---

## Grupo

- Nicolas
- Patricia
- Gustavo

## Professor

Alvaro Junio Pereira Franco

---

## Sobre a Linguagem

O compilador foi projetado para a linguagem **ConvCC-2025-1**.

---

## Gramática da Linguagem

A especificação formal da gramática base, uma análise detalhada das melhorias e extensões implementadas, e a gramática final LL(1) fatorada estão documentadas no arquivo: GRAMATICA.txt

---

## Estrutura do Projeto

O projeto é modularizado para separar as diferentes responsabilidades do compilador. Abaixo está uma visão geral dos principais arquivos e seus papéis:

| Módulo                   | Descrição                                                                                              |
| ------------------------ | ------------------------------------------------------------------------------------------------------ | ------------------------------------------------------------------------------------------------------------------ |
| `main.c`                 | Ponto de entrada do programa. Orquestra a inicialização dos analisadores e o processamento do arquivo. |
| `acoes_semanticas.(h     | c)`                                                                                                    | Contém rotinas semânticas acionadas pelo analisador sintático (validação de tipos, escopo, geração de código).     |
| `analisador_lexico.(h    | c)`                                                                                                    | Lê o código-fonte caractere a caractere e agrupa tokens (identificadores, palavras-chave, operadores).             |
| `analisador_sintatico.(h | c)`                                                                                                    | Implementa um parser preditivo LL(1) usando pilha e tabela de análise. Constrói a Árvore Sintática Abstrata (AST). |
| `tabela_analise.(h       | c)`                                                                                                    | Define a tabela de parsing LL(1) (mapeia pares [Não-Terminal, Terminal] para produções da gramática).              |
| `tabela_sdt_auxiliar.(h  | c)`                                                                                                    | Define regras de produção e ações semânticas (base da Tradução Dirigida por Sintaxe - SDT).                        |
| `gerenciador_escopo.(h   | c)`                                                                                                    | Gerencia hierarquia de escopos (global, funções, blocos) e tabelas de símbolos associadas.                         |
| `tabela_simbolos.(h      | c)`                                                                                                    | Implementa tabela de símbolos (usando tabela hash) para armazenar informações de identificadores.                  |
| `no_ast.(h               | c)`                                                                                                    | Define a estrutura da Árvore Sintática Abstrata (AST).                                                             |
| `erros.(h                | c)`                                                                                                    | Interface unificada para relatar erros léxicos, sintáticos e semânticos.                                           |
| `pilha.(h                | c)`                                                                                                    | Implementação de estrutura de dados de pilha (utilizada pelo analisador sintático).                                |

---

## Requisitos

Para compilar e executar este projeto, você precisará de:

- `make`
- `gcc` (ou compilador C compatível como `clang`)

---

## Como Executar

### 1. Limpar builds anteriores (opcional, mas recomendado):

```bash
make clean
```

```bash
make
```

```bash
./compilador ~/ufsc/compiladores/main/src/tests/teste_2_break.txt
```

Nota: Substitua o caminho pelo seu arquivo de teste específico.

## Saída Esperada

Após a execução, o compilador imprimirá no console:

- Visualização das Árvores de Expressão

- Mensagem de Sucesso da Análise (confirmação de análises léxica/sintática/semântica)

- Tabelas de Símbolos geradas para cada escopo

- Código Intermediário de três endereços gerado

- Erros: Quaisquer erros (léxicos, sintáticos ou semânticos) encontrados durante a compilação
