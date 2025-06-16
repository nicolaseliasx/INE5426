// main.rs
use std::env;
use std::fs::File;
use std::io::{self, Read, Write};
use std::path::Path;
use std::rc::Rc;
use std::cell::RefCell;
use crate::errors::CompilationError;
use crate::lexical_analyzer::LexicalAnalyzer;
use crate::parser::SyntaxAnalyzer;
use crate::scope_manager::ScopeManager;
use crate::token::Token;
use crate::ast_node::AstNode;
use crate::parse_table::PARSE_TABLE;
use crate::token_identifiers::*;

fn print_fire() {
    println!(
        r"⠀⠀⠀⠀⠀⠀⠀⠀      ⣦⣀⠀⠀⠀⠀⠀ ⢲⣄⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀  ⣠⡆⠀⠀⠀⠀⠀⠀⠀⠛⣦⣄⣀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠙⣿⣷⣤⠀⠀⠀⠀⠀⢻⣿⣷⣄⢀⠀⠀⠀⠀⠀⠀⢀⣴⣿⡟⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⢻⣿⣷⣦⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠘⣿⣿⣷⣄⠀⠀⠀⠀⣿⣿⣿⣷⠱⣆⠀⠀⠀⢀⣾⣿⣿⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣿⣿⣿⣿⣆⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢹⣿⣽⣿⡆⠀⠀⠀⢸⣿⣞⣿⣧⢸⣷⣤⠀⢸⣿⣯⣿⠆⠀⠀⠀⠀⠀⠀⠀⠀⢀⣴⡟⢰⣿⡷⣿⣿⡄⠀⠀⠀⠀⠀⠀⠀⠀⣿⣦⡄⠀⠀⠀⠀⢻⣿⣶⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⣟⣾⣿⠀⠀⠀⣼⣿⡏⣿⣿⠀⣿⣿⣗⠺⣿⣳⣿⣧⠀⠀⠀⠀⠀⠀⠀⣴⣿⡟⠀⣸⣿⡟⣽⣿⠇⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⣿⣆⠀⠀⠀⠀⣿⣿⣿⣦⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠸⣦⠀⠀⠀⠀⠀⠀⠀⠀⣼⣿⣯⢿⣿⡇⠀⣰⣿⡿⢸⣿⡿⠀⣼⣿⣻⡦⣿⣯⢿⣿⡆⠀⠀⠀⠀⢀⣾⣿⣿⠀⢠⣿⡿⢱⣿⡿⠀⠀⠀⠀⠀⠀⠀⠀⠀⣼⣿⢯⣿⠀⠀⢠⠀⣼⣿⣻⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢻⣷⡀⠀⠀⠀⠀⠀⣰⣿⣿⣞⣿⣿⠃⢀⣿⡿⣡⣿⡿⠃⢀⣿⣿⣽⣷⢹⣿⣻⢿⣿⡄⠀⠀⢀⣾⣿⢿⡇⠀⣾⣿⣱⣿⣿⠃⠀⠀⠀⠀⠀⠀⠀⠀⣰⣿⣿⣿⣿⠁⢀⡿⢰⣿⣣⣿⠇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⣿⣿⡄⠀⠀⠀⢰⣿⣿⣻⣼⣿⡿⠀⢸⣿⢣⣿⡿⠁⣠⣿⣿⡟⣾⣿⢈⣿⣯⣟⣿⣷⠀⠀⣸⣿⣟⣿⡇⠀⣿⣧⣿⣿⠃⠀⠀⠀⠀⠀⠀⠀⠀⣰⣿⣿⣳⣿⡟⠀⣾⡇⢸⣷⣿⠏⠀⠀⠀⠀⠀⠀⠀⠀⠀⡀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⠀⠀⠀⠀⠀⠀⠀⠀⢻⣿⣿⡄⠀⢠⣿⣿⢯⣳⣿⣿⣦⣄⠘⣿⣿⡿⠁⣴⣿⣿⢯⣽⣿⡟⢀⣿⣷⢯⣿⣿⡇⠀⣿⣿⣽⣻⣿⠀⣿⣷⣿⠇⠀⠀⠀⠀⣀⠀⠀⢀⣼⣿⣿⣳⢿⣿⠁⣰⣿⣇⢸⣿⡟⠀⠀⠀⠀⠀⠀⠀⠀⣠⡾⠁⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⢠⣾⠇⠀⠀⠀⠀⠀⠀⠀⠀⣸⣿⢿⣷⠀⣿⣿⢯⣟⣼⣿⡇⢻⣿⣧⠘⣿⠃⣼⣿⡿⡽⣞⣿⣿⠁⢸⣿⣟⣮⢿⣿⡇⠀⣿⣿⢶⣻⣿⣇⠘⣿⣿⠀⠀⢀⣴⡿⠁⠀⣠⣿⢱⣿⣯⣽⣻⣿⠀⣿⡿⣿⡄⢻⣧⠀⠀⠀⠀⠀⠀⣴⣾⡟⠁⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⣰⣿⣿⠀⠀⠀⠀⠀⠀⠀⠀⢀⣿⣿⢿⡇⢰⣿⣯⡟⣼⣿⣿⠁⠘⣿⣽⣧⠈⢰⣿⣯⡷⣛⣿⣿⠃⠀⣼⣿⣿⢼⣻⣿⡇⠀⢺⣿⡿⣼⣻⣿⣦⠘⠇⠀⣠⣿⡿⠁⠀⣰⣿⡟⢸⣿⣳⢾⡽⣿⣇⢸⣿⡿⣿⣆⠙⠀⠀⠀⠀⢀⣾⣿⣿⠁⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⢀⣿⣿⣿⠀⠀⠀⠀⠀⠀⠀⠀⣼⣿⡿⣿⠃⣿⣟⣾⢡⡿⣿⡟⠀⠀⢿⣯⢿⡇⣼⣿⣞⡇⣿⣿⣯⠀⣼⣿⣿⠏⣾⣿⣟⣰⡇⠘⣿⣿⣳⣭⢿⣿⣧⠀⢠⣿⣿⡃⠀⢰⣿⡿⡇⠸⣿⣯⡇⢻⣿⣿⣯⣿⣿⣿⣿⣷⡀⠀⠀⢀⣿⣿⣻⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠐⣿⣯⣿⣇⠱⣆⡀⠀⠀⠀⣸⣿⡿⣽⡿⢸⣿⢯⡇⢸⣟⣯⣟⠀⠀⣻⣿⣻⣿⢼⣷⣻⠄⢸⣿⣿⣿⣿⡿⠏⣸⣿⣿⣷⣿⡅⠀⢸⣿⣷⣏⣾⣻⣿⡆⢸⣿⢿⡅⠀⣾⣿⣻⣿⠐⣿⣿⣻⡄⠻⣽⣿⣯⣿⣷⣻⢿⣿⡆⠀⢸⣿⣟⣿⣇⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠈⣧⡀⠀⢻⣿⣾⣿⣆⠹⣿⣆⠀⢠⣿⣿⡽⣿⡇⢸⣿⢯⡇⢸⣯⢿⣿⡀⣠⣿⣟⡷⣿⢸⣿⡽⣇⠀⡹⠾⠟⠋⢀⣾⣿⣿⣻⣿⢿⡆⠀⢈⣿⣿⢾⡘⣷⣻⣷⣸⣿⢿⣧⠀⣿⣿⣽⣿⡆⠘⣿⣿⣽⡀⢹⡾⣿⣯⢻⣿⣯⢿⣿⡄⢸⣿⣯⢿⣿⣆⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⣿⣷⡀⠸⣿⣷⣻⣿⡀⣿⣿⡧⢼⣿⢷⣻⣿⡇⢹⣿⣟⡆⠈⣿⣻⢿⣷⣿⡿⣭⣿⡟⠘⣿⣟⣯⠀⠐⡀⢂⢠⣿⣿⣿⠍⣿⣿⢿⣿⣦⣼⣿⣟⣾⠇⢹⣯⢿⡏⣿⡿⣿⡀⢺⣿⣞⣿⣿⣦⣹⣿⣯⠇⠀⣿⣻⣿⠀⣿⣯⣟⣿⣷⠈⣿⣿⢯⡿⣿⣧⡀⠀⠀⠀⢀⣀⣤
⠀⠀⠀⣿⣿⣇⠀⣿⣷⣻⣿⡇⢹⣯⣿⣼⣿⡇⣿⣿⡇⠘⣿⣯⡗⠀⢜⢯⡿⡽⣯⢷⣯⣿⡷⢨⣿⣟⣾⠀⡼⠁⡌⣾⣿⣿⡏⠀⣿⣿⢯⡿⣿⣟⣿⡽⣾⠀⠐⣯⣿⣟⢿⣿⢿⣷⡜⣿⣯⠺⣟⡿⣿⣟⡿⡀⠀⡿⣽⣿⠀⣿⣿⢞⣿⡟⣲⡿⢻⣿⣳⢟⣿⣿⡄⠀⣴⣿⣿⠃
⠀⠀⣼⣿⣯⡇⣼⣿⢷⣻⣿⠀⣽⡿⣽⣿⣿⣽⢸⣿⣿⣤⣿⣯⡟⢧⠈⢢⡙⡿⠁⣾⢿⣿⡇⢸⣿⣟⣾⠰⡇⣸⠱⣟⣿⣷⡀⠀⣿⣿⣳⢻⡽⣿⡳⣽⢣⠇⢈⡷⣿⣏⠸⣿⣯⢿⣿⡹⣿⣷⡈⠙⠳⠟⠐⠀⠀⣿⣿⣇⣼⣿⣯⢿⣿⣷⣿⡇⠘⣿⣯⡛⣾⣿⣟⢰⣿⢿⡟⠀
⠀⣼⣿⣿⣿⣽⣿⡿⣫⣿⡇⢰⣿⣟⣿⡏⣿⣿⣆⠹⢿⡿⣟⣷⡻⢸⡄⠀⢻⠃⠀⣿⣿⣿⣀⣾⣿⣽⡎⢸⠁⡏⠀⣿⣻⣿⣿⣾⣿⡿⣽⠃⢻⡷⣽⢋⡎⠀⣴⡿⣿⡿⢀⣿⣟⢺⣿⣧⠹⣿⣿⣄⠀⠙⡆⠀⠀⣿⣿⣿⣿⡟⣾⣿⣿⣿⡽⣇⠀⣹⣿⣽⠸⣿⣿⢨⣿⣿⣷⠀
⣼⣿⣟⣿⣿⣿⣿⢃⣿⣿⣷⣿⣿⢞⣿⡇⢸⣿⣽⡄⠠⡙⢿⣯⡗⢨⠀⠀⡘⠰⡰⠘⣿⢿⣿⢿⣻⡞⠁⡾⠀⢱⡐⠈⠷⣯⣟⣯⣟⡽⢏⠀⢨⡿⢁⡞⠀⢰⣿⣿⣿⠃⢈⣿⣿⡃⣿⣿⠄⢻⣿⣽⠀⠀⢧⡀⡆⢹⣾⡽⠃⣾⣿⡿⢸⣿⣽⣿⣦⣿⣿⡏⢘⣷⣿⢸⣿⢿⣿⡄
⣿⣿⢽⣿⣿⣿⢾⠀⢿⣻⢿⣯⠏⣾⣿⡇⠘⣿⣯⢿⠀⠙⣆⠹⠇⣤⠇⡄⡃⠀⡇⠀⢈⠙⠙⢋⡁⠀⠠⢹⠀⣄⠑⡌⠀⡀⠉⡈⢀⣰⡾⠀⣼⣣⠋⠇⡀⢺⣯⣿⡟⠀⢀⣿⣟⡇⣿⣿⡇⢸⣿⣾⠁⡆⢸⡇⢸⠈⡻⠀⢸⣟⣿⡇⠸⣿⣷⠻⣿⣿⠟⠀⣸⣿⣿⣾⣿⢯⣿⡧
⣿⣿⢸⣿⣿⣿⣻⡇⠈⠻⣿⠁⠨⡷⣿⣷⣤⣽⣿⣻⠅⠀⠘⡆⣸⠏⡰⠀⡇⡄⠘⡄⠂⠈⢀⠀⠲⣀⠀⣽⠀⡈⡦⡈⠢⢑⠀⣴⠞⢁⡠⢀⡽⠁⢸⠀⠀⢹⣿⣿⡇⢀⣾⣿⣿⠁⢿⣽⣿⣿⣿⠟⢠⠃⡘⢧⠈⢫⠀⠀⠸⣯⣿⣷⣤⣿⣿⡇⠘⣋⠆⢠⣿⣿⣿⣿⠏⣾⣿⡇
⠹⣿⣧⠻⣿⢿⣷⣻⣄⠘⣄⠀⢀⠿⣽⣻⢿⡿⣯⠟⠀⠀⠀⡼⠃⣴⠇⣸⠁⢸⢂⠘⢔⠀⡀⠀⡄⠻⣄⠘⣇⢡⠀⡉⠢⡀⠉⠁⡔⠋⠁⠨⠁⢀⢾⡀⠆⠘⣷⢿⣿⣿⣿⡿⠃⠀⢊⢉⡛⠋⠁⡠⠃⢠⠃⡘⠠⡀⢷⡀⡀⠻⢽⣿⣿⣿⠟⠀⠐⠁⣠⠿⠿⠛⠋⢁⣰⣿⣿⠁
⠀⠹⣿⣷⣌⡙⠛⠽⠷⢧⡈⠢⣄⠲⢍⡛⠳⠛⠁⢊⣠⠞⠋⣰⡟⢁⣴⠇⢀⠂⣦⣉⠢⢄⡑⠄⠈⣶⢄⣁⠙⠤⡂⠹⢦⣈⠓⠈⢠⡶⣼⡄⡶⣹⣧⣳⡘⠰⢌⡛⠚⢓⢫⠴⠁⠀⠈⠁⠀⢐⡭⠔⢒⡇⢠⣇⢠⣆⠈⣷⣈⠢⢄⡠⢉⠀⠀⣀⠄⠘⠁⠀⠀⣠⣴⡿⣿⡿⠋⠀
⠀⠀⠈⠙⠿⢽⣷⣶⣤⣤⣌⣦⣈⣳⣶⣤⣤⣴⣠⣭⣴⣶⣛⣧⣴⣾⣩⣴⣾⣧⣝⣯⣷⣶⣭⣗⣤⣈⣛⣶⣭⣝⣃⣂⣀⣉⣻⣦⣔⣿⣮⣅⣁⣻⣾⣽⣻⣧⣤⣥⣤⣠⣄⣤⣠⣤⣤⣴⣶⣯⣤⣶⣯⣴⣟⣿⣮⣟⣷⣮⣟⣿⣶⣶⣖⣶⣾⣥⣖⣶⣲⣮⣷⠿⠞⠋⠁⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠉⠉⠁⠀⠁⠀⠀⠀⠀⠀⠁⠀⠀⠀⠁⠀⠀⠀⠁⠀⠀⠀⠈⠀⠀⠀⠀⠀⠀⠁⠈⠀⠀⠀⠁⠈⠀⠁⠀⠀⠀⠀⠀⠈⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
  INE5420 - Compiladores - Prof. Alvaro Junio Pereira Franco - GRUPO: Nicolas, Patricia, Gustavo"
    );
}

