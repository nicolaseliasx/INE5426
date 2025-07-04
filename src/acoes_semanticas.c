#define _DEFAULT_SOURCE

#include "acoes_semanticas.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "erros.h"
#include "gerenciador_escopo.h"
#include "no_ast.h"
#include "resolvedor_expressao.h"
#include "tabela_simbolos.h"
#include "token.h"

static ResolvedorExpressao* resolvedor_global = NULL;

void inicializar_resolvedor_global()
{
    if (resolvedor_global == NULL)
    {
        resolvedor_global = criar_resolvedor_expressao();
    }
}

// Garante que o primeiro operando de uma expressão esteja em uma variável.
static char* garantir_primeiro_operando_em_var(NoAST* node, ListaString* codigo)
{
    if (node->res_var_codigo.var && strlen(node->res_var_codigo.var) > 0)
    {
        return strdup(node->res_var_codigo.var);
    }

    const char* valor = node->sdt_mat.no->valor;
    if (isalpha(valor[0]) || valor[0] == '$')
    {
        return strdup(valor);
    }

    char* nova_var = gerar_variavel_temporaria(resolvedor_global);
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "%s = %s", nova_var, valor);
    adicionar_string(codigo, buffer);
    return nova_var;
}

// Obtém o valor de um operando a direita, que pode ser um literal ou variavel.
static char* obter_valor_rhs(NoAST* node)
{
    if (node->res_var_codigo.var && strlen(node->res_var_codigo.var) > 0)
    {
        return strdup(node->res_var_codigo.var);
    }
    return strdup(node->sdt_mat.no->valor);
}

// =================================================================
// Implementações do namespace CODIGO (Geração de código)
// =================================================================

void CODIGO_relop_action(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    (void)gerenciador;
    // REGRA: EXPRESSION' -> relop NUMEXPRESSION
    // 'no_pai' aqui é o nó para EXPRESSION'
    NoAST* relop = no_pai->filhos[0];
    NoAST* numexpression = no_pai->filhos[1];

    // 1. O nome do operando da esquerda (ex: "a") foi herdado e está em
    // 'no_pai->res_var_codigo.var'
    char* var_lhs = no_pai->res_var_codigo.var;

    // 2. Processa o código do operando da direita e obtém seu nome (ex: "4" ou "$temp")
    lista_codigo_adicionar_lista(no_pai->codigo, numexpression->codigo);
    char* var_rhs = obter_valor_rhs(numexpression);  // Usa a função auxiliar que você já tem

    // 3. Cria uma NOVA variável temporária para o resultado
    char* var_resultado = gerar_variavel_temporaria(resolvedor_global);

    // 4. Gera o código de comparação NÃO-DESTRUTIVO
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "%s = %s %s %s",
             var_resultado,         // Armazena o resultado na nova temporária (ex: $1)
             var_lhs,               // Usa o operando da esquerda (ex: a)
             relop->token->lexema,  // O operador (ex: <)
             var_rhs);              // Usa o operando da direita (ex: 4)
    adicionar_string(no_pai->codigo, buffer);

    // 5. Sintetiza o resultado: atualiza a variável do nó atual ('no_pai')
    // para que o pai dele (EXPRESSION) possa usar o resultado correto.
    free(no_pai->res_var_codigo.var);            // Libera o valor herdado ("a")
    no_pai->res_var_codigo.var = var_resultado;  // Atribui o nome da nova temporária ("$1")

    // Libera a memória que não será mais usada
    free(var_rhs);
}

void CODIGO_lidar_atribuicao_funcao(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    (void)gerenciador;
    // 'no_pai' aqui é o nó para a regra ATRIB'
    NoAST* funccall_node = no_pai->filhos[0];

    // Parte 1: Define a variável de resultado como "$rv"
    if (no_pai->res_var_codigo.var)
    {
        free(no_pai->res_var_codigo.var);
    }
    no_pai->res_var_codigo.var = strdup("$rv");

    // Parte 2: [A CORREÇÃO CRÍTICA] Copia o código do filho para o pai.
    // Isso traz as instruções "call fn", "Lxx:", etc., para o lugar certo.
    lista_codigo_adicionar_lista(no_pai->codigo, funccall_node->codigo);
}

void CODIGO_lidar_break(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    (void)gerenciador;
    // STATEMENT -> break ;
    // STATEMENT.code = goto STATEMENT.for_nxt [0]
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "goto %s", no_pai->para_proximo);
    adicionar_string(no_pai->codigo, buffer);
}

void CODIGO_lidar_leitura(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    (void)gerenciador;
    // READSTAT -> read LVALUE
    NoAST* lvalue = no_pai->filhos[1];

    // O nó lvalue já foi resolvido pela sua ação semântica.
    // Precisamos pegar o código gerado por ela e usar o endereço final.
    lista_codigo_adicionar_lista(no_pai->codigo, lvalue->codigo);

    const char* endereco = lvalue->sdt_mat.no->valor;

    char buffer[256];
    snprintf(buffer, sizeof(buffer), "read %s", endereco);
    adicionar_string(no_pai->codigo, buffer);
}

void CODIGO_lidar_impressao(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    (void)gerenciador;
    // PRINTSTAT -> print EXPRESSION
    // PRINTSTAT.code = EXPRESSION.code || print EXPRESSION.var [0]
    NoAST* expressao = no_pai->filhos[1];

    lista_codigo_adicionar_lista(no_pai->codigo, expressao->codigo);

    char buffer[256];
    snprintf(buffer, sizeof(buffer), "print %s", expressao->res_var_codigo.var);
    adicionar_string(no_pai->codigo, buffer);
}

void CODIGO_obter_variavel_lvalue(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    (void)gerenciador;
    // LVALUE -> ident ALLOCAUX
    // LVALUE.var = ident.lexema [0]
    NoAST* ident = no_pai->filhos[0];
    no_pai->res_var_codigo.var = strdup(ident->token->lexema);
}

void CODIGO_lidar_retorno(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    (void)gerenciador;
    if (no_pai->filhos[1] && no_pai->filhos[1]->codigo)
    {
        lista_codigo_adicionar_lista(no_pai->codigo, no_pai->filhos[1]->codigo);
    }

    if (no_pai->proximo)
    {
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "goto %s", no_pai->proximo);
        adicionar_string(no_pai->codigo, buffer);
    }
}

void CODIGO_lidar_retorno2(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    (void)gerenciador;
    // RETURNSTAT' -> EXPRESSION
    if (no_pai->filhos[0]->res_var_codigo.var)
    {
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "$rv = %s", no_pai->filhos[0]->res_var_codigo.var);
        adicionar_string(no_pai->codigo, buffer);
    }
}

void CODIGO_obter_parametros(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    (void)gerenciador;
    // REGRA: PARAMLISTCALL' -> , PARAMLISTCALL
    // Ação: Passa para cima tanto o contador quanto o CÓDIGO do filho.
    NoAST* paramlist_filho = no_pai->filhos[1];

    // 1. Copia o contador de parâmetros do filho para o pai.
    no_pai->res_var_codigo.contador_param = paramlist_filho->res_var_codigo.contador_param;

    // 2. Anexa a lista de código do filho (com seus 'param ...')
    //    à lista de código do nó pai (PARAMLISTCALL').
    lista_codigo_adicionar_lista(no_pai->codigo, paramlist_filho->codigo);
}

