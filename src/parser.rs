// parser.rs
use std::collections::HashMap;
use std::rc::Rc;
use std::cell::RefCell;
use crate::errors::CompilationError;
use crate::parser_stack_item::{StackItem, StackItemType, StackItemValue};
use crate::parse_table::PARSE_TABLE;
use crate::syntax_tree::SyntaxNode;
use crate::token::Token;
use crate::scope_manager::ScopeManager;

type ParseTableRef = &'static HashMap<&'static str, HashMap<&'static str, &'static Vec<StackItem>>>;

pub struct SyntaxAnalyzer {
    initial_symbol: &'static str,
    ast_root: Rc<RefCell<SyntaxNode>>,
    scope_manager: Rc<RefCell<ScopeManager>>,
    parse_table: ParseTableRef,
    stack: Vec<StackItem>,
}

impl SyntaxAnalyzer {
    pub fn new(scope_manager: Rc<RefCell<ScopeManager>>) -> Self {
        let analyzer = SyntaxAnalyzer {
            initial_symbol: "PROGRAM",
            ast_root: Rc::new(RefCell::new(SyntaxNode::new("PROGRAM".to_string()))),
            scope_manager,
            parse_table: &PARSE_TABLE,
            stack: Vec::new(),
        };
        analyzer.init_stack();
        analyzer
    }

    fn init_stack(&mut self) {
        // Símbolo de fim de entrada
        self.stack.push(StackItem::new(StackItemType::Symbol, "$"));
        
        // Símbolo inicial com nó raiz
        let root_item = StackItem::new(
            StackItemType::Symbol,
            StackItemValue::Symbol(self.initial_symbol.to_string())
        );
        self.stack.push(root_item);
    }

    pub fn process_token(&mut self, token: &Token) -> Result<(), CompilationError> {
        while let Some(top) = self.stack.pop() {
            match top.item_type {
                StackItemType::Action => {
                    // Executar ação semântica
                    if let StackItemValue::Action(action) = top.value {
                        action(Rc::clone(&self.ast_root), Rc::clone(&self.scope_manager))?;
                    }
                },
                StackItemType::Symbol => {
                    let symbol = if let StackItemValue::Symbol(s) = &top.value {
                        s
                    } else {
                        return Err(CompilationError::syntax("Invalid stack item type".to_string()));
                    };
                    
                    // Verificar se é o token esperado
                    if symbol == token.token_type {
                        // Associar token ao nó (exceto para EOF)
                        if token.lexeme != "$" {
                            self.associate_token(top, token)?;
                        }
                        return Ok(());
                    }
                    
                    // Expandir não-terminal
                    self.expand_production(&symbol, token)?;
                }
            }
        }
        
        Err(CompilationError::syntax(
            format!("Unexpected token '{}' at {}:{}", token.lexeme, token.line, token.column)
        ))
    }

    fn associate_token(&self, item: StackItem, token: &Token) -> Result<(), CompilationError> {
        if let Some(node) = item.ast_node {
            node.borrow_mut().set_token(Rc::new(token.clone()));
            Ok(())
        } else {
            Err(CompilationError::syntax(
                format!("Missing AST node for token '{}'", token.lexeme)
            ))
        }
    }

    fn expand_production(&mut self, symbol: &str, token: &Token) -> Result<(), CompilationError> {
        let production = self.get_production(symbol, &token.token_type)?;
        let mut new_items = Vec::with_capacity(production.len());
        
        // Processar itens da produção na ordem direta
        for item in production.iter() {
            if item.is_epsilon() {
                continue;
            }
            
            let mut new_item = item.clone();
            
            // Configurar nós AST para símbolos
            if let StackItemType::Symbol = item.item_type {
                let new_node = Rc::new(RefCell::new(SyntaxNode::new(item.symbol().to_string())));
                new_item.set_ast_node(Some(Rc::clone(&new_node)));
                
                // Adicionar como filho do nó atual
                if let Some(parent) = &item.ast_node {
                    parent.borrow_mut().add_child(Rc::clone(&new_node));
                }
            }
            
            new_items.push(new_item);
        }
        
        // Empilhar na ordem inversa
        for item in new_items.into_iter().rev() {
            self.stack.push(item);
        }
        
        Ok(())
    }

    fn get_production(&self, symbol: &str, token_type: &str) -> Result<Vec<StackItem>, CompilationError> {
        self.parse_table.get(symbol)
            .and_then(|inner| inner.get(token_type))
            .map(|items| items.clone())
            .ok_or_else(|| CompilationError::syntax(
                format!("Syntax error: No production for {} -> {}", symbol, token_type)
            ))
    }

    pub fn is_complete(&self) -> bool {
        self.stack.is_empty()
    }

    pub fn ast_root(&self) -> Rc<RefCell<SyntaxNode>> {
        Rc::clone(&self.ast_root)
    }
}