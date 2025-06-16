// token_identifiers.rs
use std::collections::{HashMap, HashSet};
use std::sync::Arc;
use crate::finite_state_machine::{FiniteStateMachine, MachineStatus};
use crate::token::Token;

pub struct TokenIdentifier {
    pub token_type: String,
    pub fsm: Arc<FiniteStateMachine>,
}

// Funções auxiliares
fn is_alpha(c: char) -> bool {
    c.is_ascii_alphabetic()
}

fn is_numeric(c: char) -> bool {
    c.is_ascii_digit()
}

// Identificador para nomes de variáveis/funções
pub fn ident_token_identifier() -> TokenIdentifier {
    let mut transitions = HashMap::new();
    
    transitions.insert("0".to_string(), Box::new(|c: char, is_eof: bool| -> String {
        if is_eof { return "dead".to_string(); }
        if is_alpha(c) || c == '_' { "1".to_string() } else { "dead".to_string() }
    }) as Box<dyn Fn(char, bool) -> String>);
    
    transitions.insert("1".to_string(), Box::new(|c: char, is_eof: bool| -> String {
        if is_eof { return "final".to_string(); }
        if is_alpha(c) || is_numeric(c) || c == '_' { "1".to_string() } else { "final".to_string() }
    }));
    
    let final_states: HashSet<String> = ["final".to_string()].iter().cloned().collect();
    let cursor_back_states: HashSet<String> = ["final".to_string()].iter().cloned().collect();
    
    TokenIdentifier {
        token_type: "IDENT".to_string(),
        fsm: Arc::new(FiniteStateMachine::new(
            transitions,
            "0".to_string(),
            final_states,
            cursor_back_states,
        )),
    }
}

// Identificador para números inteiros
pub fn integer_token_identifier() -> TokenIdentifier {
    let mut transitions = HashMap::new();
    
    transitions.insert("0".to_string(), Box::new(|c: char, is_eof: bool| -> String {
        if is_eof { return "dead".to_string(); }
        if is_numeric(c) { "1".to_string() } else { "dead".to_string() }
    }));
    
    transitions.insert("1".to_string(), Box::new(|c: char, is_eof: bool| -> String {
        if is_eof { return "final".to_string(); }
        if is_numeric(c) { "1".to_string() }
        else if c == 'e' { "2".to_string() }
        else { "final".to_string() }
    }));
    
    transitions.insert("2".to_string(), Box::new(|c: char, is_eof: bool| -> String {
        if is_eof { return "dead".to_string(); }
        if is_numeric(c) { "3".to_string() }
        else if c == '+' { "4".to_string() }
        else { "dead".to_string() }
    }));
    
    transitions.insert("3".to_string(), Box::new(|c: char, is_eof: bool| -> String {
        if is_eof { return "final".to_string(); }
        if is_numeric(c) { "3".to_string() } else { "final".to_string() }
    }));
    
    transitions.insert("4".to_string(), Box::new(|c: char, is_eof: bool| -> String {
        if is_eof { return "dead".to_string(); }
        if is_numeric(c) { "3".to_string() } else { "dead".to_string() }
    }));
    
    let final_states: HashSet<String> = ["final".to_string()].iter().cloned().collect();
    let cursor_back_states: HashSet<String> = ["final".to_string()].iter().cloned().collect();
    
    TokenIdentifier {
        token_type: "NI".to_string(),
        fsm: Arc::new(FiniteStateMachine::new(
            transitions,
            "0".to_string(),
            final_states,
            cursor_back_states,
        )),
    }
}

// Identificador para números de ponto flutuante
pub fn float_token_identifier() -> TokenIdentifier {
    let mut transitions = HashMap::new();
    
    transitions.insert("0".to_string(), Box::new(|c: char, is_eof: bool| -> String {
        if is_eof { return "dead".to_string(); }
        if c == '.' { "1".to_string() }
        else if is_numeric(c) { "6".to_string() }
        else { "dead".to_string() }
    }));
    
    transitions.insert("1".to_string(), Box::new(|c: char, is_eof: bool| -> String {
        if is_eof { return "dead".to_string(); }
        if is_numeric(c) { "2".to_string() } else { "dead".to_string() }
    }));
    
    transitions.insert("2".to_string(), Box::new(|c: char, is_eof: bool| -> String {
        if is_eof { return "final".to_string(); }
        if is_numeric(c) { "2".to_string() }
        else if c == 'e' { "3".to_string() }
        else { "final".to_string() }
    }));
    
    transitions.insert("3".to_string(), Box::new(|c: char, is_eof: bool| -> String {
        if is_eof { return "dead".to_string(); }
        if is_numeric(c) { "4".to_string() }
        else if c == '+' || c == '-' { "5".to_string() }
        else { "dead".to_string() }
    }));
    
    transitions.insert("4".to_string(), Box::new(|c: char, is_eof: bool| -> String {
        if is_eof { return "final".to_string(); }
        if is_numeric(c) { "4".to_string() } else { "final".to_string() }
    }));
    
    transitions.insert("5".to_string(), Box::new(|c: char, is_eof: bool| -> String {
        if is_eof { return "dead".to_string(); }
        if is_numeric(c) { "4".to_string() } else { "dead".to_string() }
    }));
    
    transitions.insert("6".to_string(), Box::new(|c: char, is_eof: bool| -> String {
        if is_eof { return "dead".to_string(); }
        if is_numeric(c) { "6".to_string() }
        else if c == '.' { "2".to_string() }
        else if c == 'e' { "7".to_string() }
        else { "dead".to_string() }
    }));
    
    transitions.insert("7".to_string(), Box::new(|c: char, is_eof: bool| -> String {
        if is_eof { return "final".to_string(); }
        if c == '-' { "5".to_string() } else { "final".to_string() }
    }));
    
    let final_states: HashSet<String> = ["final".to_string()].iter().cloned().collect();
    let cursor_back_states: HashSet<String> = ["final".to_string()].iter().cloned().collect();
    
    TokenIdentifier {
        token_type: "NPF".to_string(),
        fsm: Arc::new(FiniteStateMachine::new(
            transitions,
            "0".to_string(),
            final_states,
            cursor_back_states,
        )),
    }
}