void CODIGO_chamada_paramlist(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    (void)gerenciador;
    // REGRA: PARAMLISTCALL -> LVALUE PARAMLISTCALL'
    NoAST* lvalue_node = no_pai->filhos[0];
    NoAST* paramlist_linha = no_pai->filhos[1];

    // Concatena o código gerado pelo LVALUE (importante para cálculo de índice de array)
    lista_codigo_adicionar_lista(no_pai->codigo, lvalue_node->codigo);

    // Incrementa o contador de parâmetros
    no_pai->res_var_codigo.contador_param = 1 + paramlist_linha->res_var_codigo.contador_param;

    char buffer[256];
    // Usa o resultado já processado pelo LVALUE.
    // Isso funciona tanto para "var" quanto para "vetor[$t1]".
    snprintf(buffer, sizeof(buffer), "param %s", lvalue_node->sdt_mat.no->valor);
    adicionar_string(no_pai->codigo, buffer);

    // Anexa a lista de código com os parâmetros subsequentes
    lista_codigo_adicionar_lista(no_pai->codigo, paramlist_linha->codigo);
}

void CODIGO_chamada_funcao(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    (void)gerenciador;
    NoAST* ident = no_pai->filhos[2];
    NoAST* paramlistcall = no_pai->filhos[4];

    no_pai->res_var_codigo.ret = no_ast_gerar_novo_rotulo(no_pai);

    lista_codigo_adicionar_lista(no_pai->codigo, paramlistcall->codigo);

    char buffer[256];

    // 1. Guardar endereço de retorno
    snprintf(buffer, sizeof(buffer), "$r = %s", no_pai->res_var_codigo.ret);
    adicionar_string(no_pai->codigo, buffer);

    // 2. Chamar função
    snprintf(buffer, sizeof(buffer), "call %s %d", ident->token->lexema,
             paramlistcall->res_var_codigo.contador_param);
    adicionar_string(no_pai->codigo, buffer);

    // 3. Rótulo de retorno
    snprintf(buffer, sizeof(buffer), "%s:", no_pai->res_var_codigo.ret);
    adicionar_string(no_pai->codigo, buffer);
}

void CODIGO_ir_para_principal(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    (void)gerenciador;
    // PROGRAM -> FUNCLIST PROGRAM'
    // PROGRAM.code -> goto MAIN || FUNCLIST.code || MAIN: || PROGRAM'.code [1]
    NoAST* funclist = no_pai->filhos[0];
    NoAST* program_linha = no_pai->filhos[1];

    adicionar_string(no_pai->codigo, strdup("goto MAIN"));
    lista_codigo_adicionar_lista(no_pai->codigo, funclist->codigo);
    adicionar_string(no_pai->codigo, strdup("MAIN:"));
    lista_codigo_adicionar_lista(no_pai->codigo, program_linha->codigo);
}

void CODIGO_funcdef_h(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    (void)gerenciador;
    // FUNCDEF -> def ident ( PARAMLIST ) { STATELIST }
    // begin = newlabel
    // STATELIST.nxt = 'return'
    // FUNCDEF.cod = begin: || STATELIST.code || goto return
    NoAST* ident = no_pai->filhos[1];
    no_pai->res_var_codigo.inicio = strdup(ident->token->lexema);
    NoAST* statelist = no_pai->filhos[6];
    statelist->proximo = strdup("$r");
}

void CODIGO_funcdef_s(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    (void)gerenciador;
    // FUNCDEF -> def ident ( PARAMLIST ) { STATELIST }
    // FUNCDEF.cod = begin: || STATELIST.code || goto STATELIST.nxt
    NoAST* statelist = no_pai->filhos[6];

    char buffer[256];
    snprintf(buffer, sizeof(buffer), "%s:", no_pai->res_var_codigo.inicio);
    adicionar_string(no_pai->codigo, buffer);

    lista_codigo_adicionar_lista(no_pai->codigo, statelist->codigo);

    snprintf(buffer, sizeof(buffer), "goto %s", statelist->proximo);
    adicionar_string(no_pai->codigo, buffer);
}

void CODIGO_acao_else_vazio(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    (void)gerenciador;
    // IFSTAT' -> '' (epsilon)
    // IFSTAT'.begin = IFSTAT'.nxt
    no_pai->res_var_codigo.inicio = strdup(no_pai->proximo);
}

void CODIGO_acao_else_h(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    (void)gerenciador;
    // IFSTAT' -> else { STATEMENT }
    // begin = newlabel()
    // STATEMENT.nxt = IFSTAT'.nxt
    // IFSTAT'.code = begin: || statement.code || goto STATEMENT.nxt
    NoAST* statement = no_pai->filhos[2];
    statement->proximo = strdup(no_pai->proximo);
    no_pai->res_var_codigo.inicio = no_ast_gerar_novo_rotulo(no_pai);
}

void CODIGO_acao_else_s(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    (void)gerenciador;
    // IFSTAT' -> else { STATEMENT }
    // IFSTAT'.code = begin: || statement.code || goto STATEMENT.nxt
    NoAST* statement = no_pai->filhos[2];

    char buffer[256];
    snprintf(buffer, sizeof(buffer), "%s:", no_pai->res_var_codigo.inicio);
    adicionar_string(no_pai->codigo, buffer);

    lista_codigo_adicionar_lista(no_pai->codigo, statement->codigo);

    snprintf(buffer, sizeof(buffer), "goto %s", statement->proximo);
    adicionar_string(no_pai->codigo, buffer);
}

