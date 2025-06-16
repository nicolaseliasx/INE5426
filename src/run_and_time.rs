// run_and_time.rs
use std::process::Command;
use std::time::Instant;
use std::os::unix::process::CommandExt;
use std::path::Path;

fn main() {
    let args: Vec<String> = std::env::args().skip(1).collect();
    
    if args.is_empty() {
        eprintln!("Usage: run_and_time <executable> [args...]");
        std::process::exit(1);
    }

    let executable = &args[0];
    let program_args = &args[1..];
    
    if !Path::new(executable).exists() {
        eprintln!("Error: '{}' does not exist.", executable);
        std::process::exit(1);
    }
    
    if !is_executable(executable) {
        eprintln!("Error: '{}' is not executable.", executable);
        std::process::exit(1);
    }

    println!("Running '{}' with arguments '{:?}'...", executable, program_args);
    
    let start = Instant::now();
    let status = Command::new(executable)
        .args(program_args)
        .exec();
    
    // Se chegou aqui, significa que o exec falhou
    eprintln!("Failed to execute '{}': {}", executable, status);
    std::process::exit(1);
}

fn is_executable(path: &str) -> bool {
    use std::os::unix::fs::PermissionsExt;
    
    if let Ok(metadata) = std::fs::metadata(path) {
        let permissions = metadata.permissions();
        let mode = permissions.mode();
        // Verifica se tem permissão de execução para o proprietário
        (mode & 0o100) != 0
    } else {
        false
    }
}