#include <fstream>    // For file operations (could use <cstdio> for FILE*)
#include <iostream>   // For console output (std::cout, std::cerr)
#include <cstdio>     // For FILE*, fopen, fclose, stdout
#include <cstring>    // For strcmp, strlen
#include <cstdlib>    // For exit (used by error macros)
#include <stdexcept>  // For std::runtime_error (for initial setup errors)

// Include the new C-style header files for your compiler components
#include "acoes_semanticas.h"
#include "analisador_lexico.h"
#include "analisador_sintatico.h"
#include "erros.h" // Contains LANCAR_ERRO_ macros that handle program exit on error
#include "gerenciador_escopo.h"
#include "no_ast.h" // Defines NoAST structure and related functions for AST and intermediate code (ListaString)
#include "tabela_analise.h" // Defines the global parse table (tabela_analise)
#include "token.h" // Defines the Token structure and creation/liberation functions

// Global variable for label counting. This counter is passed by reference
// to AST nodes to ensure unique labels are generated for intermediate code.
static int global_label_counter = 0;

/**
 * @brief Prints the generated intermediate code from a ListaString structure.
 * @param code_list A pointer to the ListaString containing the code lines.
 */
void print_intermediate_code(ListaString* code_list) {
    if (!code_list) {
        return; // Do nothing if the list is null
    }
    // Iterate through the list of strings and print each line
    for (int i = 0; i < code_list->tamanho; ++i) {
        std::cout << code_list->itens[i] << std::endl;
    }
}

/**
 * @brief Prints a decorative ASCII art for the compiler's start,
 * mimicking the Rust example's visual output.
 */