void CODIGO_acao_if_s(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    (void)gerenciador;
    // REGRA: IFSTAT -> if ( EXPRESSION ) { STATEMENT } IFSTAT'
    NoAST* expressao = no_pai->filhos[2];
    NoAST* statement = no_pai->filhos[5];
    NoAST* ifstat_linha = no_pai->filhos[7];

    // 1. Anexa o código gerado para avaliar a expressão (ex: x = a + b)
    lista_codigo_adicionar_lista(no_pai->codigo, expressao->codigo);

    // A variável que guarda o resultado da condição (pode ser "x" ou "$t1")
    char* var_condicao = expressao->res_var_codigo.var;

    // Ponteiro para a variável que terá o resultado booleano final (0 ou 1)
    char* var_booleana_final;

    // --- [INÍCIO DA NOVA LÓGICA DE CORREÇÃO] ---

    // 2. Detecta se a expressão é uma comparação ou um valor numérico.
    //    Acessamos o nó EXPRESSION' (filho 1 do nó 'expressao')
    NoAST* expression_linha_node = expressao->filhos[1];

    // Se o nó EXPRESSION' não tem filhos, a regra usada foi '-> ε' (epsilon).
    // Isso significa que não houve um 'relop' (>, <, ==, etc.).
    bool precisa_comparacao_explicita = (expression_linha_node->quantidade_filhos == 0);

    if (precisa_comparacao_explicita)
    {
        // CASO: if(x) ou if(a + b) ou if(5)
        // O resultado da condição é um número, não um booleano.
        // Precisamos gerar a comparação explícita: var != 0

        // Gera uma nova variável temporária para guardar o resultado booleano.
        var_booleana_final = gerar_variavel_temporaria(resolvedor_global);

        char buffer_comp[256];
        // Gera a instrução de código intermediário: $temp = x != 0
        snprintf(buffer_comp, sizeof(buffer_comp), "%s = %s != 0", var_booleana_final,
                 var_condicao);
        adicionar_string(no_pai->codigo, buffer_comp);
    }
    else
    {
        // CASO: if(x > 5) ou if(a == b)
        // O resultado já é um booleano (0 ou 1) gerado pela ação `CODIGO_relop_action`.
        // Podemos usá-lo diretamente.
        var_booleana_final = strdup(var_condicao);
    }

    // --- [FIM DA NOVA LÓGICA DE CORREÇÃO] ---

    // 3. Gera o desvio condicional usando a variável que garantidamente contém um booleano.
    char buffer_if[256];
    snprintf(buffer_if, sizeof(buffer_if), "if False %s goto %s",
             var_booleana_final,                    // Usa a variável booleana
             ifstat_linha->res_var_codigo.inicio);  // Rótulo para o bloco else ou para depois do if
    adicionar_string(no_pai->codigo, buffer_if);

    // Libera a memória da string que foi duplicada ou gerada.
    free(var_booleana_final);

    // 4. Anexa o resto do código (corpo do if, goto para o final, corpo do else)
    lista_codigo_adicionar_lista(no_pai->codigo, statement->codigo);

    snprintf(buffer_if, sizeof(buffer_if), "goto %s", statement->proximo);
    adicionar_string(no_pai->codigo, buffer_if);

    lista_codigo_adicionar_lista(no_pai->codigo, ifstat_linha->codigo);
}

void CODIGO_acao_for(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    (void)gerenciador;
    // FORSTAT -> for ( ATRIBSTAT1 ; EXPRESSION ; ATRIBSTAT2 ) STATEMENT
    // ALL_PRODS.nxt = FORSTAT.next
    // begin = newlabel()
    // FORSTAT.code = ATRIBSTAT1 || begin: || EXPRESSION
    //                || if not EXPRESSION.val goto FORSTAT.next ||
    //                STATEMENT.code || ATRIBSTAT2
    //                || goto begin
    NoAST* atribstat1 = no_pai->filhos[2];
    NoAST* expressao = no_pai->filhos[4];
    NoAST* atribstat2 = no_pai->filhos[6];
    NoAST* statement = no_pai->filhos[8];

    char* rotulo_inicio_laco = no_ast_gerar_novo_rotulo(no_pai);

    lista_codigo_adicionar_lista(no_pai->codigo, atribstat1->codigo);

    char buffer[256];
    snprintf(buffer, sizeof(buffer), "%s:", rotulo_inicio_laco);
    adicionar_string(no_pai->codigo, buffer);

    lista_codigo_adicionar_lista(no_pai->codigo, expressao->codigo);

    snprintf(buffer, sizeof(buffer), "if False %s goto %s", expressao->res_var_codigo.var,
             no_pai->proximo);  // FORSTAT.next is no_pai->proximo
    adicionar_string(no_pai->codigo, buffer);

    lista_codigo_adicionar_lista(no_pai->codigo, statement->codigo);
    lista_codigo_adicionar_lista(no_pai->codigo, atribstat2->codigo);

    snprintf(buffer, sizeof(buffer), "goto %s", rotulo_inicio_laco);
    adicionar_string(no_pai->codigo, buffer);
}

void CODIGO_herdar_proximo_for(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    (void)gerenciador;
    // FORSTAT -> for ( ATRIBSTAT1 ; EXPRESSION ; ATRIBSTAT2 ) STATEMENT
    NoAST* statement = no_pai->filhos[8];
    statement->para_proximo = strdup(no_pai->proximo);  // Inherit for_nxt from father
}

void CODIGO_for_setup_h(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    (void)gerenciador;
    // O 'no_pai' aqui é o FORSTAT.
    // O corpo do laço (STATEMENT) é o último filho adicionado até agora.
    if (no_pai->quantidade_filhos == 0) return;

    NoAST* statement_node = no_pai->filhos[no_pai->quantidade_filhos - 1];

    // 1. Gera um novo rótulo para o passo de incremento. Ex: "L58"
    char* rotulo_incremento = no_ast_gerar_novo_rotulo(no_pai);

    // 2. Define o atributo 'proximo' do corpo do laço para apontar para este rótulo.
    //    Agora, qualquer 'goto' no final do corpo saberá para onde ir.
    if (statement_node->proximo) free(statement_node->proximo);
    statement_node->proximo = strdup(rotulo_incremento);

    // 3. Armazena este rótulo no próprio nó FORSTAT para que a ação final possa usá-lo.
    //    Usaremos o campo 'ret' de 'res_var_codigo' como um "bolso" temporário.
    if (no_pai->res_var_codigo.ret) free(no_pai->res_var_codigo.ret);
    no_pai->res_var_codigo.ret = rotulo_incremento;
}

void CODIGO_acao_for_s(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    (void)gerenciador;
    // FORSTAT -> for ( ATRIBSTAT1 ; EXPRESSION ; ATRIBSTAT2 ) STATEMENT
    NoAST* atribstat1 = no_pai->filhos[2];
    NoAST* expressao = no_pai->filhos[4];
    NoAST* atribstat2 = no_pai->filhos[6];
    NoAST* statement = no_pai->filhos[8];

    // Gera o rótulo para o início do laço (teste da condição)
    char* rotulo_inicio_laco = no_ast_gerar_novo_rotulo(no_pai);

    // Pega o rótulo do incremento que foi salvo pela ação de setup
    char* rotulo_incremento = no_pai->res_var_codigo.ret;

    char buffer[256];

    // --- Montagem do Código Final ---

    // 1. Código da inicialização (executa uma vez)
    lista_codigo_adicionar_lista(no_pai->codigo, atribstat1->codigo);

    // 2. Rótulo do início do laço
    snprintf(buffer, sizeof(buffer), "%s:", rotulo_inicio_laco);
    adicionar_string(no_pai->codigo, buffer);

    // 3. Código da expressão de condição
    lista_codigo_adicionar_lista(no_pai->codigo, expressao->codigo);

    // 4. Salto condicional para FORA do laço se a condição for falsa
    //    O 'no_pai->proximo' aqui é o rótulo de saída do laço, herdado corretamente.
    snprintf(buffer, sizeof(buffer), "if False %s goto %s", expressao->res_var_codigo.var,
             no_pai->proximo);
    adicionar_string(no_pai->codigo, buffer);

    // 5. Código do corpo do laço
    lista_codigo_adicionar_lista(no_pai->codigo, statement->codigo);

    // 6. Rótulo do passo de incremento (o 'goto' do final do corpo aponta para cá)
    snprintf(buffer, sizeof(buffer), "%s:", rotulo_incremento);
    adicionar_string(no_pai->codigo, buffer);

    // 7. Código do incremento
    lista_codigo_adicionar_lista(no_pai->codigo, atribstat2->codigo);

    // 8. Salto incondicional de volta para o início do laço (para re-avaliar a condição)
    snprintf(buffer, sizeof(buffer), "goto %s", rotulo_inicio_laco);
    adicionar_string(no_pai->codigo, buffer);

    // Libera a memória que não será mais usada
    free(rotulo_inicio_laco);
    // O 'rotulo_incremento' foi movido de res_var_codigo.ret para o código, então não precisa de
    // free aqui.
}

