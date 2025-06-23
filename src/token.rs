use crate::finite_state_machine::{FiniteStateMachine, MachineStatus};

#[derive(Debug, Clone, PartialEq)]
pub struct Token {
    pub lexeme: String,
    pub token_type: String,
    pub line: usize,
    pub column: usize,
}

impl Token {
    pub fn new(lexeme: &str, token_type: &str, line: usize, column: usize) -> Self {
        Self {
            lexeme: lexeme.to_string(),
            token_type: token_type.to_string(),
            line,
            column,
        }
    }
}

// Funções auxiliares para caracteres
pub fn is_alpha(ch: char) -> bool {
    ch.is_ascii_alphabetic()
}

pub fn is_numeric(ch: char) -> bool {
    ch.is_ascii_digit()
}

pub fn is_alphanumeric(ch: char) -> bool {
    ch.is_ascii_alphanumeric()
}

pub fn is_whitespace(c: char) -> bool {
    c.is_ascii_whitespace()
}

#[derive(Debug)]
pub struct TokenIdentifier {
    pub token_type: String,
    pub fsm: FiniteStateMachine,
    pub cursor_start: Option<usize>,
    pub start_line: Option<usize>,
    pub start_column: Option<usize>,
}

impl TokenIdentifier {
    pub fn new(token_type: &str, fsm: FiniteStateMachine) -> Self {
        Self {
            token_type: token_type.to_string(),
            fsm,
            cursor_start: None,
            start_line: None,
            start_column: None,
        }
    }

    pub fn reset(&mut self) {
        self.fsm.reset();
        self.cursor_start = None;
        self.start_line = None;
        self.start_column = None;
    }

    pub fn lexeme(&self) -> &str {
        self.fsm.lexeme()
    }

    pub fn status(&self) -> MachineStatus {
        self.fsm.status()
    }

    pub fn has_already_succeeded(&self) -> bool {
        self.fsm.status() == MachineStatus::Success
    }

    pub fn should_rewind(&self) -> bool {
        self.fsm.should_rewind()
    }

    pub fn analyze(
        &mut self,
        c: char,
        is_eof: bool,
        cursor_position: usize,
        line: usize,
        column: usize,
    ) -> MachineStatus {
        let prev_status = self.fsm.status();
        
        // Só processa se não estiver em estado final ou erro
        if prev_status == MachineStatus::Error || prev_status == MachineStatus::Success {
            return prev_status;
        }
        
        // Registra posição inicial no primeiro caractere válido
        if prev_status == MachineStatus::Idle && 
           (self.fsm.transition(c, is_eof) == MachineStatus::Running || 
            self.fsm.status() == MachineStatus::Success) 
        {
            self.cursor_start = Some(cursor_position);
            self.start_line = Some(line);
            self.start_column = Some(column);
        }
        
        self.fsm.status()
    }

    pub fn create_token(&self) -> Option<Token> {
        if self.fsm.status() == MachineStatus::Success {
            Some(Token {
                lexeme: self.lexeme().to_string(),
                token_type: self.token_type.clone(),
                line: self.start_line?,
                column: self.start_column?,
            })
        } else {
            None
        }
    }
}