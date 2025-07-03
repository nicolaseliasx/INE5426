#include <stdio.h>   // Para operações de ficheiro (fopen, fclose) e I/O (printf, fprintf)
#include <stdlib.h>  // Para exit, malloc, free
#include <string.h>  // Para strcmp, strlen

#include "acoes_semanticas.h"
#include "analisador_lexico.h"
#include "analisador_sintatico.h"
#include "debug.h"
#include "erros.h"
#include "gerenciador_escopo.h"
#include "no_ast.h"
#include "tabela_analise.h"
#include "token.h"

// Função auxiliar para imprimir o código intermediário com mais depuração
void print_intermediate_code(ListaString* code_list)
{
    DEBUG_PRINT("[DEBUG] Tentando imprimir código intermediário...\n");
    if (!code_list)
    {
        DEBUG_PRINT("[DEBUG] A lista de código é NULA. Nada a imprimir.\n");
        DEBUG_PRINT("[DEBUG] A lista de código é NULA. Nada a imprimir.\n");
        return;
    }
    if (code_list->tamanho == 0)
    {
        DEBUG_PRINT("[DEBUG] A lista de código está VAZIA. Nada a imprimir.\n");
        return;
    }
    DEBUG_PRINT("[DEBUG] Imprimindo %d linhas de código.\n", code_list->tamanho);
    for (int i = 0; i < code_list->tamanho; ++i)
    {
        printf("%s\n", code_list->itens[i]);
    }
    DEBUG_PRINT("[DEBUG] Impressão do código intermediário concluída.\n");
}