void CODIGO_atribuicao(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    (void)gerenciador;
    // ATRIBSTAT -> LVALUE = ATRIBSTAT'
    NoAST* lvalue = no_pai->filhos[0];
    NoAST* atribstat_linha = no_pai->filhos[2];

    // O lvalue já vem resolvido pela ação CODIGO_avaliar_identificador
    // A string de acesso já está em lvalue->sdt_mat.no->valor
    const char* endereco_lhs = lvalue->sdt_mat.no->valor;

    // Junta os códigos
    lista_codigo_adicionar_lista(no_pai->codigo, lvalue->codigo);
    lista_codigo_adicionar_lista(no_pai->codigo, atribstat_linha->codigo);

    // Gera a instrução final
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "%s = %s", endereco_lhs, atribstat_linha->res_var_codigo.var);
    adicionar_string(no_pai->codigo, buffer);
}

void CODIGO_definir_valor_expressao(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    (void)gerenciador;
    NoAST* expressao = no_pai->filhos[0];
    no_pai->res_var_codigo.var = strdup(expressao->res_var_codigo.var);

    if (no_pai->codigo)
    {
        liberar_lista_string(no_pai->codigo);
    }

    no_pai->codigo = expressao->codigo;
    expressao->codigo = NULL;
}

void CODIGO_obter_codigo_filhos(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    (void)gerenciador;
    // Generic action to splice code from all children to the father
    for (size_t i = 0; i < (size_t)no_pai->quantidade_filhos; i++)
    {
        lista_codigo_adicionar_lista(no_pai->codigo, no_pai->filhos[i]->codigo);
    }
}

void CODIGO_combinar_codigo_para_lista(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    (void)gerenciador;
    if (no_pai->quantidade_filhos != 2) return;

    NoAST* statement = no_pai->filhos[0];
    NoAST* statelist_prime = no_pai->filhos[1];

    // Inicializa código do nó pai se necessário
    if (!no_pai->codigo)
    {
        no_pai->codigo = criar_lista_string();
    }

    // 1. Adiciona código do statement
    lista_codigo_adicionar_lista(no_pai->codigo, statement->codigo);

    // 2. Adiciona APENAS o rótulo de próximo do statement
    if (statement->proximo)
    {
        char rotulo[256];
        snprintf(rotulo, sizeof(rotulo), "%s:", statement->proximo);
        adicionar_string(no_pai->codigo, rotulo);
    }

    // 3. Adiciona código do restante da lista (STATELIST')
    lista_codigo_adicionar_lista(no_pai->codigo, statelist_prime->codigo);
}

void CODIGO_herdar_proximo(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    (void)gerenciador;
    // Generic action to inherit 'nxt' and 'for_nxt' attributes to children
    for (size_t i = 0; i < (size_t)no_pai->quantidade_filhos; i++)
    {
        if (no_pai->filhos[i])
        {  // Check for non-NULL child
            if (no_pai->proximo)
            {
                no_pai->filhos[i]->proximo = strdup(no_pai->proximo);
            }
            if (no_pai->para_proximo)
            {
                no_pai->filhos[i]->para_proximo = strdup(no_pai->para_proximo);
            }
        }
    }
}

void CODIGO_definir_proximo_filho(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    (void)gerenciador;
    if (no_pai->quantidade_filhos > 0)
    {
        NoAST* statement = no_pai->filhos[0];

        // Libera rótulo anterior se existir
        if (statement->proximo)
        {
            free(statement->proximo);
        }

        // Gera novo rótulo para o final deste statement
        statement->proximo = no_ast_gerar_novo_rotulo(no_pai);
    }
}

void CODIGO_definir_rotulo_final(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    (void)gerenciador;
    no_pai->proximo = strdup("END");
}

void CODIGO_gerar_codigo_final(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    (void)gerenciador;
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "%s:", no_pai->proximo);
    adicionar_string(no_pai->codigo, buffer);
}

void CODIGO_imprimir_codigo(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    (void)gerenciador;
    // Print each line of code
    // Assuming lista_codigo_imprimir_tudo exists
    // Otherwise, iterate through no_pai->codigo and print each string
    lista_codigo_imprimir_tudo(no_pai->codigo);
}

// =================================================================
// Implementações do namespace ESCOPO (Gerenciamento de escopos)
// =================================================================

void ESCOPO_acao1(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    (void)no_pai;
    // PROGRAM  ->  STATEMENT
    // PROGRAM  ->  FUNCLIST PROGRAM'
    // PROGRAM  ->  ''
    // STATEMENT  ->  { STATELIST }
    gerenciador_abrir_escopo(gerenciador, "");  // Default scope type
}

void ESCOPO_acao2(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    (void)no_pai;
    // FUNCDEF  ->  def ident ( PARAMLIST ) { STATELIST }
    // STATEMENT  ->  { STATELIST }
    // IFSTAT  -> if ( EXPRESSION ) { STATEMENT } IFSTAT'
    // IFSTAT' -> else { STATEMENT }
    // FORSTAT  ->  for ( ATRIBSTAT ; EXPRESSION ; ATRIBSTAT ) STATEMENT
    gerenciador_fechar_escopo(gerenciador);
}

void ESCOPO_acao3(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    // FUNCDEF  ->  def ident ( PARAMLIST ) { STATELIST }
    // PARAMLIST  ->  int ident PARAMLIST'
    // PARAMLIST  ->  float ident PARAMLIST'
    // PARAMLIST  ->  string ident PARAMLIST'
    // VARDECL  ->  int ident INDEX
    // VARDECL  ->  float ident INDEX
    // VARDECL  ->  string ident INDEX
    // add_symbol(ident)
    NoAST* ident = no_pai->filhos[1];
    gerenciador_escopo_adicionar_simbolo(gerenciador, ident->token);
}

void ESCOPO_acao4(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    (void)no_pai;
    // FUNCDEF  ->  def ident ( PARAMLIST ) { STATELIST }
    gerenciador_abrir_escopo(gerenciador, "function");
}

void ESCOPO_acao5(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    (void)no_pai;
    // IFSTAT  -> if ( EXPRESSION ) { STATEMENT } IFSTAT'
    gerenciador_abrir_escopo(gerenciador, "if");
}

void ESCOPO_acao6(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    (void)no_pai;
    // IFSTAT' -> else { STATEMENT }
    gerenciador_abrir_escopo(gerenciador, "else");
}

void ESCOPO_acao7(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    (void)no_pai;
    // FORSTAT  ->  for ( ATRIBSTAT ; EXPRESSION ; ATRIBSTAT ) STATEMENT
    gerenciador_abrir_escopo(gerenciador, "for");
}

// =================================================================
// Implementações do namespace DEC (Declarações de tipos)
// =================================================================