// Identificador para operadores matemáticos
pub fn math_operator_token_identifier() -> TokenIdentifier {
    let mut transitions = HashMap::new();
    
    transitions.insert("0".to_string(), Box::new(|c: char, is_eof: bool| -> String {
        if is_eof { return "dead".to_string(); }
        if c == '+' || c == '-' || c == '*' || c == '/' || c == '%' || c == '=' {
            "final".to_string()
        } else {
            "dead".to_string()
        }
    }));
    
    let final_states: HashSet<String> = ["final".to_string()].iter().cloned().collect();
    let cursor_back_states: HashSet<String> = HashSet::new();
    
    TokenIdentifier {
        token_type: "MATHOP".to_string(),
        fsm: Arc::new(FiniteStateMachine::new(
            transitions,
            "0".to_string(),
            final_states,
            cursor_back_states,
        )),
    }
}

// Identificador para caracteres únicos
pub fn one_char_token_identifier() -> TokenIdentifier {
    let mut transitions = HashMap::new();
    
    transitions.insert("0".to_string(), Box::new(|c: char, is_eof: bool| -> String {
        if is_eof { return "dead".to_string(); }
        if c == '(' || c == ')' || c == '{' || c == '}' || 
           c == '[' || c == ']' || c == ';' || c == ',' {
            "final".to_string()
        } else {
            "dead".to_string()
        }
    }));
    
    let final_states: HashSet<String> = ["final".to_string()].iter().cloned().collect();
    let cursor_back_states: HashSet<String> = HashSet::new();
    
    TokenIdentifier {
        token_type: "ONECHAR".to_string(),
        fsm: Arc::new(FiniteStateMachine::new(
            transitions,
            "0".to_string(),
            final_states,
            cursor_back_states,
        )),
    }
}

// Identificador para strings constantes
pub fn string_const_token_identifier() -> TokenIdentifier {
    let mut transitions = HashMap::new();
    
    transitions.insert("0".to_string(), Box::new(|c: char, is_eof: bool| -> String {
        if is_eof { return "dead".to_string(); }
        if c == '"' { "1".to_string() } else { "dead".to_string() }
    }));
    
    transitions.insert("1".to_string(), Box::new(|c: char, is_eof: bool| -> String {
        if is_eof { return "dead".to_string(); }
        if c != '"' { "1".to_string() } else { "final".to_string() }
    }));
    
    let final_states: HashSet<String> = ["final".to_string()].iter().cloned().collect();
    let cursor_back_states: HashSet<String> = ["final".to_string()].iter().cloned().collect();
    
    TokenIdentifier {
        token_type: "STRC".to_string(),
        fsm: Arc::new(FiniteStateMachine::new(
            transitions,
            "0".to_string(),
            final_states,
            cursor_back_states,
        )),
    }
}

// Identificador para operadores relacionais
pub fn relop_token_identifier() -> TokenIdentifier {
    let mut transitions = HashMap::new();
    
    transitions.insert("0".to_string(), Box::new(|c: char, is_eof: bool| -> String {
        if is_eof { return "dead".to_string(); }
        if c == '>' || c == '<' { "1".to_string() }
        else if c == '=' || c == '!' { "2".to_string() }
        else { "dead".to_string() }
    }));
    
    transitions.insert("1".to_string(), Box::new(|c: char, is_eof: bool| -> String {
        if is_eof { return "final_1".to_string(); }
        if c == '=' { "final_2".to_string() } else { "final_1".to_string() }
    }));
    
    transitions.insert("2".to_string(), Box::new(|c: char, is_eof: bool| -> String {
        if is_eof { return "dead".to_string(); }
        if c == '=' { "final_2".to_string() } else { "dead".to_string() }
    }));
    
    let final_states: HashSet<String> = ["final_1".to_string(), "final_2".to_string()].iter().cloned().collect();
    let cursor_back_states: HashSet<String> = ["final_1".to_string()].iter().cloned().collect();
    
    TokenIdentifier {
        token_type: "RELOP".to_string(),
        fsm: Arc::new(FiniteStateMachine::new(
            transitions,
            "0".to_string(),
            final_states,
            cursor_back_states,
        )),
    }
}