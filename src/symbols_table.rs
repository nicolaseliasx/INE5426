// symbols_table.rs
use std::collections::HashMap;
use std::fmt;
use std::rc::Rc;
use crate::errors::SemanticError;
use crate::token::Token;

#[derive(Debug, Clone)]
pub struct TableEntry {
    pub symbol_type: String,
    pub lines: Vec<usize>,
}

pub struct SymbolsTable {
    table: HashMap<String, TableEntry>,
}

impl SymbolsTable {
    pub fn new() -> Self {
        SymbolsTable {
            table: HashMap::new(),
        }
    }

    pub fn add_symbol(&mut self, token: Rc<Token>) {
        self.table.entry(token.lexeme.clone())
            .or_insert(TableEntry {
                symbol_type: String::new(),
                lines: Vec::new(),
            })
            .lines.push(token.line);
    }

    pub fn set_type(&mut self, token: &Token, symbol_type: &str) {
        if let Some(entry) = self.table.get_mut(&token.lexeme) {
            entry.symbol_type = symbol_type.to_string();
        }
    }

    pub fn contains_symbol(&self, lexeme: &str) -> bool {
        self.table.contains_key(lexeme)
    }

    pub fn is_type(&self, token: &Token, symbol_type: &str) -> bool {
        self.table.get(&token.lexeme)
            .map(|entry| entry.symbol_type == symbol_type)
            .unwrap_or(false)
    }

    pub fn get_type(&self, token: &Token) -> Result<String, SemanticError> {
        self.table.get(&token.lexeme)
            .map(|entry| entry.symbol_type.clone())
            .ok_or_else(|| SemanticError::new(format!("Lexeme '{}' not found", token.lexeme)))
    }

    pub fn print(&self, out: &mut impl fmt::Write) -> fmt::Result {
        for (lexeme, entry) in &self.table {
            writeln!(out, "\tSymbol: {}", lexeme)?;
            writeln!(out, "\t\ttype: {}", entry.symbol_type)?;
            write!(out, "\t\tlines: [")?;
            
            let mut first = true;
            for line in &entry.lines {
                if !first {
                    write!(out, ",")?;
                }
                write!(out, "{}", line)?;
                first = false;
            }
            writeln!(out, "]")?;
        }
        Ok(())
    }
}