void DEC_acao1(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    // FUNCDEF -> def ident ( PARAMLIST ) { STATELIST }
    // add_type(ident.val_lex, "function")
    NoAST* ident = no_pai->filhos[1];
    gerenciador_definir_tipo_simbolo(gerenciador, ident->token, strdup("function"));
}

void DEC_acao2(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    // PARAMLIST -> int ident PARAMLIST'
    // add_type(ident.val_lex, "int")
    NoAST* ident = no_pai->filhos[1];
    gerenciador_definir_tipo_simbolo(gerenciador, ident->token, strdup("int"));
}

void DEC_acao3(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    // PARAMLIST -> float ident PARAMLIST'
    // add_type(ident.val_lex, "float")
    NoAST* ident = no_pai->filhos[1];
    gerenciador_definir_tipo_simbolo(gerenciador, ident->token, strdup("float"));
}

void DEC_acao4(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    // PARAMLIST -> string ident PARAMLIST'
    // add_type(ident.val_lex, "string")
    NoAST* ident = no_pai->filhos[1];
    gerenciador_definir_tipo_simbolo(gerenciador, ident->token, strdup("string"));
}

void DEC_acao5(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    (void)gerenciador;
    // INDEX -> [int_constant] INDEX1
    // INDEX1.initial_type = INDEX.initial_type
    NoAST* index = no_pai;
    NoAST* index1 = no_pai->filhos[3];
    index1->sdt_dec.tipo_inicial = strdup(index->sdt_dec.tipo_inicial);
}

void DEC_acao6(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    (void)gerenciador;
    NoAST* index = no_pai;
    NoAST* index1 = no_pai->filhos[3];
    NoAST* int_const = no_pai->filhos[1];
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "array(%s, %s)", int_const->token->lexema,
             index1->sdt_dec.tipo);
    index->sdt_dec.tipo = strdup(buffer);
}

void DEC_acao7(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    (void)gerenciador;
    // INDEX-> '' (epsilon)
    // INDEX.type = INDEX.initial_type
    NoAST* index = no_pai;
    index->sdt_dec.tipo = strdup(index->sdt_dec.tipo_inicial);
}

void DEC_acao8(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    (void)gerenciador;
    // VARDECL -> int ident INDEX
    // INDEX.initial_type = "int"
    NoAST* index = no_pai->filhos[2];
    index->sdt_dec.tipo_inicial = strdup("int");
}

void DEC_acao9(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    (void)gerenciador;
    // VARDECL -> float ident INDEX
    // INDEX.initial_type = "float"
    NoAST* index = no_pai->filhos[2];
    index->sdt_dec.tipo_inicial = strdup("float");
}

void DEC_acao10(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    (void)gerenciador;
    // VARDECL -> string ident INDEX
    // INDEX.initial_type = "string"
    NoAST* index = no_pai->filhos[2];
    index->sdt_dec.tipo_inicial = strdup("string");
}

void DEC_acao11(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    // VARDECL -> int ident INDEX
    // VARDECL -> float ident INDEX
    // VARDECL -> string ident INDEX
    // add_type(ident.val_lex, INDEX.type)
    NoAST* ident = no_pai->filhos[1];
    NoAST* index = no_pai->filhos[2];
    gerenciador_definir_tipo_simbolo(gerenciador, ident->token, strdup(index->sdt_dec.tipo));
}

// =================================================================
// Implementações do namespace DECLARAR_VERIFICAR (Verificação de declarações)
// =================================================================

void DECLARAR_VERIFICAR_acao1(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    // FUNCCALL -> call ( ident ( PARAMLISTCALL ) )
    // is_declared(ident.val_lex)
    // is_type(ident.val_lex, "function")
    NoAST* ident = no_pai->filhos[2];
    if (!gerenciador_simbolo_declarado(gerenciador, ident->token))
    {
        char msg_error[512];
        snprintf(msg_error, sizeof(msg_error), "%s usado antes da declaracao na linha %d:%d",
                 ident->token->lexema, ident->token->linha, ident->token->coluna);
        // Assuming your 'erros.h' has a function like `LANCAR_ERRO_SEMANTICO`
        LANCAR_ERRO_SEMANTICO(msg_error);
    }
    if (!gerenciador_simbolo_e_tipo(gerenciador, ident->token, "function"))
    {
        char msg_error[512];
        snprintf(msg_error, sizeof(msg_error), "%s na linha %d:%d nao e uma funcao",
                 ident->token->lexema, ident->token->linha, ident->token->coluna);
        LANCAR_ERRO_SEMANTICO(msg_error);
    }
}

void DECLARAR_VERIFICAR_acao2(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    // A regra agora é PARAMLISTCALL -> LVALUE ...
    // O 'no_pai' da ação é o nó PARAMLISTCALL.
    // O nó LVALUE é seu primeiro filho.
    NoAST* lvalue_node = no_pai->filhos[0];

    // O nó IDENT é o primeiro filho do LVALUE.
    NoAST* ident_node = lvalue_node->filhos[0];

    if (!gerenciador_simbolo_declarado(gerenciador, ident_node->token))
    {
        char msg_error[512];
        snprintf(msg_error, sizeof(msg_error), "%s usado antes da declaracao na linha %d:%d",
                 ident_node->token->lexema, ident_node->token->linha, ident_node->token->coluna);
        LANCAR_ERRO_SEMANTICO(msg_error);
    }

    gerenciador_registrar_uso_simbolo(gerenciador, ident_node->token);
}

// =================================================================
// Implementações do namespace BREAK (Verificação de break)
// =================================================================

void BREAK_acao1(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    // STATEMENT -> break ;
    if (!gerenciador_esta_no_escopo(gerenciador, "for"))
    {
        NoAST* break_t = no_pai->filhos[0];
        char msg_error[512];
        snprintf(msg_error, sizeof(msg_error), "break usado fora de um comando for na linha %d:%d",
                 break_t->token->linha, break_t->token->coluna);
        LANCAR_ERRO_SEMANTICO(msg_error);
    }
}

// =================================================================
// Implementações do namespace AUXILIAR (Funções auxiliares)
// =================================================================

char* AUXILIAR_obter_tipo(const char* tipo_str)
{
    const char* f = tipo_str;
    const char* ultimo_tipo_encontrado = NULL;
    while (*f != '\0')
    {
        if (*f == 'i' && strncmp(f, "int", 3) == 0)
        {
            ultimo_tipo_encontrado = "int";
        }
        else if (*f == 'f' && strncmp(f, "float", 5) == 0)
        {
            ultimo_tipo_encontrado = "float";
        }
        else if (*f == 's' && strncmp(f, "string", 6) == 0)
        {
            ultimo_tipo_encontrado = "string";
        }
        f++;
    }

    if (ultimo_tipo_encontrado)
    {
        return strdup(ultimo_tipo_encontrado);
    }
    return strdup(tipo_str);
}

