// expression_tree.rs
use std::rc::Rc;
use std::cell::RefCell;
use crate::syntax_tree::ExpressionNode;

pub struct ExpressionTreeResolver;

impl ExpressionTreeResolver {
    pub fn visualize_tree(node: &Rc<RefCell<ExpressionNode>>, prefix: &str, is_left: bool) {
        let node_ref = node.borrow();
        println!("{}{} {}", 
            prefix, 
            if is_left { "├── " } else { "└── " },
            if node_ref.value == "N" {
                format!("{}", node_ref.op)
            } else {
                node_ref.value.clone()
            }
        );

        let new_prefix = format!("{}{}", prefix, if is_left { "│    " } else { "     " });
        
        if let Some(left) = &node_ref.left {
            Self::visualize_tree(left, &new_prefix, true);
        }
        
        if let Some(right) = &node_ref.right {
            Self::visualize_tree(right, &new_prefix, false);
        }
    }

    pub fn display_tree(root: &Rc<RefCell<ExpressionNode>>) {
        println!("{}", "=".repeat(56));
        println!("{}", root.borrow().data_type);
        Self::visualize_tree(root, "", false);
    }

    pub fn display_error(operation: char, left: &Rc<RefCell<ExpressionNode>>, right: &Rc<RefCell<ExpressionNode>>) {
        println!("{}", "=".repeat(56));
        println!("Error in operation: '{}'", operation);
        println!("Left subtree:");
        Self::visualize_tree(left, "      ", true);
        println!("Right subtree:");
        Self::visualize_tree(right, "      ", false);
    }

    pub fn generate_temp_var() -> String {
        use std::sync::atomic::{AtomicI32, Ordering};
        static COUNTER: AtomicI32 = AtomicI32::new(0);
        let id = COUNTER.fetch_add(1, Ordering::Relaxed);
        format!("${}", id)
    }
}