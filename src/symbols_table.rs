use std::fmt;

#[derive(Debug, Clone, PartialEq)]
pub enum SymbolType {
    Int,
    Float,
    String,
    Function,
    Array(Vec<usize>),
}

#[derive(Debug, Clone)]
pub struct SymbolEntry {
    pub name: String,
    pub symbol_type: SymbolType,
    pub line: usize,
    pub column: usize,
}

impl SymbolEntry {
    pub fn new(name: &str, symbol_type: SymbolType, line: usize, column: usize) -> Self {
        SymbolEntry {
            name: name.to_string(),
            symbol_type,
            line,
            column,
        }
    }
}

impl fmt::Display for SymbolEntry {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(
            f, "{}: {:?} @ {}:{}",
            self.name, self.symbol_type, self.line, self.column
        )
    }
}