void print_fire() {
    std::cout <<
R"(⠀⠀⠀⠀⠀⠀⠀⠀           ⣦⣀⠀⠀⠀⠀⠀ ⢲⣄⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀  ⣠⡆⠀⠀⠀⠀⠀⠀⠀⠛⣦⣄⣀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠙⣿⣷⣤⠀⠀⠀⠀⠀⢻⣿⣷⣄⢀⠀⠀⠀⠀⠀⠀⢀⣴⣿⡟⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⢻⣿⣷⣦⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠘⣿⣿⣷⣄⠀⠀⠀⠀⣿⣿⣿⣷⠱⣆⠀⠀⠀⢀⣾⣿⣿⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣿⣿⣿⣿⣆⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢹⣿⣽⣿⡆⠀⠀⠀⢸⣿⣞⣿⣧⢸⣷⣤⠀⢸⣿⣯⣿⠆⠀⠀⠀⠀⠀⠀⠀⠀⢀⣴⡟⢰⣿⡷⣿⣿⡄⠀⠀⠀⠀⠀⠀⠀⠀⣿⣦⡄⠀⠀⠀⠀⢻⣿⣶⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⣟⣾⣿⠀⠀⠀⣼⣿⡏⣿⣿⠀⣿⣿⣗⠺⣿⣳⣿⣧⠀⠀⠀⠀⠀⠀⠀⣴⣿⡟⠀⣸⣿⡟⣽⣿⠇⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⣿⣆⠀⠀⠀⠀⣿⣿⣿⣦⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠸⣦⠀⠀⠀⠀⠀⠀⠀⠀⣼⣿⣯⢿⣿⡇⠀⣰⣿⡿⢸⣿⡿⠀⣼⣿⣻⡦⣿⣯⢿⣿⡆⠀⠀⠀⠀⢀⣾⣿⣿⠀⢠⣿⡿⢱⣿⡿⠀⠀⠀⠀⠀⠀⠀⠀⠀⣼⣿⢯⣿⠀⠀⢠⠀⣼⣿⣻⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢻⣷⡀⠀⠀⠀⠀⠀⣰⣿⣿⣞⣿⣿⠃⢀⣿⡿⣡⣿⡿⠃⢀⣿⣿⣽⣷⢹⣿⣻⢿⣿⡄⠀⠀⢀⣾⣿⢿⡇⠀⣾⣿⣱⣿⣿⠃⠀⠀⠀⠀⠀⠀⠀⠀⣰⣿⣿⣳⣿⡟⠀⣾⡇⢸⣷⣿⠏⠀⠀⠀⠀⠀⠀⠀⠀⠀⡀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⠀⠀⠀⠀⠀⠀⠀⠀⢻⣿⣿⡄⠀⢠⣿⣿⢯⣳⣿⣿⣦⣄⠘⣿⣿⡿⠁⣴⣿⣿⢯⣽⣿⡟⢀⣿⣷⢯⣿⣿⡇⠀⣿⣿⣽⣻⣿⠀⣿⣷⣿⠇⠀⠀⠀⠀⣀⠀⠀\n"
R"(⢀⣼⣿⣿⣳⢿⣿⠁⣰⣿⣇⢸⣿⡟⠀⠀⠀⠀⠀⠀⠀⠀⣠⡾⠁⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⢠⣾⠇⠀⠀⠀⠀⠀⠀⠀⠀⣸⣿⢿⣷⠀⣿⣿⢯⣟⣼⣿⡇⢻⣿⣧⠘⣿⠃⣼⣿⡿⡽⣞⣿⣿⠁⢸⣿⣟⣮⢿⣿⡇⠀⣿⣿⢶⣻⣿⣇⠘⣿⣿⠀⠀⢀⣴⡿⠁⠀⣠⣿⢱⣿⣯⣽⣻⣿⠀⣿⡿⣿⡄⢻⣧⠀⠀⠀⠀⠀⠀⣴⣾⡟⠁⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⣰⣿⣿⠀⠀⠀⠀⠀⠀⠀⠀⢀⣿⣿⢿⡇⢰⣿⣯⡟⣼⣿⣿⠁⠘⣿⣽⣧⠈⢰⣿⣯⡷⣛⣿⣿⠃⠀⣼⣿⣿⠏⣾⣿⣟⣰⡇⠘⣿⣿⣳⣭⢿⣿⣧⠀⢠⣿⣿⡃⠀\n"
R"(⢰⣿⡿⡇⠸⣿⣯⡇⢻⣿⣿⣯⣿⣿⣿⣿⣷⡀⠀⠀⢀⣿⣿⣻⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠐⣿⣯⣿⣇⠱⣆⡀⠀⠀⠀⣸⣿⡿⣽⡿⢸⣿⢯⡇⢸⣟⣯⣟⠀⠀⣻⣿⣻⣿⢼⣷⣻⠄⢸⣿⣿⣿⣿⡿⠏⣸⣿⣿⣷⣿⡅⠀⢸⣿⣷⣏⣾⣻⣿⡆⢸⣿⢿⡅⠀⣾⣿⣻⣿⠐⣿⣿⣻⡄⠻⣽⣿⣯⣿⣷⣻⢿⣿⡆⠀⢸⣿⣟⣿⣇⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠈⣧⡀⠀⢻⣿⣾⣿⣆⠹⣿⣆⠀⢠⣿⣿⡽⣿⡇⢸⣿⢯⡇⠈⣿⣻⢿⣷⣿⡿⣭⣿⡟⠘⣿⣟⣯⠀⠐⡀⢂⢠⣿⣿⣿⠍⣿⣿⢿⣿⣦⣼⣿⣟⣾⠇⢹⣯⢿⡏⣿⡿⣿⡀⢺⣿⣞⣿⣿⣦⣹⣿⣯⠇⠀⣿⣻⣿⠀⣿⣯⣟⣿⣷⠈⣿⣿⢯⡿⣿⣧⡀⠀⠀⠀⢀⣀⣤
⠀⠀⠀⣿⣷⡀⠸⣿⣷⣻⣿⡀⣿⣿⡧⢼⣿⢷⣻⣿⡇⢹⣿⣟⡆⠀⢜⢯⡿⡽⣯⢷⣯⣿⡷⢨⣿⣟⣾⠀⡼⠁⡌⣾⣿⣿⡏⠀⣿⣿⢯⡿⣿⣟⣿⡽⣾⠀⠐⣯⣿⣟⢿⣿⢿⣷⡜⣿⣯⠺⣟⡿⣿⣟⡿⡀⠀⡿⣽⣿⠀⣿⣿⢞⣿⡟\n"
R"(⣲⡿⢻⣿⣳⢟⣿⣿⡄⠀⣴⣿⣿⠃
⠀⠀⣼⣿⣯⡇⣼⣿⢷⣻⣿⠀⣽⡿⣽⣿⣿⣽⢸⣿⣿⣤⣿⣯⡟⢧⠈⢢⡙⡿⠁⣾⢿⣿⡇⢸⣿⣟⣾⠰⡇⣸⠱⣟⣿⣷⡀⠀⣿⣿⣳⢻⡽⣿⡳⣽⢣⠇⢈⡷⣿⣏⠸⣿⣯⢿⣿⡹⣿⣷⡈⠙⠳⠟⠐⠀⠀⣿⣿⣇⣼⣿⣯⢿⣿⣷⣿⡇⠘⣿⣯⡛⣾⣿⣟⢰⣿⢿⡟⠀
⠀⣼⣿⣿⣿⣽⣿⡿⣫⣿⡇⢰⣿⣟⣿⡏⣿⣿⣆⠹⢿⡿⣟⣷⡻⢸⡄⠀⢻⠃⠀⣿⣿⣿⣀⣾⣿⣽⡎⢸⠁⡏⠀⣿⣻⣿⣿⣾⣿⡿⣽⠃⢻⡷⣽⢋⡎⠀⢰⣿⣿⣿⠃⢈⣿⣟⡇⣿⣿⠄⢻⣿⣽⠀⠀⢧⡀⡆⢹⣾⡽⠃⣾⣿⡿⢸⣿⣽⣿⣦⣿⣿⡏⢘⣷⣿⢸⣿⢿⣿⡄
⣿⣿⢽⣿⣿⣿⢾⠀⢿⣻⢿⣯⠏⣾⣿⡇⠘⣿⣯⢿⠀⠙⣆⠹⠇⣤⠇⡄⡃⠀⡇⠀⢈⠙⠙⢋⡁⠀⠠⢹⠀⣄⠑⡌⠀⡀⠉⡈⢀⣰⡾⠀⣼⣣⠋⠇⡀⢺⣯⣿⡟⠀⢀⣿⣟⡇⣿⣿⡇⢸⣿⣾⠁⡆⢸⡇⢸⠈⡻⠀⢸⣟⣿⡇⠸⣿⣷⠻⣿⣿⠟⠀⣸⣿⣿⣾⣿⢯⣿⡧
⣿⣿⢸⣿⣿⣿⣻⡇⠈⠻⣿⠁⠨⡷⣿⣷⣤⣽⣿⣻⠅⠀⠘⡆⣸⠏⡰⠀⡇⡄⠘⡄⠂⠈⢀⠀⠲⣀⠀⣽⠀⡈⡦⡈⠢⢑⠀⣴⠞⢁⡠⢀⡽⠁⢸⠀⠀⢹⣿⣿⡇⢀⣾⣿⣿⠁⢿⣽⣿⣿⣿⠟\n"
R"(⢠⠃⡘⢧⠈⢫⠀⠀⠸⣯⣿⣷⣤⣿⣿⡇⠘⣋⠆⢠⣿⣿⣿⣿⠏⣾⣿⡇
⠹⣿⣧⠻⣿⢿⣷⣻⣄⠘⣄⠀⢀⠿⣽⣻⢿⡿⣯⠟⠀⠀⠀⡼⠃⣴⠇⣸⠁⢸⢂⠘⢔⠀⡀⠀⡄⠻⣄⠘⣇⢡⠀⡉⠢⡀⠉⠁⡔⠋⠁⠨⠁⢀⢾⡀⠆⠘⣷⢿⣿⣿⣿⡿⠃⠀⢊⢉⡛⠋⠁⡠⠃⢠⠃⡘⠠⡀⢷⡀⡀⠻⢽⣿⣿⣿⠟⠀⠐⠁⣠⠿⠿⠛⠋⢁⣰⣿⣿⠁
⠀⠹⣿⣷⣌⡙⠛⠽⠷⢧⡈⠢⣄⠲⢍⡛⠳⠛⠁⢊⣠⠞⠋⣰⡟⢁⣴⠇⢀⠂⣦⣉⠢⢄⡑⠄⠈⣶⢄⣁⠙⠤⡂⠹⢦⣈⠓⠈⢠⡶⣼⡄⡶⣹⣧⣳⡘⠰⢌⡛⠚⢓⢫⠴⠁⠀⠈⠁⠀⢐⡭⠔⢒⡇⢠⣇⢠⣆⠈⣷⣈⠢⢄⡠⢉⠀⠀⣀⠄⠘⠁⠀⠀⣠⣴⡿⣿⡿⠋⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠉⠉⠁⠀⠁⠀⠀⠀⠀⠀⠁⠀⠀⠀⠁⠀⠀⠀⠁⠀⠀⠀⠈⠀⠀⠀⠀⠀⠀⠁⠈⠀⠀⠀⠁⠈⠀⠁⠀⠀⠀⠀⠀⠈⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
INE5420 - Compiladores - Prof. Alvaro Junio Pereira Franco - GRUPO: Nicolas, Patricia, Gustavo)" << std::endl;
}

