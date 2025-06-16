// lexical_analyzer.rs
use std::collections::{HashMap, HashSet};
use std::fs::File;
use std::io::{self, BufRead, BufReader, Read, Seek, SeekFrom};
use std::path::Path;
use std::sync::atomic::{AtomicUsize, Ordering};
use crate::errors::{CompilationError, ErrorReporter};
use crate::finite_state_machine::{FiniteStateMachine, MachineStatus};
use crate::token::Token;

static TOKEN_ID_COUNTER: AtomicUsize = AtomicUsize::new(0);

pub struct LexicalAnalyzer {
    reader: BufReader<File>,
    keywords: HashSet<String>,
    token_identifiers: Vec<FiniteStateMachine>,
    current_line: usize,
    current_column: usize,
    token_line_offset: usize,
    in_token: bool,
    current_byte_pos: u64,
    token_id_by_lexeme: HashMap<String, String>,
}

struct PossibleToken {
    token: Token,
    start: u64,
}

impl LexicalAnalyzer {
    pub fn new(file_path: &Path, keywords: Vec<&str>) -> Result<Self, CompilationError> {
        let file = File::open(file_path).map_err(|e| {
            CompilationError::lexical_simple(
                format!("Failed to open file: {}", e),
                0,
                0,
            )
        })?;
        
        let reader = BufReader::new(file);
        let keywords_set = keywords.iter().map(|s| s.to_string()).collect();
        
        // Mapeamento de lexemas para tipos de token
        let token_id_by_lexeme: HashMap<String, String> = [
            ("+", "PLUS"),
            ("-", "MINUS"),
            ("*", "MULTIPLICATION"),
            ("/", "DIVISION"),
            ("%", "MODULUS"),
            ("=", "ASSIGN"),
            ("(", "OPEN_PARENTHESIS"),
            (")", "CLOSE_PARENTHESIS"),
            ("{", "OPEN_BRACE"),
            ("}", "CLOSE_BRACE"),
            ("[", "OPEN_BRACKET"),
            ("]", "CLOSE_BRACKET"),
            (";", "SEMICOLON"),
            (",", "COMMA"),
            (">", "GT"),
            ("<", "LT"),
            (">=", "GE"),
            ("<=", "LE"),
            ("==", "EQ"),
            ("!=", "NE"),
        ]
        .iter()
        .map(|&(lexeme, token_type)| (lexeme.to_string(), token_type.to_string()))
        .collect();
        
        let token_identifiers = vec![
            Self::create_identifier_fsm(),
            Self::create_integer_fsm(),
            Self::create_float_fsm(),
            Self::create_math_operator_fsm(),
            Self::create_relop_fsm(),
            Self::create_onechar_fsm(),
            Self::create_string_const_fsm(),
        ];
        
        Ok(LexicalAnalyzer {
            reader,
            keywords: keywords_set,
            token_identifiers,
            current_line: 1,
            current_column: 0,
            token_line_offset: 0,
            in_token: false,
            current_byte_pos: 0,
            token_id_by_lexeme,
        })
    }

    fn create_identifier_fsm() -> FiniteStateMachine {
        let mut transitions = HashMap::new();
        transitions.insert("0".to_string(), Box::new(|c, _| {
            if c.is_alphabetic() || c == '_' { "1".to_string() } else { "dead".to_string() }
        }) as Box<dyn Fn(char, bool) -> String>);
        
        transitions.insert("1".to_string(), Box::new(|c, _| {
            if c.is_alphanumeric() || c == '_' { "1".to_string() } else { "final".to_string() }
        }));

        let final_states: HashSet<String> = ["final".to_string()].iter().cloned().collect();
        let cursor_back_states: HashSet<String> = ["final".to_string()].iter().cloned().collect();
        
        FiniteStateMachine::new(
            transitions,
            "0".to_string(),
            final_states,
            cursor_back_states,
        )
    }

    fn create_integer_fsm() -> FiniteStateMachine {
        let mut transitions = HashMap::new();
        transitions.insert("0".to_string(), Box::new(|c, _| {
            if c.is_ascii_digit() { "1".to_string() } else { "dead".to_string() }
        }) as Box<dyn Fn(char, bool) -> String>);
        
        transitions.insert("1".to_string(), Box::new(|c, _| {
            if c.is_ascii_digit() { "1".to_string() } else { "final".to_string() }
        }));

        let final_states: HashSet<String> = ["final".to_string()].iter().cloned().collect();
        let cursor_back_states: HashSet<String> = ["final".to_string()].iter().cloned().collect();
        
        FiniteStateMachine::new(
            transitions,
            "0".to_string(),
            final_states,
            cursor_back_states,
        )
    }