void print_fire()
{
    // ... (código da arte ASCII permanece o mesmo) ...
    printf("⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n");
    printf(
        "                        ⣦⣀⠀⠀⠀⠀⠀ ⢲⣄⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀  "
        "⣠⡆⠀⠀⠀⠀⠀⠀⠀⠛⣦⣄⣀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n");
    printf(
        "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠙⣿⣷⣤⠀⠀⠀⠀⠀⢻⣿⣷⣄⢀⠀⠀⠀⠀⠀⠀⢀⣴⣿⡟⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⢻⣿⣷⣦⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀"
        "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n");
    printf(
        "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠘⣿⣿⣷⣄⠀⠀⠀⠀⣿⣿⣿⣷⠱⣆⠀⠀⠀⢀⣾⣿⣿⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣿⣿⣿⣿⣆⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀"
        "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n");
    printf(
        "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢹⣿⣽⣿⡆⠀⠀⠀⢸⣿⣞⣿⣧⢸⣷⣤⠀⢸⣿⣯⣿⠆⠀⠀⠀⠀⠀⠀⠀⠀⢀⣴⡟⢰⣿⡷⣿⣿⡄⠀⠀⠀⠀⠀⠀⠀⠀⣿⣦⡄⠀⠀⠀⠀⢻⣿⣶⣄⠀⠀⠀⠀⠀⠀"
        "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n");
    printf(
        "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⣟⣾⣿⠀⠀⠀⣼⣿⡏⣿⣿⠀⣿⣿⣗⠺⣿⣳⣿⣧⠀⠀⠀⠀⠀⠀⠀⣴⣿⡟⠀⣸⣿⡟⣽⣿⠇⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⣿⣆⠀⠀⠀⠀⣿⣿⣿⣦⠀⠀⠀⠀⠀"
        "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n");
    printf(
        "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠸⣦⠀⠀⠀⠀⠀⠀⠀⠀⣼⣿⣯⢿⣿⡇⠀⣰⣿⡿⢸⣿⡿⠀⣼⣿⣻⡦⣿⣯⢿⣿⡆⠀⠀⠀⠀⢀⣾⣿⣿⠀⢠⣿⡿⢱⣿⡿⠀⠀⠀⠀⠀⠀⠀⠀⠀⣼⣿⢯⣿⠀⠀⢠⠀⣼⣿⣻⣿⠀⠀⠀⠀⠀"
        "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n");
    printf(
        "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢻⣷⡀⠀⠀⠀⠀⠀⣰⣿⣿⣞⣿⣿⠃⢀⣿⡿⣡⣿⡿⠃⢀⣿⣿⣽⣷⢹⣿⣻⢿⣿⡄⠀⠀⢀⣾⣿⢿⡇⠀⣾⣿⣱⣿⣿⠃⠀⠀⠀⠀⠀⠀⠀⠀⣰⣿⣿⣿⣿⠁⢀⡿⢰⣿⣣⣿⠇⠀⠀⠀⠀⠀"
        "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n");
    printf(
        "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⣿⣿⡄⠀⠀⠀⢰⣿⣿⣻⣼⣿⡿⠀⢸⣿⢣⣿⡿⠁⣠⣿⣿⡟⣾⣿⢈⣿⣯⣟⣿⣷⠀⠀⣸⣿⣟⣿⡇⠀⣿⣧⣿⣿⠃⠀⠀⠀⠀⠀⠀⠀⠀⣰⣿⣿⣳⣿⡟⠀⣾⡇⢸⣷⣿⠏⠀⠀⠀⠀⠀⠀"
        "⠀⠀⠀⡀⠀⠀⠀⠀⠀⠀\n");
    printf(
        "⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⠀⠀⠀⠀⠀⠀⠀⠀⢻⣿⣿⡄⠀⢠⣿⣿⢯⣳⣿⣿⣦⣄⠘⣿⣿⡿⠁⣴⣿⣿⢯⣽⣿⡟⢀⣿⣷⢯⣿⣿⡇⠀⣿⣿⣽⣻⣿⠀⣿⣷⣿⠇⠀⠀⠀⠀⣀⠀⠀⢀⣼⣿⣿⣳⢿⣿⠁⣰⣿⣇⢸⣿⡟⠀⠀⠀⠀⠀⠀⠀"
        "⠀⣠⡾⠁⠀⠀⠀⠀⠀⠀\n");
    printf(
        "⠀⠀⠀⠀⠀⠀⠀⢠⣾⠇⠀⠀⠀⠀⠀⠀⠀⠀⣸⣿⢿⣷⠀⣿⣿⢯⣟⣼⣿⡇⢻⣿⣧⠘⣿⠃⣼⣿⡿⡽⣞⣿⣿⠁⢸⣿⣟⣮⢿⣿⡇⠀⣿⣿⢶⣻⣿⣇⠘⣿⣿⠀⠀⢀⣴⡿⠁⠀⣠⣿⢱⣿⣯⣽⣻⣿⠀⣿⡿⣿⡄⢻⣧⠀⠀⠀⠀⠀⠀⣴"
        "⣾⡟⠁⠀⠀⠀⠀⠀⠀⠀\n");
    printf(
        "⠀⠀⠀⠀⠀⠀⣰⣿⣿⠀⠀⠀⠀⠀⠀⠀⠀⢀⣿⣿⢿⡇⢰⣿⣯⡟⣼⣿⣿⠁⠘⣿⣽⣧⠈⢰⣿⣯⡷⣛⣿⣿⠃⠀⣼⣿⣿⢼⣻⣿⡇⠀⢺⣿⡿⣼⣻⣿⣦⠘⠇⠀⣠⣿⡿⠁⠀⣰⣿⡟⢸⣿⣳⢾⡽⣿⣇⢸⣿⡿⣿⣆⠙⠀⠀⠀⠀⢀⣾⣿"
        "⣿⠁⠀⠀⠀⠀⠀⠀⠀⠀\n");
    printf(
        "⠀⠀⠀⠀⠀⢀⣿⣿⣿⠀⠀⠀⠀⠀⠀⠀⠀⣼⣿⡿⣿⠃⣿⣟⣾⢡⡿⣿⡟⠀⠀⢿⣯⢿⡇⣼⣿⣞⡇⣿⣿⣯⠀⣼⣿⣿⠏⣾⣿⣟⣰⡇⠘⣿⣿⣳⣭⢿⣿⣧⠀⢠⣿⣿⡃⠀⢰⣿⡿⡇⠸⣿⣯⡇⢻⣿⣿⣯⣿⣿⣿⣿⣷⡀⠀⠀⢀⣿⣿⣻"
        "⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀\n");
    printf(
        "⠀⠀⠀⠀⠀⠐⣿⣯⣿⣇⠱⣆⡀⠀⠀⠀⣸⣿⡿⣽⡿⢸⣿⢯⡇⢸⣟⣯⣟⠀⠀⣻⣿⣻⣿⢼⣷⣻⠄⢸⣿⣿⣿⣿⡿⠏⣸⣿⣿⣷⣿⡅⠀⢸⣿⣷⣏⣾⣻⣿⡆⢸⣿⢿⡅⠀⣾⣿⣻⣿⠐⣿⣿⣻⡄⠻⣽⣿⣯⣿⣷⣻⢿⣿⡆⠀⢸⣿⣟⣿"
        "⣇⠀⠀⠀⠀⠀⠀⠀⠀⠀\n");
    printf(
        "⠀⠀⠈⣧⡀⠀⢻⣿⣾⣿⣆⠹⣿⣆⠀⢠⣿⣿⡽⣿⡇⢸⣿⢯⡇⢸⣯⢿⣿⡀⣠⣿⣟⡷⣿⢸⣿⡽⣇⠀⡹⠾⠟⠋⢀⣾⣿⣿⣻⣿⢿⡆⠀⢈⣿⣿⢾⡘⣷⣻⣷⣸⣿⢿⣧⠀⣿⣿⣽⣿⡆⠘⣿⣿⣽⡀⢹⡾⣿⣯⢻⣿⣯⢿⣿⡄⢸⣿⣯⢿"
        "⣿⣆⠀⠀⠀⠀⠀⠀⠀⠀\n");
    printf(
        "⠀⠀⠀⣿⣷⡀⠸⣿⣷⣻⣿⡀⣿⣿⡧⢼⣿⢷⣻⣿⡇⢹⣿⣟⡆⠈⣿⣻⢿⣷⣿⡿⣭⣿⡟⠘⣿⣟⣯⠀⠐⡀⢂⢠⣿⣿⣿⠍⣿⣿⢿⣿⣦⣼⣿⣟⣾⠇⢹⣯⢿⡏⣿⡿⣿⡀⢺⣿⣞⣿⣿⣦⣹⣿⣯⠇⠀⣿⣻⣿⠀⣿⣯⣟⣿⣷⠈⣿⣿⢯"
        "⡿⣿⣧⡀⠀⠀⠀⢀⣀⣤\n");
    printf(
        "⠀⠀⠀⣿⣿⣇⠀⣿⣷⣻⣿⡇⢹⣯⣿⣼⣿⡇⣿⣿⡇⠘⣿⣯⡗⠀⢜⢯⡿⡽⣯⢷⣯⣿⡷⢨⣿⣟⣾⠀⡼⠁⡌⣾⣿⣿⡏⠀⣿⣿⢯⡿⣿⣟⣿⡽⣾⠀⠐⣯⣿⣟⢿⣿⢿⣷⡜⣿⣯⠺⣟⡿⣿⣟⡿⡀⠀⡿⣽⣿⠀⣿⣿⢞⣿⡟⣲⡿⢻⣿"
        "⣳⢟⣿⣿⡄⠀⣴⣿⣿⠃\n");
    printf(
        "⠀⠀⣼⣿⣯⡇⣼⣿⢷⣻⣿⠀⣽⡿⣽⣿⣿⣽⢸⣿⣿⣤⣿⣯⡟⢧⠈⢢⡙⡿⠁⣾⢿⣿⡇⢸⣿⣟⣾⠰⡇⣸⠱⣟⣿⣷⡀⠀⣿⣿⣳⢻⡽⣿⡳⣽⢣⠇⢈⡷⣿⣏⠸⣿⣯⢿⣿⡹⣿⣷⡈⠙⠳⠟⠐⠀⠀⣿⣿⣇⣼⣿⣯⢿⣿⣷⣿⡇⠘⣿"
        "⣯⡛⣾⣿⣟⢰⣿⢿⡟⠀\n");
    printf(
        "⠀⣼⣿⣿⣿⣽⣿⡿⣫⣿⡇⢰⣿⣟⣿⡏⣿⣿⣆⠹⢿⡿⣟⣷⡻⢸⡄⠀⢻⠃⠀⣿⣿⣿⣀⣾⣿⣽⡎⢸⠁⡏⠀⣿⣻⣿⣿⣾⣿⡿⣽⠃⢻⡷⣽⢋⡎⠀⣴⡿⣿⡿⢀⣿⣟⢺⣿⣧⠹⣿⣿⣄⠀⠙⡆⠀⠀⣿⣿⣿⣿⡟⣾⣿⣿⣿⡽⣇⠀⣹"
        "⣿⣽⠸⣿⣿⢨⣿⣿⣷⠀\n");
    printf(
        "⣼⣿⣟⣿⣿⣿⣿⢃⣿⣿⣷⣿⣿⢞⣿⡇⢸⣿⣽⡄⠠⡙⢿⣯⡗⢨⠀⠀⡘⠰⡰⠘⣿⢿⣿⢿⣻⡞⠁⡾⠀⢱⡐⠈⠷⣯⣟⣯⣟⡽⢏⠀⢨⡿⢁⡞⠀⢰⣿⣿⣿⠃⢈⣿⣿⡃⣿⣿⠄⢻⣿⣽⠀⠀⢧⡀⡆⢹⣾⡽⠃⣾⣿⡿⢸⣿⣽⣿⣦⣿"
        "⣿⡏⢘⣷⣿⢸⣿⢿⣿⡄\n");
    printf(
        "⣿⣿⢽⣿⣿⣿⢾⠀⢿⣻⢿⣯⠏⣾⣿⡇⠘⣿⣯⢿⠀⠙⣆⠹⠇⣤⠇⡄⡃⠀⡇⠀⢈⠙⠙⢋⡁⠀⠠⢹⠀⣄⠑⡌⠀⡀⠉⡈⢀⣰⡾⠀⣼⣣⠋⠇⡀⢺⣯⣿⡟⠀⢀⣿⣟⡇⣿⣿⡇⢸⣿⣾⠁⡆⢸⡇⢸⠈⡻⠀⢸⣟⣿⡇⠸⣿⣷⠻⣿⣿"
        "⠟⠀⣸⣿⣿⣾⣿⢯⣿⡧\n");
    printf(
        "⣿⣿⢸⣿⣿⣿⣻⡇⠈⠻⣿⠁⠨⡷⣿⣷⣤⣽⣿⣻⠅⠀⠘⡆⣸⠏⡰⠀⡇⡄⠘⡄⠂⠈⢀⠀⠲⣀⠀⣽⠀⡈⡦⡈⠢⢑⠀⣴⠞⢁⡠⢀⡽⠁⢸⠀⠀⢹⣿⣿⡇⢀⣾⣿⣿⠁⢿⣽⣿⣿⣿⠟⢠⠃⡘⢧⠈⢫⠀⠀⠸⣯⣿⣷⣤⣿⣿⡇⠘⣋"
        "⠆⢠⣿⣿⣿⣿⠏⣾⣿⡇\n-");
    printf(
        "⠹⣿⣧⠻⣿⢿⣷⣻⣄⠘⣄⠀⢀⠿⣽⣻⢿⡿⣯⠟⠀⠀⠀⡼⠃⣴⠇⣸⠁⢸⢂⠘⢔⠀⡀⠀⡄⠻⣄⠘⣇⢡⠀⡉⠢⡀⠉⠁⡔⠋⠁⠨⠁⢀⢾⡀⠆⠘⣷⢿⣿⣿⣿⡿⠃⠀⢊⢉⡛⠋⠁⡠⠃⢠⠃⡘⠠⡀⢷⡀⡀⠻⢽⣿⣿⣿⠟⠀⠐⠁"
        "⣠⠿⠿⠛⠋⢁⣰⣿⣿⠁\n");
    printf(
        "⠀⠹⣿⣷⣌⡙⠛⠽⠷⢧⡈⠢⣄⠲⢍⡛⠳⠛⠁⢊⣠⠞⠋⣰⡟⢁⣴⠇⢀⠂⣦⣉⠢⢄⡑⠄⠈⣶⢄⣁⠙⠤⡂⠹⢦⣈⠓⠈⢠⡶⣼⡄⡶⣹⣧⣳⡘⠰⢌⡛⠚⢓⢫⠴⠁⠀⠈⠁⠀⢐⡭⠔⢒⡇⢠⣇⢠⣆⠈⣷⣈⠢⢄⡠⢉⠀⠀⣀⠄⠘"
        "⠁⠀⠀⣠⣴⡿⣿⡿⠋⠀\n-");
    printf(
        "⠀⠀⠈⠙⠿⢽⣷⣶⣤⣤⣌⣦⣈⣳⣶⣤⣤⣴⣠⣭⣴⣶⣛⣧⣴⣾⣩⣴⣾⣧⣝⣯⣷⣶⣭⣗⣤⣈⣛⣶⣭⣝⣃⣂⣀⣉⣻⣦⣔⣿⣮⣅⣁⣻⣾⣽⣻⣧⣤⣥⣤⣠⣄⣤⣠⣤⣤⣴⣶⣯⣤⣶⣯⣴⣟⣿⣮⣟⣷⣮⣟⣿⣶⣶⣖⣶⣾⣥⣖⣶"
        "⣲⣮⣷⠿⠞⠋⠁⠀⠀⠀\n");
    printf(
        "⠀⠀⠀⠀⠀⠀⠀⠀⠉⠉⠁⠀⠁⠀⠀⠀⠀⠀⠁⠀⠀⠀⠁⠀⠀⠀⠁⠀⠀⠀⠈⠀⠀⠀⠀⠀⠀⠁⠈⠀⠀⠀⠁⠈⠀⠁⠀⠀⠀⠀⠀⠈⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀"
        "⠀⠀\n");
    printf(
        "  INE5420 - Compiladores - Prof. Alvaro Junio Pereira Franco - GRUPO: Nicolas, Patricia, "
        "Gustavo\n");
}

