use std::rc::Rc;
use std::cell::RefCell;
use crate::parser_table::PARSE_TABLE;
use crate::parser_stack_item::{StackItem, StackItemType, StackItemValue};
use crate::syntax_tree::SyntaxNode;
use crate::token::Token;
use crate::errors::CompilationError;
use crate::scope_manager::ScopeManager;

pub struct SyntaxAnalyzer {
    parse_stack: Vec<StackItem>,
    scope_manager: Rc<RefCell<ScopeManager>>,
    ast_root: Option<Rc<RefCell<SyntaxNode>>>,
}

impl SyntaxAnalyzer {
    pub fn new(scope_manager: Rc<RefCell<ScopeManager>>) -> Self {
        SyntaxAnalyzer {
            parse_stack: Vec::new(),
            scope_manager,
            ast_root: None,
        }
    }

    pub fn initialize(&mut self) {
        self.parse_stack.clear();  // Limpar pilha antes de inicializar
        
        // Empilhar símbolo de fim de entrada
        self.parse_stack.push(StackItem::new_symbol("$"));
        
        // Criar item para o símbolo inicial
        let mut program_item = StackItem::new_symbol("PROGRAM");
        
        // Criar nó raiz da AST
        let root_node = Rc::new(RefCell::new(SyntaxNode::new("PROGRAM")));
        self.ast_root = Some(Rc::clone(&root_node));
        
        // Configurar ancestralidade
        program_item.set_ast_node_ancestry(Some(root_node), None);
        
        // Empilhar símbolo inicial
        self.parse_stack.push(program_item);
    }

    pub fn process_token(&mut self, token: &Token) -> Result<(), CompilationError> {
        let token_type = token.token_type.clone();
        let token_lexeme = token.lexeme.clone();

        loop {
            let stack_top = match self.parse_stack.pop() {
                Some(item) => item,
                None => {
                    return Err(CompilationError::syntax_simple(
                        format!("Unexpected end of stack at token '{}'", token_lexeme),
                        token.line,
                        token.column,
                        vec![]
                    ));
                }
            };

            match stack_top.item_type {
                StackItemType::Action => {
                    if let StackItemValue::Action(action_fn) = stack_top.value {
                        let node = stack_top.ast_node.clone();
                        action_fn(node, Rc::clone(&self.scope_manager))?;
                    }
                }
                StackItemType::Symbol => {
                    let symbol = if let StackItemValue::Symbol(s) = stack_top.value {
                        s
                    } else {
                        unreachable!()
                    };

                    if symbol == token_type {
                        // Terminal corresponde: configurar token se não for EOF
                        if token_lexeme != "$" {
                            if let Some(node) = stack_top.ast_node {
                                node.borrow_mut().set_token(Rc::new(token.clone()));
                            }
                        }
                        return Ok(());
                    }
                    
                    // Verificar se é um não-terminal
                    let production = match PARSE_TABLE.get(&symbol)
                        .and_then(|row| row.get(&token_type)) 
                    {
                        Some(prod) => prod,
                        None => {
                            return Err(CompilationError::syntax_simple(
                                format!("Unexpected token '{}'", token_lexeme),
                                token.line,
                                token.column,
                                vec![] // TODO: Preencher com tokens esperados
                            ));
                        }
                    };

                    // Processar produção
                    let mut items_to_push = Vec::new();
                    
                    for item in production.iter() {
                        if item.is_epsilon() {
                            continue;
                        }
                        
                        let mut new_item = item.clone();
                        
                        // Configurar ancestralidade para símbolos
                        if let StackItemType::Symbol = new_item.item_type {
                            if let Some(parent_node) = &stack_top.ast_node {
                                let new_node = Rc::new(RefCell::new(SyntaxNode::new(
                                    new_item.get_symbol().unwrap_or_default()
                                )));
                                parent_node.borrow_mut().add_child(new_node.clone());
                                new_item.set_ast_node_ancestry(Some(new_node), None);
                            }
                        } else if let StackItemType::Action = new_item.item_type {
                            // Ações herdam o ancestral
                            new_item.set_ast_node_ancestry(None, stack_top.ast_node.clone());
                        }
                        
                        items_to_push.push(new_item);
                    }
                    
                    // Empilhar em ordem reversa
                    for item in items_to_push.into_iter().rev() {
                        self.parse_stack.push(item);
                    }
                }
            }
        }
    }

    pub fn is_complete(&self) -> bool {
        self.parse_stack.is_empty()
    }

    pub fn ast_root(&self) -> Rc<RefCell<SyntaxNode>> {
        self.ast_root.as_ref().expect("AST root not set").clone()
    }
}

// Adicionar método get_symbol ao StackItem
impl StackItem {
    pub fn get_symbol(&self) -> Option<&str> {
        if let StackItemValue::Symbol(s) = &self.value {
            Some(s)
        } else {
            None
        }
    }
}