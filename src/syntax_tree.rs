use std::cell::RefCell;
use std::rc::Rc;
use std::sync::atomic::{AtomicI32, Ordering};

// Placeholder para o tipo Token
pub struct Token;

#[derive(Debug, Default)]
pub struct ExpressionAttributes {
    pub array_counter: i32,
    pub array_vars: Rc<RefCell<Vec<String>>>,
    pub operation: char,
    pub expr_node: Option<Rc<RefCell<ExpressionNode>>>,
}

#[derive(Debug, Default)]
pub struct DeclarationData {
    pub base_type: String,
    pub resolved_type: String,
}

#[derive(Debug, Default)]
pub struct CodegenState {
    pub temp_var: String,
    pub entry_label: String,
    pub return_label: String,
    pub return_value: String,
    pub param_count: i32,
}

#[derive(Debug)]
pub struct ExpressionNode {
    pub op: char,
    pub data_type: String,
    pub value: String,
    pub left: Option<Rc<RefCell<ExpressionNode>>>,
    pub right: Option<Rc<RefCell<ExpressionNode>>>,
}

impl ExpressionNode {
    pub fn leaf(op: char, data_type: String, value: String) -> Self {
        ExpressionNode {
            op,
            data_type,
            value,
            left: None,
            right: None,
        }
    }

    pub fn unary(op: char, value: String, child: Rc<RefCell<ExpressionNode>>) -> Self {
        let data_type = child.borrow().data_type.clone();
        ExpressionNode {
            op,
            data_type,
            value,
            left: Some(child),
            right: None,
        }
    }

    pub fn binary(
        op: char,
        value: String,
        left: Rc<RefCell<ExpressionNode>>,
        right: Rc<RefCell<ExpressionNode>>,
    ) -> Result<Self, String> {
        let left_type = left.borrow().data_type.clone();
        let right_type = right.borrow().data_type.clone();

        if left_type == right_type {
            Ok(ExpressionNode {
                op,
                data_type: left_type,
                value,
                left: Some(left),
                right: Some(right),
            })
        } else {
            Err(format!(
                "Type mismatch in expression: operation '{}' between {} and {}",
                op, left_type, right_type
            ))
        }
    }
}

#[derive(Debug)]
pub struct SyntaxNode {
    pub node_type: String,
    pub token: Option<Rc<Token>>,
    pub child_nodes: Vec<Rc<RefCell<SyntaxNode>>>,
    pub generated_code: Rc<RefCell<Vec<String>>>,
    pub next_label: String,
    pub loop_exit_label: String,
    pub decl_data: DeclarationData,
    pub codegen_state: CodegenState,
    pub expr_attrs: ExpressionAttributes,
}

impl SyntaxNode {
    pub fn new(node_type: String) -> Self {
        SyntaxNode {
            node_type,
            token: None,
            child_nodes: Vec::new(),
            generated_code: Rc::new(RefCell::new(Vec::new())),
            next_label: String::new(),
            loop_exit_label: String::new(),
            decl_data: DeclarationData::default(),
            codegen_state: CodegenState::default(),
            expr_attrs: ExpressionAttributes::default(),
        }
    }

    pub fn with_children(node_type: String, child_nodes: Vec<Rc<RefCell<SyntaxNode>>>) -> Self {
        SyntaxNode {
            node_type,
            token: None,
            child_nodes,
            generated_code: Rc::new(RefCell::new(Vec::new())),
            next_label: String::new(),
            loop_exit_label: String::new(),
            decl_data: DeclarationData::default(),
            codegen_state: CodegenState::default(),
            expr_attrs: ExpressionAttributes::default(),
        }
    }

    pub fn set_token(&mut self, token: Rc<Token>) {
        self.token = Some(token);
    }

    pub fn add_child(&mut self, node: Rc<RefCell<SyntaxNode>>) {
        self.child_nodes.push(node);
    }

    pub fn set_children(&mut self, new_children: Vec<Rc<RefCell<SyntaxNode>>>) {
        self.child_nodes = new_children;
    }

    pub fn create_label() -> String {
        static LABEL_COUNTER: AtomicI32 = AtomicI32::new(0);
        let count = LABEL_COUNTER.fetch_add(1, Ordering::Relaxed);
        format!("L{}", count)
    }
}