int* AUXILIAR_obter_tamanhos_vetor(const char* tipo_str, int* num_elementos)
{
    if (!tipo_str || !num_elementos)
    {
        *num_elementos = 0;
        return NULL;
    }

    int temp_values[32];  // Buffer temporário para até 32 dimensões
    int count = 0;
    const char* ptr = tipo_str;

    // A lógica correta é procurar por números dentro da string
    while (*ptr != '\0' && count < 32)
    {
        // Pula caracteres não numéricos
        if (!isdigit((unsigned char)*ptr))
        {
            ptr++;
            continue;
        }

        // Encontrou um dígito, agora lê o número inteiro completo
        char* end_ptr;
        long val = strtol(ptr, &end_ptr, 10);
        temp_values[count++] = (int)val;
        ptr = end_ptr;  // Avança o ponteiro para depois do número lido
    }

    *num_elementos = count;
    if (count == 0)
    {
        return NULL;
    }

    int* values = (int*)malloc(sizeof(int) * count);
    if (!values)
    {
        *num_elementos = 0;
        return NULL;
    }
    memcpy(values, temp_values, sizeof(int) * count);

    return values;
}

// =================================================================
// Implementações do namespace AEXP (Ações para expressões)
// =================================================================

// AÇÃO CENTRAL PARA RESOLVER LVALUES
void AEXP_avaliar_identificador_lvalue(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    // A regra é LVALUE -> IDENT ALLOCAUX
    NoAST* lvalue_node = no_pai;  // O nó pai é o próprio LVALUE
    NoAST* ident_node = lvalue_node->filhos[0];
    NoAST* allocaux_node = lvalue_node->filhos[1];

    // Verificar declaração
    if (!gerenciador_simbolo_declarado(gerenciador, ident_node->token))
    {
        char msg_error[512];
        snprintf(msg_error, sizeof(msg_error), "%s usado antes da declaracao na linha %d:%d",
                 ident_node->token->lexema, ident_node->token->linha, ident_node->token->coluna);
        LANCAR_ERRO_SEMANTICO(msg_error);
    }
    gerenciador_registrar_uso_simbolo(gerenciador, ident_node->token);

    char* tipo_str = gerenciador_obter_tipo_simbolo(gerenciador, ident_node->token);
    if (!tipo_str)
    {
        LANCAR_ERRO_SEMANTICO("Simbolo nao encontrado ou sem tipo definido.");
        return;
    }

    // Propaga informações do allocaux para o lvalue
    lvalue_node->sdt_mat.contador_vetor = allocaux_node->sdt_mat.contador_vetor;
    lista_codigo_adicionar_lista(lvalue_node->codigo, allocaux_node->codigo);

    if (lvalue_node->sdt_mat.contador_vetor > 0)  // É um acesso de array
    {
        // A lógica a seguir calcula o deslocamento (offset) para um array
        // multidimensional. Ela implementa corretamente o padrão "row-major order"
        // (ordem principal por linha), que é o padrão em C/C++.
        int num_dimensoes_declaradas = 0;
        int* dimensoes_maximas = AUXILIAR_obter_tamanhos_vetor(tipo_str, &num_dimensoes_declaradas);

        if (lvalue_node->sdt_mat.contador_vetor != num_dimensoes_declaradas)
        {
            char msg_error[512];
            snprintf(msg_error, sizeof(msg_error),
                     "Numero incorreto de indices para o array '%s'. Esperado: %d, Fornecido: %d",
                     ident_node->token->lexema, num_dimensoes_declaradas,
                     lvalue_node->sdt_mat.contador_vetor);
            LANCAR_ERRO_SEMANTICO(msg_error);
        }

        ListaString* variaveis_indices = allocaux_node->sdt_mat.vetor_array_var;
        char* somador_temp = gerar_variavel_temporaria(resolvedor_global);
        char buffer[256];

        snprintf(buffer, sizeof(buffer), "%s = 0", somador_temp);
        adicionar_string(lvalue_node->codigo, buffer);

        // O loop que efetivamente calcula o offset
        for (int i = 0; i < variaveis_indices->tamanho; i++)
        {
            char* var_indice_atual = variaveis_indices->itens[i];
            char* temp_mult = gerar_variavel_temporaria(resolvedor_global);
            snprintf(buffer, sizeof(buffer), "%s = %s", temp_mult, var_indice_atual);
            adicionar_string(lvalue_node->codigo, buffer);
            for (int j = i + 1; j < num_dimensoes_declaradas; j++)
            {
                snprintf(buffer, sizeof(buffer), "%s = %s * %d", temp_mult, temp_mult,
                         dimensoes_maximas[j]);
                adicionar_string(lvalue_node->codigo, buffer);
            }
            snprintf(buffer, sizeof(buffer), "%s = %s + %s", somador_temp, somador_temp, temp_mult);
            adicionar_string(lvalue_node->codigo, buffer);
            free(temp_mult);
        }

        char acesso_final[256];
        snprintf(acesso_final, sizeof(acesso_final), "%s[%s]", ident_node->token->lexema,
                 somador_temp);

        char* tipo_final = AUXILIAR_obter_tipo(tipo_str);
        lvalue_node->sdt_mat.no = criar_no_expressao_simples('n', tipo_final, acesso_final);

        free(somador_temp);
        free(dimensoes_maximas);
        free(tipo_final);
    }
    else  // É uma variável escalar
    {
        lvalue_node->sdt_mat.no =
            criar_no_expressao_simples('n', tipo_str, ident_node->token->lexema);
    }

    free(tipo_str);
}

void AEXP_lexema_para_valor(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    (void)gerenciador;
    // FACTOR -> int_constant | float_constant | string_constant
    // Factor.Node = new Node(value)
    NoAST* factor_node = no_pai;
    NoAST* constant_node = no_pai->filhos[0];
    char* type_str = NULL;

    if (strcmp(constant_node->identificador, "NI") == 0)
    {
        type_str = strdup("int");
    }
    else if (strcmp(constant_node->identificador, "NPF") == 0)
    {
        type_str = strdup("float");
    }
    else if (strcmp(constant_node->identificador, "STRC") == 0)
    {
        type_str = strdup("string");
    }

    if (factor_node->sdt_mat.no && factor_node->sdt_mat.no->tipo)
    {
        free(factor_node->sdt_mat.no->tipo);
    }

    factor_node->sdt_mat.no =
        criar_no_expressao_simples('n', type_str, constant_node->token->lexema);

    // O tipo agora pertence ao nó da expressão, não precisamos mais da cópia local.
    free(type_str);
}

void AEXP_definir_operacao(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    (void)gerenciador;
    NoAST* numexpr_aux_node = no_pai;
    NoAST* operation_node = no_pai->filhos[0];
    NoAST* numexpression_node = no_pai->filhos[1];

    numexpr_aux_node->sdt_mat.operacao = operation_node->token->lexema[0];
    numexpr_aux_node->sdt_mat.no = numexpression_node->sdt_mat.no;

    numexpression_node->sdt_mat.no = NULL;

    numexpr_aux_node->sdt_mat.contador_vetor = numexpression_node->sdt_mat.contador_vetor;
    lista_codigo_adicionar_lista(numexpr_aux_node->codigo, numexpression_node->codigo);
    copiar_res_var_codigo(&numexpr_aux_node->res_var_codigo, &numexpression_node->res_var_codigo);
}

