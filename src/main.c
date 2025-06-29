#include <stdio.h>    // Para operações de ficheiro (fopen, fclose) e I/O (printf, fprintf)
#include <string.h>   // Para strcmp, strlen
#include <stdlib.h>   // Para exit, malloc, free

// Incluir os cabeçalhos C do seu projeto
#include "acoes_semanticas.h"
#include "analisador_lexico.h"
#include "analisador_sintatico.h"
#include "erros.h"
#include "gerenciador_escopo.h"
#include "no_ast.h"
#include "tabela_analise.h"
#include "token.h"

void print_intermediate_code(ListaString* code_list) {
    if (!code_list) {
        return;
    }
    for (int i = 0; i < code_list->tamanho; ++i) {
        printf("%s\n", code_list->itens[i]);
    }
}

void print_fire() {
    printf("⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n");
    printf("                        ⣦⣀⠀⠀⠀⠀⠀ ⢲⣄⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀  ⣠⡆⠀⠀⠀⠀⠀⠀⠀⠛⣦⣄⣀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n");
    printf("⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠙⣿⣷⣤⠀⠀⠀⠀⠀⢻⣿⣷⣄⢀⠀⠀⠀⠀⠀⠀⢀⣴⣿⡟⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⢻⣿⣷⣦⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n");
    printf("⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠘⣿⣿⣷⣄⠀⠀⠀⠀⣿⣿⣿⣷⠱⣆⠀⠀⠀⢀⣾⣿⣿⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣿⣿⣿⣿⣆⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n");
    printf("⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢹⣿⣽⣿⡆⠀⠀⠀⢸⣿⣞⣿⣧⢸⣷⣤⠀⢸⣿⣯⣿⠆⠀⠀⠀⠀⠀⠀⠀⠀⢀⣴⡟⢰⣿⡷⣿⣿⡄⠀⠀⠀⠀⠀⠀⠀⠀⣿⣦⡄⠀⠀⠀⠀⢻⣿⣶⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n");
    printf("⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⣟⣾⣿⠀⠀⠀⣼⣿⡏⣿⣿⠀⣿⣿⣗⠺⣿⣳⣿⣧⠀⠀⠀⠀⠀⠀⠀⣴⣿⡟⠀⣸⣿⡟⣽⣿⠇⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⣿⣆⠀⠀⠀⠀⣿⣿⣿⣦⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n");
    printf("⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠸⣦⠀⠀⠀⠀⠀⠀⠀⠀⣼⣿⣯⢿⣿⡇⠀⣰⣿⡿⢸⣿⡿⠀⣼⣿⣻⡦⣿⣯⢿⣿⡆⠀⠀⠀⠀⢀⣾⣿⣿⠀⢠⣿⡿⢱⣿⡿⠀⠀⠀⠀⠀⠀⠀⠀⠀⣼⣿⢯⣿⠀⠀⢠⠀⣼⣿⣻⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n");
    printf("⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢻⣷⡀⠀⠀⠀⠀⠀⣰⣿⣿⣞⣿⣿⠃⢀⣿⡿⣡⣿⡿⠃⢀⣿⣿⣽⣷⢹⣿⣻⢿⣿⡄⠀⠀⢀⣾⣿⢿⡇⠀⣾⣿⣱⣿⣿⠃⠀⠀⠀⠀⠀⠀⠀⠀⣰⣿⣿⣿⣿⠁⢀⡿⢰⣿⣣⣿⠇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n");
    printf("⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⣿⣿⡄⠀⠀⠀⢰⣿⣿⣻⣼⣿⡿⠀⢸⣿⢣⣿⡿⠁⣠⣿⣿⡟⣾⣿⢈⣿⣯⣟⣿⣷⠀⠀⣸⣿⣟⣿⡇⠀⣿⣧⣿⣿⠃⠀⠀⠀⠀⠀⠀⠀⠀⣰⣿⣿⣳⣿⡟⠀⣾⡇⢸⣷⣿⠏⠀⠀⠀⠀⠀⠀⠀⠀⠀⡀⠀⠀⠀⠀⠀⠀\n");
    printf("⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⠀⠀⠀⠀⠀⠀⠀⠀⢻⣿⣿⡄⠀⢠⣿⣿⢯⣳⣿⣿⣦⣄⠘⣿⣿⡿⠁⣴⣿⣿⢯⣽⣿⡟⢀⣿⣷⢯⣿⣿⡇⠀⣿⣿⣽⣻⣿⠀⣿⣷⣿⠇⠀⠀⠀⠀⣀⠀⠀⢀⣼⣿⣿⣳⢿⣿⠁⣰⣿⣇⢸⣿⡟⠀⠀⠀⠀⠀⠀⠀⠀⣠⡾⠁⠀⠀⠀⠀⠀⠀\n");
    printf("⠀⠀⠀⠀⠀⠀⠀⢠⣾⠇⠀⠀⠀⠀⠀⠀⠀⠀⣸⣿⢿⣷⠀⣿⣿⢯⣟⣼⣿⡇⢻⣿⣧⠘⣿⠃⣼⣿⡿⡽⣞⣿⣿⠁⢸⣿⣟⣮⢿⣿⡇⠀⣿⣿⢶⣻⣿⣇⠘⣿⣿⠀⠀⢀⣴⡿⠁⠀⣠⣿⢱⣿⣯⣽⣻⣿⠀⣿⡿⣿⡄⢻⣧⠀⠀⠀⠀⠀⠀⣴⣾⡟⠁⠀⠀⠀⠀⠀⠀⠀\n");
    printf("⠀⠀⠀⠀⠀⠀⣰⣿⣿⠀⠀⠀⠀⠀⠀⠀⠀⢀⣿⣿⢿⡇⢰⣿⣯⡟⣼⣿⣿⠁⠘⣿⣽⣧⠈⢰⣿⣯⡷⣛⣿⣿⠃⠀⣼⣿⣿⢼⣻⣿⡇⠀⢺⣿⡿⣼⣻⣿⣦⠘⠇⠀⣠⣿⡿⠁⠀⣰⣿⡟⢸⣿⣳⢾⡽⣿⣇⢸⣿⡿⣿⣆⠙⠀⠀⠀⠀⢀⣾⣿⣿⠁⠀⠀⠀⠀⠀⠀⠀⠀\n");
    printf("⠀⠀⠀⠀⠀⢀⣿⣿⣿⠀⠀⠀⠀⠀⠀⠀⠀⣼⣿⡿⣿⠃⣿⣟⣾⢡⡿⣿⡟⠀⠀⢿⣯⢿⡇⣼⣿⣞⡇⣿⣿⣯⠀⣼⣿⣿⠏⣾⣿⣟⣰⡇⠘⣿⣿⣳⣭⢿⣿⣧⠀⢠⣿⣿⡃⠀⢰⣿⡿⡇⠸⣿⣯⡇⢻⣿⣿⣯⣿⣿⣿⣿⣷⡀⠀⠀⢀⣿⣿⣻⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀\n");
    printf("⠀⠀⠀⠀⠀⠐⣿⣯⣿⣇⠱⣆⡀⠀⠀⠀⣸⣿⡿⣽⡿⢸⣿⢯⡇⢸⣟⣯⣟⠀⠀⣻⣿⣻⣿⢼⣷⣻⠄⢸⣿⣿⣿⣿⡿⠏⣸⣿⣿⣷⣿⡅⠀⢸⣿⣷⣏⣾⣻⣿⡆⢸⣿⢿⡅⠀⣾⣿⣻⣿⠐⣿⣿⣻⡄⠻⣽⣿⣯⣿⣷⣻⢿⣿⡆⠀⢸⣿⣟⣿⣇⠀⠀⠀⠀⠀⠀⠀⠀⠀\n");
    printf("⠀⠀⠈⣧⡀⠀⢻⣿⣾⣿⣆⠹⣿⣆⠀⢠⣿⣿⡽⣿⡇⢸⣿⢯⡇⢸⣯⢿⣿⡀⣠⣿⣟⡷⣿⢸⣿⡽⣇⠀⡹⠾⠟⠋⢀⣾⣿⣿⣻⣿⢿⡆⠀⢈⣿⣿⢾⡘⣷⣻⣷⣸⣿⢿⣧⠀⣿⣿⣽⣿⡆⠘⣿⣿⣽⡀⢹⡾⣿⣯⢻⣿⣯⢿⣿⡄⢸⣿⣯⢿⣿⣆⠀⠀⠀⠀⠀⠀⠀⠀\n");
    printf("⠀⠀⠀⣿⣷⡀⠸⣿⣷⣻⣿⡀⣿⣿⡧⢼⣿⢷⣻⣿⡇⢹⣿⣟⡆⠈⣿⣻⢿⣷⣿⡿⣭⣿⡟⠘⣿⣟⣯⠀⠐⡀⢂⢠⣿⣿⣿⠍⣿⣿⢿⣿⣦⣼⣿⣟⣾⠇⢹⣯⢿⡏⣿⡿⣿⡀⢺⣿⣞⣿⣿⣦⣹⣿⣯⠇⠀⣿⣻⣿⠀⣿⣯⣟⣿⣷⠈⣿⣿⢯⡿⣿⣧⡀⠀⠀⠀⢀⣀⣤\n");
    printf("⠀⠀⠀⣿⣿⣇⠀⣿⣷⣻⣿⡇⢹⣯⣿⣼⣿⡇⣿⣿⡇⠘⣿⣯⡗⠀⢜⢯⡿⡽⣯⢷⣯⣿⡷⢨⣿⣟⣾⠀⡼⠁⡌⣾⣿⣿⡏⠀⣿⣿⢯⡿⣿⣟⣿⡽⣾⠀⠐⣯⣿⣟⢿⣿⢿⣷⡜⣿⣯⠺⣟⡿⣿⣟⡿⡀⠀⡿⣽⣿⠀⣿⣿⢞⣿⡟⣲⡿⢻⣿⣳⢟⣿⣿⡄⠀⣴⣿⣿⠃\n");
    printf("⠀⠀⣼⣿⣯⡇⣼⣿⢷⣻⣿⠀⣽⡿⣽⣿⣿⣽⢸⣿⣿⣤⣿⣯⡟⢧⠈⢢⡙⡿⠁⣾⢿⣿⡇⢸⣿⣟⣾⠰⡇⣸⠱⣟⣿⣷⡀⠀⣿⣿⣳⢻⡽⣿⡳⣽⢣⠇⢈⡷⣿⣏⠸⣿⣯⢿⣿⡹⣿⣷⡈⠙⠳⠟⠐⠀⠀⣿⣿⣇⣼⣿⣯⢿⣿⣷⣿⡇⠘⣿⣯⡛⣾⣿⣟⢰⣿⢿⡟⠀\n");
    printf("⠀⣼⣿⣿⣿⣽⣿⡿⣫⣿⡇⢰⣿⣟⣿⡏⣿⣿⣆⠹⢿⡿⣟⣷⡻⢸⡄⠀⢻⠃⠀⣿⣿⣿⣀⣾⣿⣽⡎⢸⠁⡏⠀⣿⣻⣿⣿⣾⣿⡿⣽⠃⢻⡷⣽⢋⡎⠀⣴⡿⣿⡿⢀⣿⣟⢺⣿⣧⠹⣿⣿⣄⠀⠙⡆⠀⠀⣿⣿⣿⣿⡟⣾⣿⣿⣿⡽⣇⠀⣹⣿⣽⠸⣿⣿⢨⣿⣿⣷⠀\n");
    printf("⣼⣿⣟⣿⣿⣿⣿⢃⣿⣿⣷⣿⣿⢞⣿⡇⢸⣿⣽⡄⠠⡙⢿⣯⡗⢨⠀⠀⡘⠰⡰⠘⣿⢿⣿⢿⣻⡞⠁⡾⠀⢱⡐⠈⠷⣯⣟⣯⣟⡽⢏⠀⢨⡿⢁⡞⠀⢰⣿⣿⣿⠃⢈⣿⣿⡃⣿⣿⠄⢻⣿⣽⠀⠀⢧⡀⡆⢹⣾⡽⠃⣾⣿⡿⢸⣿⣽⣿⣦⣿⣿⡏⢘⣷⣿⢸⣿⢿⣿⡄\n");
    printf("⣿⣿⢽⣿⣿⣿⢾⠀⢿⣻⢿⣯⠏⣾⣿⡇⠘⣿⣯⢿⠀⠙⣆⠹⠇⣤⠇⡄⡃⠀⡇⠀⢈⠙⠙⢋⡁⠀⠠⢹⠀⣄⠑⡌⠀⡀⠉⡈⢀⣰⡾⠀⣼⣣⠋⠇⡀⢺⣯⣿⡟⠀⢀⣿⣟⡇⣿⣿⡇⢸⣿⣾⠁⡆⢸⡇⢸⠈⡻⠀⢸⣟⣿⡇⠸⣿⣷⠻⣿⣿⠟⠀⣸⣿⣿⣾⣿⢯⣿⡧\n");
    printf("⣿⣿⢸⣿⣿⣿⣻⡇⠈⠻⣿⠁⠨⡷⣿⣷⣤⣽⣿⣻⠅⠀⠘⡆⣸⠏⡰⠀⡇⡄⠘⡄⠂⠈⢀⠀⠲⣀⠀⣽⠀⡈⡦⡈⠢⢑⠀⣴⠞⢁⡠⢀⡽⠁⢸⠀⠀⢹⣿⣿⡇⢀⣾⣿⣿⠁⢿⣽⣿⣿⣿⠟⢠⠃⡘⢧⠈⢫⠀⠀⠸⣯⣿⣷⣤⣿⣿⡇⠘⣋⠆⢠⣿⣿⣿⣿⠏⣾⣿⡇\n");
    printf("⠹⣿⣧⠻⣿⢿⣷⣻⣄⠘⣄⠀⢀⠿⣽⣻⢿⡿⣯⠟⠀⠀⠀⡼⠃⣴⠇⣸⠁⢸⢂⠘⢔⠀⡀⠀⡄⠻⣄⠘⣇⢡⠀⡉⠢⡀⠉⠁⡔⠋⠁⠨⠁⢀⢾⡀⠆⠘⣷⢿⣿⣿⣿⡿⠃⠀⢊⢉⡛⠋⠁⡠⠃⢠⠃⡘⠠⡀⢷⡀⡀⠻⢽⣿⣿⣿⠟⠀⠐⠁⣠⠿⠿⠛⠋⢁⣰⣿⣿⠁\n");
    printf("⠀⠹⣿⣷⣌⡙⠛⠽⠷⢧⡈⠢⣄⠲⢍⡛⠳⠛⠁⢊⣠⠞⠋⣰⡟⢁⣴⠇⢀⠂⣦⣉⠢⢄⡑⠄⠈⣶⢄⣁⠙⠤⡂⠹⢦⣈⠓⠈⢠⡶⣼⡄⡶⣹⣧⣳⡘⠰⢌⡛⠚⢓⢫⠴⠁⠀⠈⠁⠀⢐⡭⠔⢒⡇⢠⣇⢠⣆⠈⣷⣈⠢⢄⡠⢉⠀⠀⣀⠄⠘⠁⠀⠀⣠⣴⡿⣿⡿⠋⠀\n");
    printf("⠀⠀⠈⠙⠿⢽⣷⣶⣤⣤⣌⣦⣈⣳⣶⣤⣤⣴⣠⣭⣴⣶⣛⣧⣴⣾⣩⣴⣾⣧⣝⣯⣷⣶⣭⣗⣤⣈⣛⣶⣭⣝⣃⣂⣀⣉⣻⣦⣔⣿⣮⣅⣁⣻⣾⣽⣻⣧⣤⣥⣤⣠⣄⣤⣠⣤⣤⣴⣶⣯⣤⣶⣯⣴⣟⣿⣮⣟⣷⣮⣟⣿⣶⣶⣖⣶⣾⣥⣖⣶⣲⣮⣷⠿⠞⠋⠁⠀⠀⠀\n");
    printf("⠀⠀⠀⠀⠀⠀⠀⠀⠉⠉⠁⠀⠁⠀⠀⠀⠀⠀⠁⠀⠀⠀⠁⠀⠀⠀⠁⠀⠀⠀⠈⠀⠀⠀⠀⠀⠀⠁⠈⠀⠀⠀⠁⠈⠀⠁⠀⠀⠀⠀⠀⠈⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n");
    printf("  INE5420 - Compiladores - Prof. Alvaro Junio Pereira Franco - GRUPO: Nicolas, Patricia, Gustavo\n");
}


