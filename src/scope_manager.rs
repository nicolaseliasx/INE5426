// scope_manager.rs
use std::collections::HashMap;
use std::rc::Rc;
use std::cell::RefCell;
use std::io::Write;
use crate::token::Token;
use crate::errors::CompilationError;

#[derive(Debug, Clone)]
pub struct SymbolEntry {
    pub name: String,
    pub token: Rc<Token>,
    pub symbol_type: Option<String>,
}

#[derive(Debug, Default)]
pub struct SymbolsTable {
    symbols: HashMap<String, SymbolEntry>,
}

impl SymbolsTable {
    pub fn new() -> Self {
        SymbolsTable {
            symbols: HashMap::new(),
        }
    }

    pub fn add_symbol(&mut self, token: Rc<Token>) {
        let entry = SymbolEntry {
            name: token.lexeme.clone(),
            token: Rc::clone(&token),
            symbol_type: None,
        };
        self.symbols.insert(token.lexeme.clone(), entry);
    }

    pub fn contains_symbol(&self, lexeme: &str) -> bool {
        self.symbols.contains_key(lexeme)
    }

    pub fn set_type(&mut self, token: &Token, symbol_type: &str) {
        if let Some(entry) = self.symbols.get_mut(&token.lexeme) {
            entry.symbol_type = Some(symbol_type.to_string());
        }
    }

    pub fn is_type(&self, token: &Token, symbol_type: &str) -> bool {
        self.symbols.get(&token.lexeme)
            .map(|entry| entry.symbol_type.as_deref() == Some(symbol_type))
            .unwrap_or(false)
    }

    pub fn get_type(&self, token: &Token) -> Option<String> {
        self.symbols.get(&token.lexeme)
            .and_then(|entry| entry.symbol_type.clone())
    }

    pub fn print(&self, out: &mut impl Write) -> std::io::Result<()> {
        for (name, entry) in &self.symbols {
            let symbol_type = entry.symbol_type.as_deref().unwrap_or("unknown");
            writeln!(out, "{}: {}", name, symbol_type)?;
        }
        Ok(())
    }
}

#[derive(Debug)]
pub struct ScopeItem {
    pub name: String,
    pub symbols_table: SymbolsTable,
    pub ancestor_scope: Option<Rc<RefCell<ScopeItem>>>,
    pub children_scopes: Vec<Rc<RefCell<ScopeItem>>>,
}

#[derive(Debug, Default)]
pub struct ScopeManager {
    scope_root: Option<Rc<RefCell<ScopeItem>>>,
    current_scope: Option<Rc<RefCell<ScopeItem>>>,
}

impl ScopeManager {
    pub fn new() -> Self {
        ScopeManager::default()
    }

    pub fn open_scope(&mut self, scope_name: &str) {
        let new_scope = Rc::new(RefCell::new(ScopeItem {
            name: scope_name.to_string(),
            symbols_table: SymbolsTable::new(),
            ancestor_scope: self.current_scope.clone(),
            children_scopes: Vec::new(),
        }));

        if self.scope_root.is_none() {
            self.scope_root = Some(new_scope.clone());
        }

        if let Some(current) = &self.current_scope {
            current.borrow_mut().children_scopes.push(new_scope.clone());
        }

        self.current_scope = Some(new_scope);
    }

    pub fn close_scope(&mut self) {
        if let Some(current) = &self.current_scope {
            let ancestor = current.borrow().ancestor_scope.clone();
            self.current_scope = ancestor;
        }
    }

    pub fn add_symbol(&mut self, token: Rc<Token>) -> Result<(), CompilationError> {
        let current_scope = self.current_scope.as_ref()
            .ok_or_else(|| CompilationError::semantic("No current scope".to_string()))?;
        
        let mut current = current_scope.borrow_mut();
        if current.symbols_table.contains_symbol(&token.lexeme) {
            return Err(CompilationError::semantic(
                format!("{} redeclared at {}:{}", token.lexeme, token.line, token.column)
            ));
        }
        current.symbols_table.add_symbol(Rc::clone(&token));
        Ok(())
    }

    pub fn set_type(&mut self, token: &Token, symbol_type: &str) -> Result<(), CompilationError> {
        let mut scope = self.current_scope.clone();
        while let Some(current) = scope {
            let current_ref = current.borrow();
            if current_ref.symbols_table.contains_symbol(&token.lexeme) {
                drop(current_ref);
                current.borrow_mut().symbols_table.set_type(token, symbol_type);
                return Ok(());
            }
            scope = current_ref.ancestor_scope.clone();
        }
        Err(CompilationError::semantic(
            format!("Cannot assign type to {} at {}:{}", token.lexeme, token.line, token.column)
        ))
    }

    pub fn is_declared(&self, token: &Token) -> bool {
        let mut scope = self.current_scope.clone();
        while let Some(current) = scope {
            let current_ref = current.borrow();
            if current_ref.symbols_table.contains_symbol(&token.lexeme) {
                return true;
            }
            scope = current_ref.ancestor_scope.clone();
        }
        false
    }

    pub fn is_type(&self, token: &Token, symbol_type: &str) -> bool {
        let mut scope = self.current_scope.clone();
        while let Some(current) = scope {
            let current_ref = current.borrow();
            if current_ref.symbols_table.contains_symbol(&token.lexeme) {
                return current_ref.symbols_table.is_type(token, symbol_type);
            }
            scope = current_ref.ancestor_scope.clone();
        }
        false
    }

    pub fn get_type(&self, token: &Token) -> Option<String> {
        let mut scope = self.current_scope.clone();
        while let Some(current) = scope {
            let current_ref = current.borrow();
            if current_ref.symbols_table.contains_symbol(&token.lexeme) {
                return current_ref.symbols_table.get_type(token);
            }
            scope = current_ref.ancestor_scope.clone();
        }
        None
    }

    pub fn is_in_scope(&self, scope_type: &str) -> bool {
        let mut scope = self.current_scope.clone();
        while let Some(current) = scope {
            let current_ref = current.borrow();
            if current_ref.name == scope_type {
                return true;
            }
            scope = current_ref.ancestor_scope.clone();
        }
        false
    }

    pub fn print_tables(&self, out: &mut impl Write) -> std::io::Result<()> {
        let mut stack = vec![];
        if let Some(root) = &self.scope_root {
            stack.push(root.clone());
        }

        while let Some(scope) = stack.pop() {
            let scope_ref = scope.borrow();
            writeln!(out, "\nSymbol Table: {}", scope_ref.name)?;
            scope_ref.symbols_table.print(out)?;
            
            for child in &scope_ref.children_scopes {
                stack.push(child.clone());
            }
        }
        Ok(())
    }
}