// Função para imprimir a árvore AST
fn print_ast_tree(node: &Rc<RefCell<AstNode>>, depth: usize) {
    let node_ref = node.borrow();
    let indent = "  ".repeat(depth);
    
    println!("{}{}: {}", indent, node_ref.node_type, node_ref.token.as_ref().map(|t| &t.lexeme).unwrap_or(""));
    
    for child in &node_ref.children {
        print_ast_tree(child, depth + 1);
    }
}

// Função para imprimir o código intermediário
fn print_code(node: &Rc<RefCell<AstNode>>) {
    let node_ref = node.borrow();
    for code in &node_ref.generated_code {
        println!("{}", code);
    }
}

fn main() -> Result<(), Box<dyn std::error::Error>> {
    print_fire();
    println!("\nEquipe de desenvolvimento:");
    println!("Nicolas Elias, Patricia Bardini, Gustavo Fukishima\n");

    let args: Vec<String> = env::args().collect();
    if args.len() < 2 {
        eprintln!("Uso: {} <arquivo>", args[0]);
        return Ok(());
    }

    let filename = &args[1];
    let path = Path::new(filename);
    
    let mut file = File::open(path).map_err(|e| {
        CompilationError::lexical(format!("Não foi possível abrir o arquivo '{}': {}", filename, e))
    })?;
    
    let mut source = String::new();
    file.read_to_string(&mut source).map_err(|e| {
        CompilationError::lexical(format!("Erro ao ler o arquivo '{}': {}", filename, e))
    })?;

    let keywords = vec![
        "def", "if", "else", "for", "int", "float", "string",
        "return", "new", "read", "print", "break", "call"
    ];
    
    let scope_manager = Rc::new(RefCell::new(ScopeManager::new()));
    
    let mut lex_analyzer = LexicalAnalyzer::new(path, keywords)
        .map_err(|e| format!("Falha na inicialização do analisador léxico: {}", e))?;
    
    let mut parser = SyntaxAnalyzer::new(Rc::clone(&scope_manager));
    
    while let Some(token) = lex_analyzer.next_token()? {
        parser.process_token(&token)?;
    }
    
    let eof_token = Token {
        lexeme: "$".to_string(),
        token_type: "$".to_string(),
        line: 0,
        column: 0,
    };
    parser.process_token(&eof_token)?;
    
    if !parser.is_complete() {
        return Err(Box::new(CompilationError::syntax(
            "Fim de arquivo inesperado".to_string()
        )));
    }
    
    let ast_root = parser.ast_root();
    
    println!("\n{}\n", "*".repeat(60));
    println!("Processamento concluído com sucesso!");
    println!("Todas as expressões foram validadas");
    println!("Todas as declarações de variáveis estão corretas");
    println!("Todas as instruções 'break' são válidas");
    println!("\n{}\n", "*".repeat(60));
    
    println!("Tabelas de Símbolos Geradas:\n");
    let mut stdout = io::stdout();
    scope_manager.borrow().print_tables(&mut stdout)?;
    
    println!("\n{}\n", "*".repeat(60));
    
    println!("Código Intermediário Gerado:\n");
    print_code(&ast_root);
    
    println!("\n{}\n", "*".repeat(60));
    println!("Execução finalizada com sucesso!");
    
    Ok(())
}