int main(int argc, char *argv[]) {
    print_fire();
    printf("\nEquipe de desenvolvimento:\n");
    printf("Nicolas Elias, Patricia Bardini, Gustavo Fukishima\n\n");

    if (argc < 2) {
        fprintf(stderr, "Uso: %s <arquivo>\n", argv[0]);
        return 1;
    }

    const char* filename = argv[1];
    FILE* file_ptr = fopen(filename, "r");
    if (!file_ptr) {
        LANCAR_ERRO_LEXICO("Não foi possível abrir o arquivo");
    }

    char* keywords_c_arr[] = {
        "def", "if", "else", "for", "int", "float", "string",
        "return", "new", "read", "print", "break", "call"
    };
    int num_keywords = sizeof(keywords_c_arr) / sizeof(keywords_c_arr[0]);

    GerenciadorEscopo* scope_manager = NULL;
    AnalisadorLexico* lex_analyzer = NULL;
    AnalisadorSintatico* syntax_analyzer = NULL;

    scope_manager = criar_gerenciador_escopo();
    if (!scope_manager) {
        fprintf(stderr, "Erro: Falha ao criar o gerenciador de escopo.\n");
        fclose(file_ptr);
        return 1;
    }

    lex_analyzer = criar_analisador_lexico(file_ptr, keywords_c_arr, num_keywords);
    if (!lex_analyzer) {
        fprintf(stderr, "Erro: Falha ao inicializar o analisador léxico.\n");
        liberar_gerenciador_escopo(scope_manager);
        fclose(file_ptr);
        return 1;
    }

    syntax_analyzer = criar_analisador_sintatico(scope_manager, tabela_analise, num_entradas_tabela);
    if (!syntax_analyzer) {
        fprintf(stderr, "Erro: Falha ao inicializar o analisador sintático.\n");
        destruir_analisador_lexico(lex_analyzer);
        liberar_gerenciador_escopo(scope_manager);
        fclose(file_ptr);
        return 1;
    }

    Token* current_token = NULL;
    // Loop principal de processamento de tokens
    while ((current_token = proximo_token(lex_analyzer)) != NULL) {
        analisar_token(syntax_analyzer, current_token);
    }

    // Processa o token de fim de ficheiro (EOF)
    Token* eof_token = criar_token("$", "$", lex_analyzer->linha, 0); 
    if (!eof_token) {
        fprintf(stderr, "Erro: Falha ao criar o token EOF.\n");
        // Limpeza de recursos antes de sair
        liberar_analisador_sintatico(syntax_analyzer);
        destruir_analisador_lexico(lex_analyzer);
        liberar_gerenciador_escopo(scope_manager);
        fclose(file_ptr);
        return 1;
    }
    analisar_token(syntax_analyzer, eof_token);

    if (!analise_completa(syntax_analyzer)) {
        LANCAR_ERRO_SINTATICO("Fim de arquivo inesperado ou análise sintática incompleta.");
    }

    printf("\n************************************************************\n\n");
    printf("Processamento concluído com sucesso!\n");
    printf("Todas as expressões foram validadas\n");
    printf("Todas as declarações de variáveis estão corretas\n");
    printf("Todas as instruções 'break' são válidas\n");
    printf("\n************************************************************\n\n");

    printf("Tabelas de Símbolos Geradas:\n\n");
    imprimir_tabelas_gerenciador(scope_manager, stdout);

    printf("\n************************************************************\n\n");
    printf("Código Intermediário Gerado:\n\n");
    if (syntax_analyzer->raiz_ast && syntax_analyzer->raiz_ast->codigo) {
        print_intermediate_code(syntax_analyzer->raiz_ast->codigo);
    } else {
        printf("Nenhum código intermediário foi gerado ou a raiz da AST é inválida.\n");
    }

    printf("\n************************************************************\n\n");
    printf("Execução finalizada com sucesso!\n");

    // Liberação de todos os recursos alocados
    liberar_analisador_sintatico(syntax_analyzer);
    destruir_analisador_lexico(lex_analyzer);
    liberar_gerenciador_escopo(scope_manager);
    
    fclose(file_ptr);
    
    return 0;
}