void AEXP_ident_para_cima(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    (void)gerenciador;
    // FACTOR -> LVALUE
    NoAST* factor_node = no_pai;
    NoAST* lvalue_node = factor_node->filhos[0];

    // O lvalue já vem resolvido. Apenas propague a informação para o nó FACTOR.

    // Propaga o nó da árvore de expressão
    factor_node->sdt_mat.no = lvalue_node->sdt_mat.no;
    lvalue_node->sdt_mat.no = NULL;  // Evita double free

    // Propaga o contador
    factor_node->sdt_mat.contador_vetor = lvalue_node->sdt_mat.contador_vetor;

    // Propaga o código
    lista_codigo_adicionar_lista(factor_node->codigo, lvalue_node->codigo);

    // Se for um R-Value de array, precisamos carregar o valor em uma temp.
    if (factor_node->sdt_mat.contador_vetor > 0)
    {
        char* resultado_leitura = gerar_variavel_temporaria(resolvedor_global);
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "%s = %s", resultado_leitura,
                 factor_node->sdt_mat.no->valor);
        adicionar_string(factor_node->codigo, buffer);
        factor_node->res_var_codigo.var = resultado_leitura;
    }
    else  // Se for escalar, a variável já é o próprio nome/valor.
    {
        factor_node->res_var_codigo.var = strdup(factor_node->sdt_mat.no->valor);
    }
}

void AEXP_valor_para_cima(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    (void)gerenciador;
    // UNARYEXPR -> FACTOR
    NoAST* unaryexpr_node = no_pai;
    NoAST* factor_node = no_pai->filhos[0];
    unaryexpr_node->sdt_mat.contador_vetor = factor_node->sdt_mat.contador_vetor;
    unaryexpr_node->sdt_mat.no = factor_node->sdt_mat.no;
    factor_node->sdt_mat.no = NULL;
    copiar_res_var_codigo(&unaryexpr_node->res_var_codigo, &factor_node->res_var_codigo);
    lista_codigo_adicionar_lista(unaryexpr_node->codigo, factor_node->codigo);
}

void AEXP_valor_segundo_filho_para_cima(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    (void)gerenciador;
    // UNARYEXPR -> PLUS/MINUS FACTOR
    NoAST* unaryexpr_node = no_pai;
    NoAST* symbol_node = no_pai->filhos[0];  // PLUS or MINUS
    NoAST* factor_node = no_pai->filhos[1];

    char* var_operando = obter_valor_rhs(factor_node);
    unaryexpr_node->res_var_codigo.var = gerar_variavel_temporaria(resolvedor_global);

    char buffer[256];
    snprintf(buffer, sizeof(buffer), "%s = %c%s", unaryexpr_node->res_var_codigo.var,
             symbol_node->token->lexema[0], var_operando);
    adicionar_string(unaryexpr_node->codigo, buffer);

    // Create a new node representing the unary operation
    unaryexpr_node->sdt_mat.no =
        criar_no_expressao_unario(symbol_node->token->lexema[0], "N", factor_node->sdt_mat.no);
    free(var_operando);
}

void AEXP_gerar_no(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    (void)gerenciador;
    // REGRA: NUMEXPRESSION -> TERM NUMEXPRESSIONAUX
    NoAST* numexpr_node = no_pai;
    NoAST* term_node = numexpr_node->filhos[0];  // Primeiro operando (pode ser 3*a)
    NoAST* aux_node = numexpr_node->filhos[1];   // Resto da cadeia (+ 12 - 3 % 10)

    // Processa o código do primeiro operando. Seu resultado estará em term_node->res_var_codigo.var
    lista_codigo_adicionar_lista(numexpr_node->codigo, term_node->codigo);

    // Se não há operadores de + ou -, o trabalho acaba aqui.
    if (aux_node->quantidade_filhos == 0)
    {
        copiar_res_var_codigo(&numexpr_node->res_var_codigo, &term_node->res_var_codigo);
        numexpr_node->sdt_mat.no = term_node->sdt_mat.no;
        term_node->sdt_mat.no = NULL;
        return;
    }

    // Se há operadores, vamos processar a cadeia.
    // A variável 'var_resultado_esq' irá acumular o resultado.
    char* var_resultado_esq = strdup(term_node->res_var_codigo.var);
    NoExpressao* no_resultado_esq = term_node->sdt_mat.no;
    term_node->sdt_mat.no = NULL;

    // Loop para processar a cadeia de '+' e '-'
    while (aux_node != NULL && aux_node->quantidade_filhos > 0)
    {
        NoAST* op_node = aux_node->filhos[0];            // O operador '+' ou '-'
        NoAST* proximo_term_node = aux_node->filhos[1];  // O próximo operando (ex: '12')

        // Processa o código do operando da direita.
        lista_codigo_adicionar_lista(numexpr_node->codigo, proximo_term_node->codigo);
        char* var_operando_dir = strdup(proximo_term_node->res_var_codigo.var);

        // Cria uma nova temporária para o resultado
        char* var_novo_resultado = gerar_variavel_temporaria(resolvedor_global);

        // Gera a instrução: temp = acumulado op direita
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "%s = %s %s %s", var_novo_resultado, var_resultado_esq,
                 op_node->token->lexema, var_operando_dir);
        adicionar_string(numexpr_node->codigo, buffer);

        // Atualiza a árvore e o resultado acumulado para a próxima iteração
        no_resultado_esq = criar_no_expressao_binario(
            op_node->token->lexema[0], "N", no_resultado_esq, proximo_term_node->sdt_mat.no);
        proximo_term_node->sdt_mat.no = NULL;

        free(var_resultado_esq);
        free(var_operando_dir);
        var_resultado_esq = var_novo_resultado;

        // Avança para o próximo NUMEXPRESSIONAUX na cadeia
        aux_node = aux_node->filhos[2];
    }

    // Atribui o resultado final ao nó principal
    numexpr_node->sdt_mat.no = no_resultado_esq;

    // A linha problemática foi REMOVIDA.
    // Apenas precisamos garantir que a variável de resultado final seja atribuída corretamente.
    if (numexpr_node->res_var_codigo.var)
    {
        free(numexpr_node->res_var_codigo.var);
    }
    numexpr_node->res_var_codigo.var =
        var_resultado_esq;  // var_resultado_esq já contém o resultado final.
}

void AEXP_definir_operacao2(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    (void)gerenciador;
    NoAST* unaryexpr_aux_node = no_pai;
    NoAST* op_node = no_pai->filhos[0];
    NoAST* term_node = no_pai->filhos[1];

    unaryexpr_aux_node->sdt_mat.operacao = op_node->token->lexema[0];
    unaryexpr_aux_node->sdt_mat.no = term_node->sdt_mat.no;

    term_node->sdt_mat.no = NULL;

    unaryexpr_aux_node->sdt_mat.contador_vetor = term_node->sdt_mat.contador_vetor;
    lista_codigo_adicionar_lista(unaryexpr_aux_node->codigo, term_node->codigo);
    copiar_res_var_codigo(&unaryexpr_aux_node->res_var_codigo, &term_node->res_var_codigo);
}

