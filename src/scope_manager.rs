use std::cell::RefCell;
use std::collections::HashMap;
use std::rc::Rc;
use crate::token::Token;
use crate::errors::{CompilationError, SemanticErrorType};
use crate::symbols_table::{SymbolEntry, SymbolType};

#[derive(Debug)]
pub struct ScopeItem {
    name: String,
    symbols: HashMap<String, SymbolEntry>,
    ancestor_scope: Option<Rc<RefCell<ScopeItem>>>,
    children_scopes: Vec<Rc<RefCell<ScopeItem>>>,
}

impl ScopeItem {
    pub fn new(name: &str, ancestor: Option<Rc<RefCell<ScopeItem>>>) -> Self {
        ScopeItem {
            name: name.to_string(),
            symbols: HashMap::new(),
            ancestor_scope: ancestor,
            children_scopes: Vec::new(),
        }
    }

    pub fn is_in_table(&self, name: &str) -> bool {
        self.symbols.contains_key(name)
    }

    pub fn add_symbol(&mut self, token: &Token) {
        self.symbols.insert(
            token.lexeme.clone(),
            SymbolEntry::new(token.token_type.clone(), token.line, token.column)
        );
    }

    pub fn add_type(&mut self, name: &str, symbol_type: SymbolType) -> bool {
        if let Some(entry) = self.symbols.get_mut(name) {
            entry.symbol_type = symbol_type;
            true
        } else {
            false
        }
    }

    pub fn is_type(&self, name: &str, symbol_type: SymbolType) -> bool {
        self.symbols.get(name)
            .map(|e| e.symbol_type == symbol_type)
            .unwrap_or(false)
    }

    pub fn get_type(&self, name: &str) -> Option<SymbolType> {
        self.symbols.get(name).map(|e| e.symbol_type.clone())
    }

    pub fn print(&self, out: &mut impl std::io::Write) -> std::io::Result<()> {
        writeln!(out, "Symbol Table type: {}", self.name)?;
        for (name, entry) in &self.symbols {
            writeln!(out, "  {}: {:?}", name, entry)?;
        }
        Ok(())
    }
}

#[derive(Debug)]
pub struct ScopeManager {
    current_scope: Rc<RefCell<ScopeItem>>,
    scope_root: Rc<RefCell<ScopeItem>>,
}

impl ScopeManager {
    pub fn new() -> Self {
        let root_scope = Rc::new(RefCell::new(ScopeItem::new("global", None)));
        ScopeManager {
            current_scope: Rc::clone(&root_scope),
            scope_root: root_scope,
        }
    }

    pub fn open_scope(&mut self, scope_name: &str) {
        let new_scope = Rc::new(RefCell::new(ScopeItem::new(
            scope_name,
            Some(Rc::clone(&self.current_scope))
        )));
        
        self.current_scope.borrow_mut().children_scopes.push(Rc::clone(&new_scope));
        self.current_scope = new_scope;
    }

    pub fn close_scope(&mut self) {
        if let Some(ancestor) = &self.current_scope.borrow().ancestor_scope {
            self.current_scope = Rc::clone(ancestor);
        }
    }

    pub fn add_symbol(&mut self, token: &Token) -> Result<(), CompilationError> {
        let name = &token.lexeme;
        let mut current_scope = self.current_scope.borrow_mut();
        
        if current_scope.is_in_table(name) {
            return Err(CompilationError::semantic_simple(
                SemanticErrorType::Redeclaration,
                format!("{} redeclared", name),
                token.line,
                token.column,
            ));
        }
        
        current_scope.add_symbol(token);
        Ok(())
    }

    pub fn add_type(&mut self, token: &Token, symbol_type: SymbolType) -> Result<(), CompilationError> {
        let name = &token.lexeme;
        let mut scope = Some(Rc::clone(&self.current_scope));
        
        while let Some(current) = scope {
            let mut current_borrow = current.borrow_mut();
            if current_borrow.is_in_table(name) {
                current_borrow.add_type(name, symbol_type);
                return Ok(());
            }
            
            scope = current_borrow.ancestor_scope.as_ref().map(|a| Rc::clone(a));
        }
        
        Err(CompilationError::semantic_simple(
            SemanticErrorType::UndeclaredSymbol,
            format!("Cannot assign type to undeclared symbol '{}'", name),
            token.line,
            token.column,
        ))
    }

    pub fn is_declared(&self, token: &Token) -> bool {
        let name = &token.lexeme;
        let mut scope = Some(Rc::clone(&self.current_scope));
        
        while let Some(current) = scope {
            let current_borrow = current.borrow();
            if current_borrow.is_in_table(name) {
                return true;
            }
            
            scope = current_borrow.ancestor_scope.as_ref().map(|a| Rc::clone(a));
        }
        
        false
    }

    pub fn is_type(&self, token: &Token, symbol_type: SymbolType) -> bool {
        let name = &token.lexeme;
        let mut scope = Some(Rc::clone(&self.current_scope));
        
        while let Some(current) = scope {
            let current_borrow = current.borrow();
            if current_borrow.is_in_table(name) {
                return current_borrow.is_type(name, symbol_type);
            }
            
            scope = current_borrow.ancestor_scope.as_ref().map(|a| Rc::clone(a));
        }
        
        false
    }

    pub fn is_in_scope(&self, scope_type: &str) -> bool {
        let mut scope = Some(Rc::clone(&self.current_scope));
        
        while let Some(current) = scope {
            let current_borrow = current.borrow();
            if current_borrow.name == scope_type {
                return true;
            }
            
            scope = current_borrow.ancestor_scope.as_ref().map(|a| Rc::clone(a));
        }
        
        false
    }

    pub fn get_type(&self, token: &Token) -> Option<SymbolType> {
        let name = &token.lexeme;
        let mut scope = Some(Rc::clone(&self.current_scope));
        
        while let Some(current) = scope {
            let current_borrow = current.borrow();
            if let Some(symbol_type) = current_borrow.get_type(name) {
                return Some(symbol_type);
            }
            
            scope = current_borrow.ancestor_scope.as_ref().map(|a| Rc::clone(a));
        }
        
        None
    }

    pub fn print_tables(&self, out: &mut impl std::io::Write) -> std::io::Result<()> {
        let mut stack = vec![Rc::clone(&self.scope_root)];
        
        while let Some(scope) = stack.pop() {
            let scope_ref = scope.borrow();
            scope_ref.print(out)?;
            
            for child in scope_ref.children_scopes.iter().rev() {
                stack.push(Rc::clone(child));
            }
        }
        
        Ok(())
    }
}