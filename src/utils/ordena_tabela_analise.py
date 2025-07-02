# ORDENA A TABELA DE ANALISE LEXICOGRAFICAMENTE

import re
import sys

def ordenar_tabela_analise(entrada, saida=None):
    # Expressão regular para extrair as entradas da tabela
    pattern = re.compile(r'\s*{"([^"]+)",\s*"([^"]+)",\s*"([^"]+)"},\s*')
    
    # Listas para armazenar as partes do arquivo
    parte1 = []   # Antes da tabela
    entradas = [] # Entradas da tabela (como tuplas)
    parte3 = []   # Depois da tabela
    
    estado = 0    # 0: antes da tabela, 1: dentro da tabela, 2: depois da tabela
    
    with open(entrada, 'r') as f:
        for linha in f:
            if estado == 0:
                # Coletar até encontrar o início da tabela
                parte1.append(linha)
                if "const EntradaTabelaAnalise tabela_analise[] = {" in linha:
                    estado = 1
            elif estado == 1:
                # Verificar se é o fim da tabela
                if "};" in linha:
                    parte3.append(linha)
                    estado = 2
                else:
                    # Tentar extrair entrada da tabela
                    match = pattern.match(linha)
                    if match:
                        nao_terminal, terminal, regra = match.groups()
                        entradas.append((nao_terminal.strip(), terminal.strip(), regra.strip()))
            elif estado == 2:
                # Coletar o restante após a tabela
                parte3.append(linha)
    
    # Ordenar as entradas lexicograficamente
    entradas_ordenadas = sorted(entradas, key=lambda x: (x[0], x[1]))
    
    # Formatar as entradas ordenadas
    linhas_ordenadas = []
    for nao_terminal, terminal, regra in entradas_ordenadas:
        # Manter o formato original com alinhamento
        linha_formatada = f'    {{"{nao_terminal}",{" " * (20 - len(nao_terminal))}"{terminal}",{" " * (20 - len(terminal))}"{regra}"}},\n'
        linhas_ordenadas.append(linha_formatada)
    
    # Juntar todas as partes
    conteudo_final = ''.join(parte1) + ''.join(linhas_ordenadas) + ''.join(parte3)
    
    # Escrever no arquivo de saída ou imprimir
    if saida:
        with open(saida, 'w') as f:
            f.write(conteudo_final)
    else:
        print(conteudo_final)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Uso: python ordenar_tabela.py <arquivo_entrada> [arquivo_saida]")
        sys.exit(1)
    
    entrada = sys.argv[1]
    saida = sys.argv[2] if len(sys.argv) >= 3 else None
    ordenar_tabela_analise(entrada, saida)