void AEXP_termo(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    (void)gerenciador;
    NoAST* term_node = no_pai;  // O nó TERM que está sendo processado
    NoAST* unary_node = term_node->filhos[0];
    NoAST* aux_node = term_node->filhos[1];

    ListaString* codigo_operacoes = criar_lista_string();

    // Processa o primeiro operando (UNARYEXPR)
    lista_codigo_adicionar_lista(term_node->codigo, unary_node->codigo);
    char* var_resultado_esq = garantir_primeiro_operando_em_var(unary_node, term_node->codigo);
    NoExpressao* no_resultado_esq = unary_node->sdt_mat.no;
    unary_node->sdt_mat.no = NULL;

    // Itera pela cadeia de operações (*, /, %)
    while (aux_node && aux_node->quantidade_filhos > 0)
    {
        NoAST* op_node = aux_node->filhos[0];
        NoAST* proximo_unary_node = aux_node->filhos[1];

        lista_codigo_adicionar_lista(term_node->codigo, proximo_unary_node->codigo);
        char* var_operando_dir = obter_valor_rhs(proximo_unary_node);

        char* var_novo_resultado = gerar_variavel_temporaria(resolvedor_global);
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "%s = %s %s %s", var_novo_resultado, var_resultado_esq,
                 op_node->token->lexema, var_operando_dir);
        adicionar_string(codigo_operacoes, buffer);

        no_resultado_esq = criar_no_expressao_binario(
            op_node->token->lexema[0], "N", no_resultado_esq, proximo_unary_node->sdt_mat.no);
        proximo_unary_node->sdt_mat.no = NULL;

        free(var_resultado_esq);
        var_resultado_esq = var_novo_resultado;
        free(var_operando_dir);

        aux_node = aux_node->filhos[2];
    }

    lista_codigo_adicionar_lista(term_node->codigo, codigo_operacoes);
    liberar_lista_string(codigo_operacoes);

    term_node->sdt_mat.no = no_resultado_esq;
    if (term_node->res_var_codigo.var) free(term_node->res_var_codigo.var);
    term_node->res_var_codigo.var = var_resultado_esq;
}

void AEXP_imprimir_expressao0(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    (void)gerenciador;
    // REGRA: EXPRESSION -> NUMEXPRESSION EXPRESSION'
    NoAST* expression_node = no_pai;
    NoAST* numexpression_node = no_pai->filhos[0];
    NoAST* expression_linha_node = no_pai->filhos[1];

    // A. Junta os códigos gerados pelos filhos na ordem correta
    lista_codigo_adicionar_lista(expression_node->codigo, numexpression_node->codigo);
    lista_codigo_adicionar_lista(expression_node->codigo, expression_linha_node->codigo);

    // B. Pega o resultado final, que foi SINTETIZADO por EXPRESSION', e o define
    // como o resultado final de EXPRESSION.
    if (expression_linha_node->res_var_codigo.var)
    {
        if (expression_node->res_var_codigo.var)
        {
            free(expression_node->res_var_codigo.var);
        }
        // O resultado final (ex: "$1") é propagado para o nó pai.
        expression_node->res_var_codigo.var = strdup(expression_linha_node->res_var_codigo.var);
    }
    else
    {
        // Caso não haja operação relacional (ex: x = 5;), o resultado
        // é simplesmente o do numexpression.
        if (expression_node->res_var_codigo.var)
        {
            free(expression_node->res_var_codigo.var);
        }
        expression_node->res_var_codigo.var = strdup(numexpression_node->res_var_codigo.var);
    }

    // Mantém a depuração
    imprimir_arvore(numexpression_node->sdt_mat.no);
}

void AEXP_imprimir_expressao0_h(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    (void)gerenciador;
    // 'no_pai' aqui é o nó da regra inteira (EXPRESSION)
    NoAST* numexpression_node = no_pai->filhos[0];     // O filho da esquerda (NUMEXPRESSION)
    NoAST* expression_linha_node = no_pai->filhos[1];  // O filho da direita (EXPRESSION')

    if (numexpression_node->res_var_codigo.var)
    {
        // Libera a memória do ponteiro antigo, se houver, para evitar vazamento de memória.
        if (expression_linha_node->res_var_codigo.var)
        {
            free(expression_linha_node->res_var_codigo.var);
        }

        // Copia a variável de resultado do nó da esquerda para o nó da direita.
        // Ex: Passa o nome "a" para o nó que vai processar a operação "< 4".
        expression_linha_node->res_var_codigo.var = strdup(numexpression_node->res_var_codigo.var);
    }
}

void AEXP_passar_numero(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    (void)gerenciador;
    // FACTOR -> OPEN_PARENTHESIS NUMEXPRESSION CLOSE_PARENTHESIS
    NoAST* factor_node = no_pai;
    NoAST* numexpression_node = no_pai->filhos[1];

    if (factor_node->res_var_codigo.var) free(factor_node->res_var_codigo.var);
    factor_node->res_var_codigo.var = strdup(numexpression_node->res_var_codigo.var);

    lista_codigo_adicionar_lista(factor_node->codigo, numexpression_node->codigo);

    factor_node->sdt_mat.no = numexpression_node->sdt_mat.no;
    numexpression_node->sdt_mat.no = NULL;
}

void AEXP_imprimir_expressao1(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    (void)gerenciador;
    // EXPRESSION' -> RELOP NUMEXPRESSION
    NoAST* numexpression_node = no_pai->filhos[1];
    imprimir_arvore(numexpression_node->sdt_mat.no);
}

void AEXP_imprimir_expressao2(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    (void)gerenciador;
    // ALLOCEXPRESSION' -> TYPE OPEN_BRACKET NUMEXPRESSION CLOSE_BRACKET ALLOCAUX
    NoAST* numexpression_node = no_pai->filhos[2];
    imprimir_arvore(numexpression_node->sdt_mat.no);
}

void AEXP_coletar_indice(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    (void)gerenciador;
    // A regra é: ALLOCAUX -> [ NUMEXPRESSION ] ALLOCAUX'
    NoAST* numexpression = no_pai->filhos[1];
    NoAST* allocaux_filho = no_pai->filhos[3];

    // 1. Herda o código dos filhos
    lista_codigo_adicionar_lista(no_pai->codigo, numexpression->codigo);
    lista_codigo_adicionar_lista(no_pai->codigo, allocaux_filho->codigo);

    // 2. Coleta a variável do índice da expressão (ex: '1', '2', ou uma variável como '$t0')
    if (numexpression->res_var_codigo.var)
    {
        adicionar_string(no_pai->sdt_mat.vetor_array_var, numexpression->res_var_codigo.var);
    }

    // 3. Junta a lista de índices do filho (para dimensões aninhadas)
    lista_codigo_adicionar_lista(no_pai->sdt_mat.vetor_array_var,
                                 allocaux_filho->sdt_mat.vetor_array_var);

    // 4. Incrementa o contador de dimensões
    no_pai->sdt_mat.contador_vetor = 1 + allocaux_filho->sdt_mat.contador_vetor;
}

void AEXP_inicializar_contador_vetor(NoAST* no_pai, GerenciadorEscopo* gerenciador)
{
    (void)gerenciador;
    no_pai->sdt_mat.contador_vetor = 0;
    no_pai->res_var_codigo.var = strdup("0");  // Índice padrão para variáveis escalares
}