int main(int argc, char* argv[])
{
    DEBUG_PRINT("[DEBUG] Iniciando o compilador...\n");
    print_fire();
    inicializar_resolvedor_global();
    DEBUG_PRINT("[DEBUG] Componentes inicializados com sucesso.\n\n");

    if (argc < 2)
    {
        fprintf(stderr, "Uso: %s <arquivo>\n", argv[0]);
        return 1;
    }

    const char* filename = argv[1];
    DEBUG_PRINT("[DEBUG] Arquivo de entrada: %s\n", filename);

    FILE* file_ptr = fopen(filename, "r");
    if (!file_ptr)
    {
        LANCAR_ERRO_LEXICO("Não foi possível abrir o arquivo");
    }
    DEBUG_PRINT("[DEBUG] Arquivo aberto com sucesso.\n");

    // KEYWORDS - ORDENADAS
    char* keywords_c_arr[] = {"break", "call", "def",   "else", "float",  "for",   "if",
                              "int",   "new",  "print", "read", "return", "string"};
    int num_keywords = sizeof(keywords_c_arr) / sizeof(keywords_c_arr[0]);

    GerenciadorEscopo* scope_manager = NULL;
    AnalisadorLexico* lex_analyzer = NULL;
    AnalisadorSintatico* syntax_analyzer = NULL;

    DEBUG_PRINT("[DEBUG] Criando componentes...\n");

    scope_manager = criar_gerenciador_escopo();
    if (!scope_manager)
    {
        fprintf(stderr, "Erro: Falha ao criar o gerenciador de escopo.\n");
        fclose(file_ptr);
        return 1;
    }
    DEBUG_PRINT("[DEBUG] Gerenciador de Escopo criado.\n");

    lex_analyzer = criar_analisador_lexico(file_ptr, keywords_c_arr, num_keywords);
    if (!lex_analyzer)
    {
        fprintf(stderr, "Erro: Falha ao inicializar o analisador léxico.\n");
        liberar_gerenciador_escopo(scope_manager);
        fclose(file_ptr);
        return 1;
    }
    DEBUG_PRINT("[DEBUG] Analisador Léxico criado.\n");

    syntax_analyzer =
        criar_analisador_sintatico(scope_manager, tabela_analise, num_entradas_tabela);
    if (!syntax_analyzer)
    {
        fprintf(stderr, "Erro: Falha ao inicializar o analisador sintático.\n");
        destruir_analisador_lexico(lex_analyzer);
        liberar_gerenciador_escopo(scope_manager);
        fclose(file_ptr);
        return 1;
    }
    DEBUG_PRINT("[DEBUG] Analisador Sintático criado.\n");
    DEBUG_PRINT("[DEBUG] Componentes inicializados com sucesso.\n\n");

    Token* current_token = NULL;
    DEBUG_PRINT("[DEBUG] >>> Iniciando loop principal de análise de tokens <<<\n");
    while ((current_token = proximo_token(lex_analyzer)) != NULL)
    {
        DEBUG_PRINT("[DEBUG] Token Lido: ID='%s', Lexema='%s', Linha=%d, Coluna=%d\n",
                    current_token->id, current_token->lexema, current_token->linha,
                    current_token->coluna);

        analisar_token(syntax_analyzer, current_token);

        DEBUG_PRINT("[DEBUG] Token processado pelo analisador sintático.\n");
    }
    DEBUG_PRINT("[DEBUG] >>> Fim do loop de análise de tokens (fim de arquivo atingido) <<<\n\n");

    DEBUG_PRINT("[DEBUG] Processando token de Fim de Arquivo (EOF).\n");
    Token* eof_token = criar_token("$", "$", lex_analyzer->linha, 0);
    if (!eof_token)
    {
        fprintf(stderr, "Erro: Falha ao criar o token EOF.\n");
        liberar_analisador_sintatico(syntax_analyzer);
        destruir_analisador_lexico(lex_analyzer);
        liberar_gerenciador_escopo(scope_manager);
        fclose(file_ptr);
        return 1;
    }
    analisar_token(syntax_analyzer, eof_token);
    DEBUG_PRINT("[DEBUG] Token EOF processado.\n");
    liberar_token(eof_token);

    DEBUG_PRINT("[DEBUG] Verificando se a análise sintática foi concluída com sucesso...\n");
    if (!analise_completa(syntax_analyzer))
    {
        DEBUG_PRINT("[DEBUG] ERRO: Pilha do analisador não está vazia. Análise incompleta.\n");
        LANCAR_ERRO_SINTATICO("Fim de arquivo inesperado ou análise sintática incompleta.");
    }
    DEBUG_PRINT("[DEBUG] Análise sintática concluída com sucesso.\n");

    printf("\n************************************************************\n\n");
    printf("Processamento concluído com sucesso!\n");
    printf("Todas as expressões foram validadas\n");
    printf("Todas as declarações de variáveis estão corretas\n");
    printf("\n************************************************************\n\n");

    printf("Tabelas de Símbolos Geradas:\n\n");
    imprimir_tabelas_gerenciador(scope_manager, stdout);

    printf("\n************************************************************\n\n");
    printf("Código Intermediário Gerado:\n\n");
    if (syntax_analyzer->raiz_ast && syntax_analyzer->raiz_ast->codigo)
    {
        print_intermediate_code(syntax_analyzer->raiz_ast->codigo);
    }
    else
    {
        printf("Nenhum código intermediário foi gerado ou a raiz da AST é inválida.\n");
    }

    printf("\n************************************************************\n\n");

    DEBUG_PRINT("[DEBUG] Iniciando liberação de todos os recursos...\n");
    liberar_analisador_sintatico(syntax_analyzer);
    DEBUG_PRINT("[DEBUG] Analisador sintático liberado.\n");
    destruir_analisador_lexico(lex_analyzer);
    DEBUG_PRINT("[DEBUG] Analisador léxico liberado.\n");
    liberar_gerenciador_escopo(scope_manager);
    DEBUG_PRINT("[DEBUG] Gerenciador de escopo liberado.\n");

    fclose(file_ptr);
    DEBUG_PRINT("[DEBUG] Arquivo fechado.\n");

    return 0;
}