// parser_stack_item.rs
use crate::syntax_tree::SyntaxNode;
use crate::scope_manager::ScopeManager;
use crate::errors::CompilationError;
use std::rc::Rc;
use std::cell::RefCell;

pub type SemanticActionFn = fn(
    Option<Rc<RefCell<SyntaxNode>>>, 
    Rc<RefCell<ScopeManager>>
) -> Result<(), CompilationError>;

#[derive(Debug, Clone, PartialEq)]
pub enum StackItemType {
    Symbol,
    Action,
}

#[derive(Clone)]
pub struct StackItem {
    pub item_type: StackItemType,
    pub value: StackItemValue,
    pub ast_node: Option<Rc<RefCell<SyntaxNode>>>,
    pub ast_ancestor: Option<Rc<RefCell<SyntaxNode>>>,
}

#[derive(Clone)]
pub enum StackItemValue {
    Symbol(String),
    Action(SemanticActionFn),
}

impl StackItem {
    pub fn new_symbol(symbol: &str) -> Self {
        StackItem {
            item_type: StackItemType::Symbol,
            value: StackItemValue::Symbol(symbol.to_string()),
            ast_node: None,
            ast_ancestor: None,
        }
    }

    pub fn new_action(action: SemanticActionFn) -> Self {
        StackItem {
            item_type: StackItemType::Action,
            value: StackItemValue::Action(action),
            ast_node: None,
            ast_ancestor: None,
        }
    }

    pub fn set_ast_node_ancestry(
        &mut self, 
        node: Option<Rc<RefCell<SyntaxNode>>>,
        ancestor: Option<Rc<RefCell<SyntaxNode>>>
    ) {
        self.ast_node = node;
        self.ast_ancestor = ancestor;
    }

    pub fn is_epsilon(&self) -> bool {
        if let StackItemValue::Symbol(s) = &self.value {
            s == "ε"
        } else {
            false
        }
    }
}