    fn create_float_fsm() -> FiniteStateMachine {
        let mut transitions = HashMap::new();
        transitions.insert("0".to_string(), Box::new(|c, _| {
            if c.is_ascii_digit() { "1".to_string() } else if c == '.' { "2".to_string() } else { "dead".to_string() }
        }));
        
        transitions.insert("1".to_string(), Box::new(|c, _| {
            if c.is_ascii_digit() { "1".to_string() } else if c == '.' { "2".to_string() } else if c == 'e' { "4".to_string() } else { "final".to_string() }
        }));
        
        transitions.insert("2".to_string(), Box::new(|c, _| {
            if c.is_ascii_digit() { "3".to_string() } else { "dead".to_string() }
        }));
        
        transitions.insert("3".to_string(), Box::new(|c, _| {
            if c.is_ascii_digit() { "3".to_string() } else if c == 'e' { "4".to_string() } else { "final".to_string() }
        }));
        
        transitions.insert("4".to_string(), Box::new(|c, _| {
            if c == '+' || c == '-' { "5".to_string() } else if c.is_ascii_digit() { "6".to_string() } else { "dead".to_string() }
        }));
        
        transitions.insert("5".to_string(), Box::new(|c, _| {
            if c.is_ascii_digit() { "6".to_string() } else { "dead".to_string() }
        }));
        
        transitions.insert("6".to_string(), Box::new(|c, _| {
            if c.is_ascii_digit() { "6".to_string() } else { "final".to_string() }
        }));

        let final_states: HashSet<String> = ["final".to_string()].iter().cloned().collect();
        let cursor_back_states: HashSet<String> = ["final".to_string()].iter().cloned().collect();
        
        FiniteStateMachine::new(
            transitions,
            "0".to_string(),
            final_states,
            cursor_back_states,
        )
    }

    fn create_math_operator_fsm() -> FiniteStateMachine {
        let mut transitions = HashMap::new();
        transitions.insert("0".to_string(), Box::new(|c, _| {
            match c {
                '+' | '-' | '*' | '/' | '%' | '=' => "final".to_string(),
                _ => "dead".to_string(),
            }
        }));

        let final_states: HashSet<String> = ["final".to_string()].iter().cloned().collect();
        let cursor_back_states: HashSet<String> = HashSet::new();
        
        FiniteStateMachine::new(
            transitions,
            "0".to_string(),
            final_states,
            cursor_back_states,
        )
    }

    fn create_relop_fsm() -> FiniteStateMachine {
        let mut transitions = HashMap::new();
        transitions.insert("0".to_string(), Box::new(|c, _| {
            match c {
                '>' | '<' => "1".to_string(),
                '=' | '!' => "2".to_string(),
                _ => "dead".to_string(),
            }
        }));
        
        transitions.insert("1".to_string(), Box::new(|c, _| {
            if c == '=' { "final".to_string() } else { "final".to_string() }
        }));
        
        transitions.insert("2".to_string(), Box::new(|c, _| {
            if c == '=' { "final".to_string() } else { "dead".to_string() }
        }));

        let final_states: HashSet<String> = ["final".to_string()].iter().cloned().collect();
        let cursor_back_states: HashSet<String> = ["final".to_string()].iter().cloned().collect();
        
        FiniteStateMachine::new(
            transitions,
            "0".to_string(),
            final_states,
            cursor_back_states,
        )
    }

    fn create_onechar_fsm() -> FiniteStateMachine {
        let mut transitions = HashMap::new();
        transitions.insert("0".to_string(), Box::new(|c, _| {
            match c {
                '(' | ')' | '{' | '}' | '[' | ']' | ';' | ',' => "final".to_string(),
                _ => "dead".to_string(),
            }
        }));

        let final_states: HashSet<String> = ["final".to_string()].iter().cloned().collect();
        let cursor_back_states: HashSet<String> = HashSet::new();
        
        FiniteStateMachine::new(
            transitions,
            "0".to_string(),
            final_states,
            cursor_back_states,
        )
    }