/**
 * @brief Main entry point of the compiler application.
 * Handles command-line arguments, initializes compiler components,
 * processes the source file, and prints compilation results.
 * @param argc The number of command-line arguments.
 * @param argv An array of command-line argument strings.
 * @return 0 on successful compilation, 1 on error. Errors usually cause
 * program termination via `LANCAR_ERRO_...` macros.
 */
int main(int argc, char *argv[]) {
    print_fire(); // Print the decorative ASCII art
    std::cout << "\nEquipe de desenvolvimento:" << std::endl;
    std::cout << "Nicolas Elias, Patricia Bardini, Gustavo Fukishima\n" << std::endl;

    // Check if a filename argument is provided
    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <arquivo>\n";
        return 1; // Indicate an error due to incorrect usage
    }

    const char* filename = argv[1];
    // Open the input file in read mode
    FILE* file_ptr = fopen(filename, "r");
    if (!file_ptr) {
        // Report a lexical error if the file cannot be opened.
        // LANCAR_ERRO_LEXICO macro will print the error message and call exit(EXIT_FAILURE).
        LANCAR_ERRO_LEXICO("Não foi possível abrir o arquivo");
    }

    // Define keywords as a C-style array of character pointers.
    // Explicitly casting to (char*) for string literals as they are const.
    char* keywords_c_arr[] = {
        (char*)"def", (char*)"if", (char*)"else", (char*)"for", (char*)"int", (char*)"float", (char*)"string",
        (char*)"return", (char*)"new", (char*)"read", (char*)"print", (char*)"break", (char*)"call"
    };
    int num_keywords = sizeof(keywords_c_arr) / sizeof(keywords_c_arr[0]);

    // Declare pointers for compiler components. They will be dynamically allocated.
    GerenciadorEscopo* scope_manager = NULL;
    AnalisadorLexico* lex_analyzer = NULL;
    AnalisadorSintatico* syntax_analyzer = NULL;

    // --- Component Initialization ---
    // Create the Scope Manager
    scope_manager = criar_gerenciador_escopo();
    if (!scope_manager) {
        std::cerr << "Erro: Falha ao criar o gerenciador de escopo.\n";
        if (file_ptr) fclose(file_ptr); // Close file before exiting
        return 1;
    }

    // Create the Lexical Analyzer, passing the file pointer and keywords.
    // `criar_analisador_lexico` is expected to handle its own critical failures by exiting.
    lex_analyzer = criar_analisador_lexico(file_ptr, keywords_c_arr, num_keywords);
    if (!lex_analyzer) {
        std::cerr << "Erro: Falha ao inicializar o analisador léxico.\n";
        liberar_gerenciador_escopo(scope_manager); // Clean up already created components
        if (file_ptr) fclose(file_ptr);
        return 1;
    }

    // Create the Syntax Analyzer (Parser), passing the scope manager and parse table.
    // `tabela_analise` and `num_entradas_tabela` are assumed to be globally available (`extern const`).
    syntax_analyzer = criar_analisador_sintatico(scope_manager, tabela_analise, num_entradas_tabela);
    if (!syntax_analyzer) {
        std::cerr << "Erro: Falha ao inicializar o analisador sintático.\n";
        liberar_analisador_lexico(lex_analyzer);
        liberar_gerenciador_escopo(scope_manager);
        if (file_ptr) fclose(file_ptr);
        return 1;
    }

    // --- Token Processing Loop ---
    Token* current_token = NULL;
    while (true) {
        // Get the next token from the lexical analyzer.
        // `proximo_token` allocates a new `Token*` for each token it returns.
        current_token = proximo_token(lex_analyzer);

        if (!current_token) { // `proximo_token` returns NULL when EOF is reached
            break;
        }

        // Pass the token to the syntax analyzer.
        // The parser (`analisar_token`) is responsible for managing the `current_token`'s memory.
        // It either associates it with an AST node (transferring ownership) or frees it.
        // Critical parsing errors within `analisar_token` should also cause program exit via `LANCAR_ERRO_SINTATICO`.
        analisar_token(syntax_analyzer, current_token);
        // Do NOT free `current_token` here, as ownership has been transferred to the parser/AST.
    }

    // --- Process End-of-File (EOF) Token ---
    // After all tokens from the file are processed, send a special EOF token to the parser
    // to complete the parsing process.
    Token* eof_token = criar_token("$", "$", 0, 0); // Create an EOF token (lexeme "$", ID "$", line 0, column 0)
    if (!eof_token) {
        std::cerr << "Erro: Falha ao criar o token EOF.\n";
        // Clean up already created components before exiting
        liberar_analisador_sintatico(syntax_analyzer);
        liberar_analisador_lexico(lex_analyzer);
        liberar_gerenciador_escopo(scope_manager);
        if (file_ptr) fclose(file_ptr);
        return 1;
    }
    // Pass the EOF token to the parser. The parser will handle its memory.
    analisar_token(syntax_analyzer, eof_token);

    // --- Final Compilation Check ---
    // After all tokens (including EOF) have been processed, check if the parsing is complete.
    // If not, it indicates an unexpected end of file or a syntax error that wasn't caught earlier.
    if (!analise_completa(syntax_analyzer)) {
        LANCAR_ERRO_SINTATICO("Fim de arquivo inesperado ou análise sintática incompleta.");
        // This macro will print the error and call exit(EXIT_FAILURE).
    }

    // --- Compilation Success Output ---
    std::cout << "\n" << "************************************************************" << "\n\n";
    std::cout << "Processamento concluído com sucesso!" << std::endl;
    std::cout << "Todas as expressões foram validadas" << std::endl;
    std::cout << "Todas as declarações de variáveis estão corretas" << std::endl;
    std::cout << "Todas as instruções 'break' são válidas" << std::endl;
    std::cout << "\n" << "************************************************************" << "\n\n";

    // --- Print Generated Symbol Tables ---
    std::cout << "Tabelas de Símbolos Geradas:\n" << std::endl;
    // `imprimir_tabelas` from `gerenciador_escopo.h` prints to a FILE* (stdout in this case).
    imprimir_tabelas(scope_manager, stdout);

    std::cout << "\n" << "************************************************************" << "\n\n";

    // --- Print Generated Intermediate Code ---
    std::cout << "Código Intermediário Gerado:\n" << std::endl;
    // The generated intermediate code is stored within the AST root node's `codigo` field.
    if (syntax_analyzer->raiz_ast && syntax_analyzer->raiz_ast->codigo) {
        print_intermediate_code(syntax_analyzer->raiz_ast->codigo);
    } else {
        std::cout << "Nenhum código intermediário foi gerado ou a raiz da AST é inválida." << std::endl;
    }

    std::cout << "\n" << "************************************************************" << "\n";
    std::cout << "Execução finalizada com sucesso!" << std::endl;

    // --- Cleanup Resources ---
    // Ensure all dynamically allocated components are properly freed.
    // These functions are designed to handle NULL pointers gracefully, so order is less critical
    // if a component wasn't successfully created.
    liberar_analisador_sintatico(syntax_analyzer);
    liberar_analisador_lexico(lex_analyzer);
    liberar_gerenciador_escopo(scope_manager);
    
    // Close the input file stream.
    if (file_ptr) {
        fclose(file_ptr);
    }
    
    return 0; // Return 0 to indicate successful program execution.
}