    fn create_string_const_fsm() -> FiniteStateMachine {
        let mut transitions = HashMap::new();
        transitions.insert("0".to_string(), Box::new(|c, _| {
            if c == '"' { "1".to_string() } else { "dead".to_string() }
        }));
        
        transitions.insert("1".to_string(), Box::new(|c, _| {
            if c == '"' { "final".to_string() } else { "1".to_string() }
        }));

        let final_states: HashSet<String> = ["final".to_string()].iter().cloned().collect();
        let cursor_back_states: HashSet<String> = ["final".to_string()].iter().cloned().collect();
        
        FiniteStateMachine::new(
            transitions,
            "0".to_string(),
            final_states,
            cursor_back_states,
        )
    }

    fn get_token_id(&self, id: &str, lexeme: &str) -> String {
        if id == "IDENT" && self.keywords.contains(lexeme) {
            return lexeme.to_uppercase();
        }
        
        if id == "MATHOP" || id == "ONECHAR" || id == "RELOP" {
            if let Some(token_type) = self.token_id_by_lexeme.get(lexeme) {
                return token_type.clone();
            }
        }
        
        id.to_string()
    }

    fn get_longest_token(&self, possible_tokens: &[PossibleToken]) -> &PossibleToken {
        possible_tokens
            .iter()
            .max_by_key(|t| t.token.lexeme.len())
            .expect("Non-empty candidate list")
    }

    fn reset_machines(&mut self) {
        for machine in &mut self.token_identifiers {
            machine.reset();
        }
        self.in_token = false;
        self.token_line_offset = 0;
    }

    pub fn next_token(&mut self) -> Result<Option<Token>, CompilationError> {
        let mut possible_tokens = Vec::new();
        let mut buffer = [0u8; 1];
        
        loop {
            let start_byte = self.current_byte_pos;
            let start_line = self.current_line;
            let start_col = self.current_column;
            
            // Ler um caractere
            let bytes_read = match self.reader.read(&mut buffer) {
                Ok(0) => break, // EOF
                Ok(n) => n,
                Err(e) => return Err(CompilationError::lexical_simple(
                    format!("Read error: {}", e),
                    start_line,
                    start_col
                )),
            };
            
            let c = buffer[0] as char;
            self.current_byte_pos += bytes_read as u64;
            
            // Atualizar contadores de posição
            if c == '\n' {
                self.current_line += 1;
                self.current_column = 0;
                if self.in_token {
                    self.token_line_offset += 1;
                }
            } else {
                self.current_column += 1;
            }
            
            // Ignorar espaços em branco no início
            if c.is_whitespace() && !self.in_token {
                continue;
            }
            
            let is_eof = false;
            let mut active_machines = 0;
            
            // Processar o caractere em todas as máquinas de estado
            for machine in &mut self.token_identifiers {
                if machine.status() == MachineStatus::Success {
                    continue;
                }
                
                let status = machine.transition(c, is_eof);
                
                match status {
                    MachineStatus::Running => {
                        active_machines += 1;
                    }
                    MachineStatus::Success => {
                        let lexeme = machine.lexeme().to_string();
                        let token_type = self.get_token_id(machine.token_type(), &lexeme);
                        
                        possible_tokens.push(PossibleToken {
                            token: Token {
                                lexeme,
                                token_type,
                                line: start_line,
                                column: start_col,
                            },
                            start: start_byte,
                        });
                    }
                    _ => {}
                }
            }
            
            self.in_token = active_machines > 0;
            
            // Continuar processando enquanto houver máquinas ativas
            if active_machines > 0 {
                continue;
            }
            
            // Processar quando não há máquinas ativas
            if possible_tokens.is_empty() {
                return Err(CompilationError::lexical_simple(
                    format!("Invalid token '{}'", c),
                    start_line,
                    start_col
                ));
            }
            
            // Selecionar o token mais longo
            let chosen_token = self.get_longest_token(&possible_tokens);
            let token = chosen_token.token.clone();
            
            // Atualizar posição do cursor
            let new_position = chosen_token.start + token.lexeme.len() as u64;
            self.reader.seek(SeekFrom::Start(new_position)).map_err(|e| {
                CompilationError::lexical_simple(
                    format!("Seek error: {}", e),
                    token.line,
                    token.column
                )
            })?;
            
            self.current_byte_pos = new_position;
            self.current_column = token.column + token.lexeme.len();
            
            if self.token_line_offset > 0 {
                self.current_line -= self.token_line_offset;
                self.token_line_offset = 0;
            }
            
            self.reset_machines();
            return Ok(Some(token));
        }
        
        // Processar fim de arquivo
        if !possible_tokens.is_empty() {
            let chosen_token = self.get_longest_token(&possible_tokens);
            let token = chosen_token.token.clone();
            self.reset_machines();
            return Ok(Some(token));
        }
        
        Ok(None)
    }
}