use std::rc::Rc;
use std::cell::RefCell;
use std::collections::VecDeque;
use crate::errors::CompilationError;
use crate::expression_tree_resolver::{ExpressionTreeResolver, generate_temp_var};
use crate::ast_node::{ASTNode, ExpressionNode, Node, DeclarationData, ExpressionAttributes, CodegenState};
use crate::scope_manager::ScopeManager;
use crate::token::Token;

type AstNodeRef = Rc<RefCell<ASTNode>>;
type ScopeManagerRef = Rc<RefCell<ScopeManager>>;

// Módulo CODE: Geração de código intermediário
pub mod code {
    use super::*;

    pub fn relop_action(
        ast_father_node: AstNodeRef,
        scope_manager: ScopeManagerRef
    ) -> Result<(), CompilationError> {
        let mut ast_father = ast_father_node.borrow_mut();
        let children = ast_father.children.clone();
        let numexpression = children[1].clone();
        let relop = children[0].clone();

        let numexpression_ref = numexpression.borrow();
        let relop_ref = relop.borrow();
        
        ast_father.generated_code.extend(numexpression_ref.generated_code.iter().cloned());
        ast_father.generated_code.push(format!(
            "{} = {} {} {}",
            ast_father.codegen_state.return_value,
            ast_father.codegen_state.return_value,
            relop_ref.token.as_ref().unwrap().lexeme,
            numexpression_ref.codegen_state.return_value
        ));
        Ok(())
    }

    pub fn handle_break(
        ast_father_node: AstNodeRef,
        scope_manager: ScopeManagerRef
    ) -> Result<(), CompilationError> {
        let mut ast_father = ast_father_node.borrow_mut();
        ast_father.generated_code.push(format!("goto {}", ast_father.codegen_state.loop_exit_label));
        Ok(())
    }

    pub fn handle_readstat(
        ast_father_node: AstNodeRef,
        scope_manager: ScopeManagerRef
    ) -> Result<(), CompilationError> {
        let mut ast_father = ast_father_node.borrow_mut();
        let children = ast_father.children.clone();
        let lvalue = children[1].clone();
        let lvalue_ref = lvalue.borrow();
        ast_father.generated_code.push(format!("read {}", lvalue_ref.codegen_state.return_value));
        Ok(())
    }

    pub fn handle_printstat(
        ast_father_node: AstNodeRef,
        scope_manager: ScopeManagerRef
    ) -> Result<(), CompilationError> {
        let mut ast_father = ast_father_node.borrow_mut();
        let children = ast_father.children.clone();
        let expression = children[1].clone();
        let expression_ref = expression.borrow();
        
        ast_father.generated_code.extend(expression_ref.generated_code.iter().cloned());
        ast_father.generated_code.push(format!("print {}", expression_ref.codegen_state.return_value));
        Ok(())
    }

    pub fn get_lvalue_var(
        ast_father_node: AstNodeRef,
        scope_manager: ScopeManagerRef
    ) -> Result<(), CompilationError> {
        let mut ast_father = ast_father_node.borrow_mut();
        let children = ast_father.children.clone();
        let ident = children[0].clone();
        let ident_ref = ident.borrow();
        ast_father.codegen_state.return_value = ident_ref.token.as_ref().unwrap().lexeme.clone();
        Ok(())
    }

    pub fn get_return_val(
        ast_father_node: AstNodeRef,
        scope_manager: ScopeManagerRef
    ) -> Result<(), CompilationError> {
        let mut ast_father = ast_father_node.borrow_mut();
        ast_father.codegen_state.return_value = "$rv".to_string();
        Ok(())
    }

    pub fn handle_return(
        ast_father_node: AstNodeRef,
        scope_manager: ScopeManagerRef
    ) -> Result<(), CompilationError> {
        let mut ast_father = ast_father_node.borrow_mut();
        let children = ast_father.children.clone();
        let returnstat = children[1].clone();
        let returnstat_ref = returnstat.borrow();
        
        ast_father.generated_code.extend(returnstat_ref.generated_code.iter().cloned());
        ast_father.generated_code.push(format!("goto {}", ast_father.codegen_state.return_label));
        Ok(())
    }

    pub fn handle_return2(
        ast_father_node: AstNodeRef,
        scope_manager: ScopeManagerRef
    ) -> Result<(), CompilationError> {
        let mut ast_father = ast_father_node.borrow_mut();
        let children = ast_father.children.clone();
        let lvalue = children[0].clone();
        let lvalue_ref = lvalue.borrow();
        ast_father.generated_code.push(format!("$rv = {}", lvalue_ref.codegen_state.return_value));
        Ok(())
    }

    pub fn get_params(
        ast_father_node: AstNodeRef,
        scope_manager: ScopeManagerRef
    ) -> Result<(), CompilationError> {
        let mut ast_father = ast_father_node.borrow_mut();
        let children = ast_father.children.clone();
        let paramlist = children[1].clone();
        let paramlist_ref = paramlist.borrow();
        ast_father.codegen_state.param_count = paramlist_ref.codegen_state.param_count;
        Ok(())
    }

    pub fn paramlistcall(
        ast_father_node: AstNodeRef,
        scope_manager: ScopeManagerRef
    ) -> Result<(), CompilationError> {
        let mut ast_father = ast_father_node.borrow_mut();
        let children = ast_father.children.clone();
        let ident = children[0].clone();
        let paramlist = children[1].clone();
        
        let ident_ref = ident.borrow();
        let paramlist_ref = paramlist.borrow();
        
        ast_father.codegen_state.param_count = 1 + paramlist_ref.codegen_state.param_count;
        ast_father.generated_code.push(format!("param {}", ident_ref.token.as_ref().unwrap().lexeme));
        ast_father.generated_code.extend(paramlist_ref.generated_code.iter().cloned());
        Ok(())
    }

    pub fn funccall(
        ast_father_node: AstNodeRef,
        scope_manager: ScopeManagerRef
    ) -> Result<(), CompilationError> {
        let mut ast_father = ast_father_node.borrow_mut();
        let children = ast_father.children.clone();
        let ident = children[2].clone();
        let paramlistcall = children[4].clone();
        
        let ident_ref = ident.borrow();
        let paramlistcall_ref = paramlistcall.borrow();
        
        ast_father.codegen_state.return_label = ASTNode::create_label();
        
        ast_father.generated_code.extend(paramlistcall_ref.generated_code.iter().cloned());
        ast_father.generated_code.push(format!("$r = {}", ast_father.codegen_state.return_label));
        ast_father.generated_code.push(format!(
            "call {} {}",
            ident_ref.token.as_ref().unwrap().lexeme,
            paramlistcall_ref.codegen_state.param_count
        ));
        ast_father.generated_code.push(format!("{}:", ast_father.codegen_state.return_label));
        Ok(())
    }

    pub fn goto_main(
        ast_father_node: AstNodeRef,
        scope_manager: ScopeManagerRef
    ) -> Result<(), CompilationError> {
        let mut ast_father = ast_father_node.borrow_mut();
        let children = ast_father.children.clone();
        let funclist = children[0].clone();
        let program = children[1].clone();
        
        let funclist_ref = funclist.borrow();
        let program_ref = program.borrow();
        
        ast_father.generated_code.push("goto MAIN".to_string());
        ast_father.generated_code.extend(funclist_ref.generated_code.iter().cloned());
        ast_father.generated_code.push("MAIN:".to_string());
        ast_father.generated_code.extend(program_ref.generated_code.iter().cloned());
        Ok(())
    }

    pub fn funcdef_h(
        ast_father_node: AstNodeRef,
        scope_manager: ScopeManagerRef
    ) -> Result<(), CompilationError> {
        let mut ast_father = ast_father_node.borrow_mut();
        let children = ast_father.children.clone();
        let ident = children[1].clone();
        let statelist = children[6].clone();
        
        let ident_ref = ident.borrow();
        let mut statelist_ref = statelist.borrow_mut();
        
        ast_father.codegen_state.entry_label = ident_ref.token.as_ref().unwrap().lexeme.clone();
        statelist_ref.codegen_state.return_label = "$r".to_string();
        Ok(())
    }

    pub fn funcdef_s(
        ast_father_node: AstNodeRef,
        scope_manager: ScopeManagerRef
    ) -> Result<(), CompilationError> {
        let mut ast_father = ast_father_node.borrow_mut();
        let children = ast_father.children.clone();
        let statelist = children[6].clone();
        
        let statelist_ref = statelist.borrow();
        
        ast_father.generated_code.push(format!("{}:", ast_father.codegen_state.entry_label));
        ast_father.generated_code.extend(statelist_ref.generated_code.iter().cloned());
        ast_father.generated_code.push(format!("goto {}", statelist_ref.codegen_state.return_label));
        Ok(())
    }

    pub fn empty_else_action(
        ast_father_node: AstNodeRef,
        scope_manager: ScopeManagerRef
    ) -> Result<(), CompilationError> {
        let mut ast_father = ast_father_node.borrow_mut();
        ast_father.codegen_state.entry_label = ast_father.codegen_state.return_label.clone();
        Ok(())
    }

    pub fn else_action_h(
        ast_father_node: AstNodeRef,
        scope_manager: ScopeManagerRef
    ) -> Result<(), CompilationError> {
        let mut ast_father = ast_father_node.borrow_mut();
        let children = ast_father.children.clone();
        let statement = children[2].clone();
        
        let mut statement_ref = statement.borrow_mut();
        statement_ref.codegen_state.return_label = ast_father.codegen_state.return_label.clone();
        ast_father.codegen_state.entry_label = ASTNode::create_label();
        Ok(())
    }

    pub fn else_action_s(
        ast_father_node: AstNodeRef,
        scope_manager: ScopeManagerRef
    ) -> Result<(), CompilationError> {
        let mut ast_father = ast_father_node.borrow_mut();
        let children = ast_father.children.clone();
        let statement = children[2].clone();
        
        let statement_ref = statement.borrow();
        
        ast_father.generated_code.push(format!("{}:", ast_father.codegen_state.entry_label));
        ast_father.generated_code.extend(statement_ref.generated_code.iter().cloned());
        ast_father.generated_code.push(format!("goto {}", statement_ref.codegen_state.return_label));
        Ok(())
    }

    pub fn if_action_s(
        ast_father_node: AstNodeRef,
        scope_manager: ScopeManagerRef
    ) -> Result<(), CompilationError> {
        let mut ast_father = ast_father_node.borrow_mut();
        let children = ast_father.children.clone();
        let expression = children[2].clone();
        let statement = children[5].clone();
        let ifstat = children[7].clone();
        
        let expression_ref = expression.borrow();
        let statement_ref = statement.borrow();
        let ifstat_ref = ifstat.borrow();
        
        ast_father.generated_code.extend(expression_ref.generated_code.iter().cloned());
        ast_father.generated_code.push(format!(
            "if False {} goto {}",
            expression_ref.codegen_state.return_value,
            ifstat_ref.codegen_state.entry_label
        ));
        ast_father.generated_code.extend(statement_ref.generated_code.iter().cloned());
        ast_father.generated_code.push(format!("goto {}", statement_ref.codegen_state.return_label));
        ast_father.generated_code.extend(ifstat_ref.generated_code.iter().cloned());
        Ok(())
    }

    pub fn for_action(
        ast_father_node: AstNodeRef,
        scope_manager: ScopeManagerRef
    ) -> Result<(), CompilationError> {
        let mut ast_father = ast_father_node.borrow_mut();
        let children = ast_father.children.clone();
        let atribstat1 = children[2].clone();
        let expression = children[4].clone();
        let atribstat2 = children[6].clone();
        let statement = children[8].clone();
        
        let atribstat1_ref = atribstat1.borrow();
        let expression_ref = expression.borrow();
        let atribstat2_ref = atribstat2.borrow();
        let statement_ref = statement.borrow();
        
        let label = ASTNode::create_label();
        
        ast_father.generated_code.extend(atribstat1_ref.generated_code.iter().cloned());
        ast_father.generated_code.push(format!("{}:", label));
        ast_father.generated_code.extend(expression_ref.generated_code.iter().cloned());
        ast_father.generated_code.push(format!(
            "if False {} goto {}",
            expression_ref.codegen_state.return_value,
            ast_father.codegen_state.return_label
        ));
        ast_father.generated_code.extend(statement_ref.generated_code.iter().cloned());
        ast_father.generated_code.extend(atribstat2_ref.generated_code.iter().cloned());
        ast_father.generated_code.push(format!("goto {}", label));
        Ok(())
    }

    pub fn inherit_for_next(
        ast_father_node: AstNodeRef,
        scope_manager: ScopeManagerRef
    ) -> Result<(), CompilationError> {
        let mut ast_father = ast_father_node.borrow_mut();
        let children = ast_father.children.clone();
        let statement = children[8].clone();
        
        let mut statement_ref = statement.borrow_mut();
        statement_ref.codegen_state.loop_exit_label = ast_father.codegen_state.return_label.clone();
        Ok(())
    }

    pub fn assign(
        ast_father_node: AstNodeRef,
        scope_manager: ScopeManagerRef
    ) -> Result<(), CompilationError> {
        let mut ast_father = ast_father_node.borrow_mut();
        let children = ast_father.children.clone();
        let lvalue = children[0].clone();
        let atribstat = children[2].clone();
        
        let lvalue_ref = lvalue.borrow();
        let atribstat_ref = atribstat.borrow();
        
        let assign_code = if lvalue_ref.expr_attrs.array_counter > 0 {
            format!(
                "{} = {}",
                lvalue_ref.expr_attrs.expr_node.as_ref().unwrap().borrow().value,
                atribstat_ref.codegen_state.return_value
            )
        } else {
            let ident = lvalue_ref.children[0].borrow();
            format!(
                "{} = {}",
                ident.token.as_ref().unwrap().lexeme,
                atribstat_ref.codegen_state.return_value
            )
        };
        
        ast_father.generated_code.extend(lvalue_ref.generated_code.iter().cloned());
        ast_father.generated_code.extend(atribstat_ref.generated_code.iter().cloned());
        ast_father.generated_code.push(assign_code);
        Ok(())
    }

    pub fn set_expression_val(
        ast_father_node: AstNodeRef,
        scope_manager: ScopeManagerRef
    ) -> Result<(), CompilationError> {
        let mut ast_father = ast_father_node.borrow_mut();
        let children = ast_father.children.clone();
        let expression = children[0].clone();
        
        let expression_ref = expression.borrow();
        ast_father.codegen_state.return_value = expression_ref.codegen_state.return_value.clone();
        ast_father.generated_code = expression_ref.generated_code.clone();
        Ok(())
    }

    pub fn set_label(
        ast_father_node: AstNodeRef,
        scope_manager: ScopeManagerRef
    ) -> Result<(), CompilationError> {
        let mut ast_father = ast_father_node.borrow_mut();
        let children = ast_father.children.clone();
        let statement = children[0].clone();
        
        let mut statement_ref = statement.borrow_mut();
        statement_ref.codegen_state.return_label = ASTNode::create_label();
        Ok(())
    }

    pub fn get_code_from_childs(
        ast_father_node: AstNodeRef,
        scope_manager: ScopeManagerRef
    ) -> Result<(), CompilationError> {
        let mut ast_father = ast_father_node.borrow_mut();
        for child in &ast_father.children {
            let child_ref = child.borrow();
            ast_father.generated_code.extend(child_ref.generated_code.iter().cloned());
        }
        Ok(())
    }

    pub fn get_code_from_childs_2(
        ast_father_node: AstNodeRef,
        scope_manager: ScopeManagerRef
    ) -> Result<(), CompilationError> {
        let mut ast_father = ast_father_node.borrow_mut();
        let children = ast_father.children.clone();
        let statement = children[0].clone();
        let statelist = children[1].clone();
        
        let statement_ref = statement.borrow();
        let statelist_ref = statelist.borrow();
        
        ast_father.generated_code.extend(statement_ref.generated_code.iter().cloned());
        ast_father.generated_code.push(format!("{}:", statement_ref.codegen_state.return_label));
        ast_father.generated_code.extend(statelist_ref.generated_code.iter().cloned());
        Ok(())
    }

    pub fn inherit_next(
        ast_father_node: AstNodeRef,
        scope_manager: ScopeManagerRef
    ) -> Result<(), CompilationError> {
        let ast_father = ast_father_node.borrow();
        for child in &ast_father.children {
            let mut child_ref = child.borrow_mut();
            child_ref.codegen_state.return_label = ast_father.codegen_state.return_label.clone();
            child_ref.codegen_state.loop_exit_label = ast_father.codegen_state.loop_exit_label.clone();
        }
        Ok(())
    }

    pub fn set_final_label(
        ast_father_node: AstNodeRef,
        scope_manager: ScopeManagerRef
    ) -> Result<(), CompilationError> {
        let mut ast_father = ast_father_node.borrow_mut();
        ast_father.codegen_state.return_label = "END".to_string();
        Ok(())
    }

    pub fn gen_final_code(
        ast_father_node: AstNodeRef,
        scope_manager: ScopeManagerRef
    ) -> Result<(), CompilationError> {
        let mut ast_father = ast_father_node.borrow_mut();
        ast_father.generated_code.push(format!("{}:", ast_father.codegen_state.return_label));
        Ok(())
    }

    pub fn print_code(
        ast_father_node: AstNodeRef,
        scope_manager: ScopeManagerRef
    ) -> Result<(), CompilationError> {
        let ast_father = ast_father_node.borrow();
        for code in &ast_father.generated_code {
            println!("{}", code);
        }
        Ok(())
    }
}

// Módulo SCOPE: Gestão de escopos
pub mod scope {
    use super::*;

    pub fn action1(
        ast_father_node: AstNodeRef,
        scope_manager: ScopeManagerRef
    ) -> Result<(), CompilationError> {
        scope_manager.borrow_mut().open_scope("")?;
        Ok(())
    }

    pub fn action2(
        ast_father_node: AstNodeRef,
        scope_manager: ScopeManagerRef
    ) -> Result<(), CompilationError> {
        scope_manager.borrow_mut().close_scope();
        Ok(())
    }

    pub fn action3(
        ast_father_node: AstNodeRef,
        scope_manager: ScopeManagerRef
    ) -> Result<(), CompilationError> {
        let ast_father = ast_father_node.borrow();
        let children = ast_father.children.clone();
        let ident = children[1].clone();
        scope_manager.borrow_mut().add_symbol(ident.borrow().token.as_ref().unwrap().clone())?;
        Ok(())
    }

    pub fn action4(
        ast_father_node: AstNodeRef,
        scope_manager: ScopeManagerRef
    ) -> Result<(), CompilationError> {
        scope_manager.borrow_mut().open_scope("function")?;
        Ok(())
    }

    pub fn action5(
        ast_father_node: AstNodeRef,
        scope_manager: ScopeManagerRef
    ) -> Result<(), CompilationError> {
        scope_manager.borrow_mut().open_scope("if")?;
        Ok(())
    }

    pub fn action6(
        ast_father_node: AstNodeRef,
        scope_manager: ScopeManagerRef
    ) -> Result<(), CompilationError> {
        scope_manager.borrow_mut().open_scope("else")?;
        Ok(())
    }

    pub fn action7(
        ast_father_node: AstNodeRef,
        scope_manager: ScopeManagerRef
    ) -> Result<(), CompilationError> {
        scope_manager.borrow_mut().open_scope("for")?;
        Ok(())
    }
}

// Módulo DEC: Declarações de tipos
pub mod dec {
    use super::*;

    pub fn action1(
        ast_father_node: AstNodeRef,
        scope_manager: ScopeManagerRef
    ) -> Result<(), CompilationError> {
        let ast_father = ast_father_node.borrow();
        let children = ast_father.children.clone();
        let ident = children[1].clone();
        scope_manager.borrow_mut().set_type(
            ident.borrow().token.as_ref().unwrap(),
            "function"
        )?;
        Ok(())
    }

    pub fn action2(
        ast_father_node: AstNodeRef,
        scope_manager: ScopeManagerRef
    ) -> Result<(), CompilationError> {
        let ast_father = ast_father_node.borrow();
        let children = ast_father.children.clone();
        let ident = children[1].clone();
        scope_manager.borrow_mut().set_type(
            ident.borrow().token.as_ref().unwrap(),
            "int"
        )?;
        Ok(())
    }

    pub fn action3(
        ast_father_node: AstNodeRef,
        scope_manager: ScopeManagerRef
    ) -> Result<(), CompilationError> {
        let ast_father = ast_father_node.borrow();
        let children = ast_father.children.clone();
        let ident = children[1].clone();
        scope_manager.borrow_mut().set_type(
            ident.borrow().token.as_ref().unwrap(),
            "float"
        )?;
        Ok(())
    }

    pub fn action4(
        ast_father_node: AstNodeRef,
        scope_manager: ScopeManagerRef
    ) -> Result<(), CompilationError> {
        let ast_father = ast_father_node.borrow();
        let children = ast_father.children.clone();
        let ident = children[1].clone();
        scope_manager.borrow_mut().set_type(
            ident.borrow().token.as_ref().unwrap(),
            "string"
        )?;
        Ok(())
    }

    pub fn action5(
        ast_father_node: AstNodeRef,
        scope_manager: ScopeManagerRef
    ) -> Result<(), CompilationError> {
        let ast_father = ast_father_node.borrow();
        let children = ast_father.children.clone();
        let index = children[0].clone();
        let index1 = children[3].clone();
        
        let index_ref = index.borrow();
        let mut index1_ref = index1.borrow_mut();
        index1_ref.decl_data.base_type = index_ref.decl_data.base_type.clone();
        Ok(())
    }

    pub fn action6(
        ast_father_node: AstNodeRef,
        scope_manager: ScopeManagerRef
    ) -> Result<(), CompilationError> {
        let ast_father = ast_father_node.borrow();
        let children = ast_father.children.clone();
        let index = children[0].clone();
        let index1 = children[3].clone();
        let int_const = children[1].clone();
        
        let int_const_ref = int_const.borrow();
        let index1_ref = index1.borrow();
        let mut index_ref = index.borrow_mut();
        
        index_ref.decl_data.resolved_type = format!(
            "array({}, {})",
            int_const_ref.token.as_ref().unwrap().lexeme,
            index1_ref.decl_data.resolved_type
        );
        Ok(())
    }

    pub fn action7(
        ast_father_node: AstNodeRef,
        scope_manager: ScopeManagerRef
    ) -> Result<(), CompilationError> {
        let mut ast_father = ast_father_node.borrow_mut();
        ast_father.decl_data.resolved_type = ast_father.decl_data.base_type.clone();
        Ok(())
    }

    pub fn action8(
        ast_father_node: AstNodeRef,
        scope_manager: ScopeManagerRef
    ) -> Result<(), CompilationError> {
        let mut ast_father = ast_father_node.borrow_mut();
        let children = ast_father.children.clone();
        let index = children[2].clone();
        
        let mut index_ref = index.borrow_mut();
        index_ref.decl_data.base_type = "int".to_string();
        Ok(())
    }

    pub fn action9(
        ast_father_node: AstNodeRef,
        scope_manager: ScopeManagerRef
    ) -> Result<(), CompilationError> {
        let mut ast_father = ast_father_node.borrow_mut();
        let children = ast_father.children.clone();
        let index = children[2].clone();
        
        let mut index_ref = index.borrow_mut();
        index_ref.decl_data.base_type = "float".to_string();
        Ok(())
    }

    pub fn action10(
        ast_father_node: AstNodeRef,
        scope_manager: ScopeManagerRef
    ) -> Result<(), CompilationError> {
        let mut ast_father = ast_father_node.borrow_mut();
        let children = ast_father.children.clone();
        let index = children[2].clone();
        
        let mut index_ref = index.borrow_mut();
        index_ref.decl_data.base_type = "string".to_string();
        Ok(())
    }

    pub fn action11(
        ast_father_node: AstNodeRef,
        scope_manager: ScopeManagerRef
    ) -> Result<(), CompilationError> {
        let ast_father = ast_father_node.borrow();
        let children = ast_father.children.clone();
        let ident = children[1].clone();
        let index = children[2].clone();
        
        let index_ref = index.borrow();
        scope_manager.borrow_mut().set_type(
            ident.borrow().token.as_ref().unwrap(),
            &index_ref.decl_data.resolved_type
        )?;
        Ok(())
    }
}

// Módulo DECLARE_VERIFY: Verificação de declarações
pub mod declare_verify {
    use super::*;

    pub fn action1(
        ast_father_node: AstNodeRef,
        scope_manager: ScopeManagerRef
    ) -> Result<(), CompilationError> {
        let ast_father = ast_father_node.borrow();
        let children = ast_father.children.clone();
        let ident = children[2].clone();
        let token = ident.borrow().token.as_ref().unwrap();
        
        if !scope_manager.borrow().is_declared(token) {
            return Err(CompilationError::semantic(format!(
                "{} used before declaration at {}:{}",
                token.lexeme, token.line, token.column
            )));
        }
        
        if !scope_manager.borrow().is_type(token, "function") {
            return Err(CompilationError::semantic(format!(
                "{} at {}:{} is not a function",
                token.lexeme, token.line, token.column
            )));
        }
        
        Ok(())
    }

    pub fn action2(
        ast_father_node: AstNodeRef,
        scope_manager: ScopeManagerRef
    ) -> Result<(), CompilationError> {
        let ast_father = ast_father_node.borrow();
        let children = ast_father.children.clone();
        let ident = children[0].clone();
        let token = ident.borrow().token.as_ref().unwrap();
        
        if !scope_manager.borrow().is_declared(token) {
            return Err(CompilationError::semantic(format!(
                "{} used before declaration at {}:{}",
                token.lexeme, token.line, token.column
            )));
        }
        
        Ok(())
    }
}

// Módulo BREAK: Verificação de uso de break
pub mod break_ {
    use super::*;

    pub fn action1(
        ast_father_node: AstNodeRef,
        scope_manager: ScopeManagerRef
    ) -> Result<(), CompilationError> {
        let ast_father = ast_father_node.borrow();
        let children = ast_father.children.clone();
        let break_t = children[0].clone();
        let token = break_t.borrow().token.as_ref().unwrap();
        
        if !scope_manager.borrow().is_in_scope("for") {
            return Err(CompilationError::semantic(format!(
                "break used outside for statement at {}:{}",
                token.line, token.column
            )));
        }
        
        Ok(())
    }
}

// Módulo AUXILIAR: Funções auxiliares
pub mod auxiliar {
    use super::*;

    pub fn get_type(type_str: &str, array_counter: i32) -> Result<String, CompilationError> {
        let first_char = type_str.chars().next().unwrap_or(' ');
        let base_type = match first_char {
            'i' => "int",
            'f' => "float",
            's' => "string",
            'a' => {
                let mut counter = 0;
                let mut final_type = "";
                for c in type_str.chars() {
                    match c {
                        '(' => counter += 1,
                        'i' => final_type = "int",
                        'f' => final_type = "float",
                        's' => final_type = "string",
                        _ => {}
                    }
                }
                if counter != array_counter {
                    return Err(CompilationError::semantic(
                        "Array dimension mismatch in operations".to_string()
                    ));
                }
                final_type
            }
            _ => {
                return Err(CompilationError::semantic(
                    format!("Invalid type: {}", type_str)
                ));
            }
        };
        Ok(base_type.to_string())
    }

    pub fn get_array_max(type_str: &str) -> Vec<i32> {
        let mut values = Vec::new();
        for c in type_str.chars() {
            if c.is_digit(10) {
                values.push(c.to_digit(10).unwrap() as i32);
            }
        }
        values
    }
}

// Módulo EXPA: Expressões e arrays
pub mod expa {
    use super::*;

    pub fn set_operation(
        ast_father_node: AstNodeRef,
        scope_manager: ScopeManagerRef
    ) -> Result<(), CompilationError> {
        let mut ast_father = ast_father_node.borrow_mut();
        let children = ast_father.children.clone();
        let operation = children[0].clone();
        let numexpression = children[1].clone();
        
        let operation_ref = operation.borrow();
        let numexpression_ref = numexpression.borrow();
        
        let op_char = operation_ref.token.as_ref().unwrap().lexeme.chars().next()
            .ok_or_else(|| CompilationError::semantic("Invalid operator".to_string()))?;
        
        ast_father.expr_attrs.operation = op_char;
        ast_father.expr_attrs.expr_node = numexpression_ref.expr_attrs.expr_node.clone();
        ast_father.expr_attrs.array_counter = numexpression_ref.expr_attrs.array_counter;
        ast_father.generated_code = numexpression_ref.generated_code.clone();
        ast_father.codegen_state.return_value = numexpression_ref.codegen_state.return_value.clone();
        Ok(())
    }

    pub fn ident_up(
        ast_father_node: AstNodeRef,
        scope_manager: ScopeManagerRef
    ) -> Result<(), CompilationError> {
        let mut ast_father = ast_father_node.borrow_mut();
        let children = ast_father.children.clone();
        let lvalue = children[0].clone();
        
        let lvalue_ref = lvalue.borrow();
        ast_father.expr_attrs.expr_node = lvalue_ref.expr_attrs.expr_node.clone();
        ast_father.expr_attrs.array_counter = lvalue_ref.expr_attrs.array_counter;
        ast_father.generated_code.extend(lvalue_ref.generated_code.iter().cloned());
        Ok(())
    }

    pub fn value_up(
        ast_father_node: AstNodeRef,
        scope_manager: ScopeManagerRef
    ) -> Result<(), CompilationError> {
        let mut ast_father = ast_father_node.borrow_mut();
        let children = ast_father.children.clone();
        let factor = children[0].clone();
        
        let factor_ref = factor.borrow();
        ast_father.expr_attrs.array_counter = factor_ref.expr_attrs.array_counter;
        ast_father.expr_attrs.expr_node = factor_ref.expr_attrs.expr_node.clone();
        ast_father.codegen_state.return_value = factor_ref.codegen_state.return_value.clone();
        ast_father.generated_code = factor_ref.generated_code.clone();
        Ok(())
    }

    pub fn second_child_value_up(
        ast_father_node: AstNodeRef,
        scope_manager: ScopeManagerRef
    ) -> Result<(), CompilationError> {
        let mut ast_father = ast_father_node.borrow_mut();
        let children = ast_father.children.clone();
        let symbol = children[0].clone();
        let factor = children[1].clone();
        
        let symbol_ref = symbol.borrow();
        let factor_ref = factor.borrow();
        
        let temp_var = generate_temp_var();
        ast_father.codegen_state.return_value = temp_var.clone();
        
        let op_char = symbol_ref.token.as_ref().unwrap().lexeme.chars().next()
            .ok_or_else(|| CompilationError::semantic("Invalid operator".to_string()))?;
        
        let factor_value = if factor_ref.expr_attrs.array_counter > 0 {
            factor_ref.expr_attrs.expr_node.as_ref().unwrap().borrow().value.clone()
        } else {
            factor_ref.codegen_state.return_value.clone()
        };
        
        ast_father.generated_code.push(format!(
            "{} = {}{}",
            temp_var,
            op_char,
            factor_value
        ));
        
        ast_father.expr_attrs.expr_node = Some(Rc::new(RefCell::new(ExpressionNode::unary(
            op_char,
            temp_var.clone(),
            factor_ref.expr_attrs.expr_node.as_ref().unwrap().clone()
        ))));
        
        Ok(())
    }

    pub fn generate_node(
        ast_father_node: AstNodeRef,
        scope_manager: ScopeManagerRef
    ) -> Result<(), CompilationError> {
        let mut ast_father = ast_father_node.borrow_mut();
        let children = ast_father.children.clone();
        let term_node = children[0].clone();
        let numexpression_aux = children[1].clone();
        
        let term_ref = term_node.borrow();
        let numexpression_aux_ref = numexpression_aux.borrow();
        
        if let Some(aux_node) = &numexpression_aux_ref.expr_attrs.expr_node {
            // Obter tipos base
            let term_type = auxiliar::get_type(
                &term_ref.expr_attrs.expr_node.as_ref().unwrap().borrow().data_type,
                term_ref.expr_attrs.array_counter
            )?;
            
            let aux_type = auxiliar::get_type(
                &aux_node.borrow().data_type,
                numexpression_aux_ref.expr_attrs.array_counter
            )?;
            
            // Verificar compatibilidade de tipos
            if term_type != aux_type {
                return Err(CompilationError::semantic(format!(
                    "Type mismatch: {} and {}",
                    term_type, aux_type
                )));
            }
            
            // Obter valores para operação
            let term_value = if term_ref.expr_attrs.array_counter > 0 {
                term_ref.expr_attrs.expr_node.as_ref().unwrap().borrow().value.clone()
            } else {
                term_ref.codegen_state.return_value.clone()
            };
            
            let aux_value = if numexpression_aux_ref.expr_attrs.array_counter > 0 {
                aux_node.borrow().value.clone()
            } else {
                numexpression_aux_ref.codegen_state.return_value.clone()
            };
            
            // Gerar código e nó da expressão
            let temp_var = generate_temp_var();
            ast_father.codegen_state.return_value = temp_var.clone();
            
            ast_father.generated_code.extend(term_ref.generated_code.iter().cloned());
            ast_father.generated_code.extend(numexpression_aux_ref.generated_code.iter().cloned());
            ast_father.generated_code.push(format!(
                "{} = {} {} {}",
                temp_var,
                term_value,
                numexpression_aux_ref.expr_attrs.operation,
                aux_value
            ));
            
            ast_father.expr_attrs.expr_node = Some(Rc::new(RefCell::new(ExpressionNode::binary(
                numexpression_aux_ref.expr_attrs.operation,
                temp_var,
                term_ref.expr_attrs.expr_node.as_ref().unwrap().clone(),
                aux_node.clone()
            )?));
        } else {
            // Propagação simples se não houver operação
            ast_father.expr_attrs.expr_node = term_ref.expr_attrs.expr_node.clone();
            ast_father.expr_attrs.array_counter = term_ref.expr_attrs.array_counter;
            ast_father.codegen_state.return_value = term_ref.codegen_state.return_value.clone();
            ast_father.generated_code = term_ref.generated_code.clone();
        }
        
        Ok(())
    }

    pub fn set_operation2(
        ast_father_node: AstNodeRef,
        scope_manager: ScopeManagerRef
    ) -> Result<(), CompilationError> {
        let mut ast_father = ast_father_node.borrow_mut();
        let children = ast_father.children.clone();
        let op = children[0].clone();
        let term = children[1].clone();
        
        let op_ref = op.borrow();
        let term_ref = term.borrow();
        
        let op_char = op_ref.token.as_ref().unwrap().lexeme.chars().next()
            .ok_or_else(|| CompilationError::semantic("Invalid operator".to_string()))?;
        
        ast_father.expr_attrs.operation = op_char;
        ast_father.expr_attrs.expr_node = term_ref.expr_attrs.expr_node.clone();
        ast_father.expr_attrs.array_counter = term_ref.expr_attrs.array_counter;
        ast_father.generated_code = term_ref.generated_code.clone();
        ast_father.codegen_state.return_value = term_ref.codegen_state.return_value.clone();
        Ok(())
    }

    pub fn term(
        ast_father_node: AstNodeRef,
        scope_manager: ScopeManagerRef
    ) -> Result<(), CompilationError> {
        let mut ast_father = ast_father_node.borrow_mut();
        let children = ast_father.children.clone();
        let unaryexpr = children[0].clone();
        let unaryexpraux = children[1].clone();
        
        let unaryexpr_ref = unaryexpr.borrow();
        let unaryexpraux_ref = unaryexpraux.borrow();
        
        if let Some(aux_node) = &unaryexpraux_ref.expr_attrs.expr_node {
            // Obter tipos base
            let expr_type = auxiliar::get_type(
                &unaryexpr_ref.expr_attrs.expr_node.as_ref().unwrap().borrow().data_type,
                unaryexpr_ref.expr_attrs.array_counter
            )?;
            
            let aux_type = auxiliar::get_type(
                &aux_node.borrow().data_type,
                unaryexpraux_ref.expr_attrs.array_counter
            )?;
            
            // Verificar compatibilidade de tipos
            if expr_type != aux_type {
                return Err(CompilationError::semantic(format!(
                    "Type mismatch: {} and {}",
                    expr_type, aux_type
                )));
            }
            
            // Obter valores para operação
            let expr_value = if unaryexpr_ref.expr_attrs.array_counter > 0 {
                unaryexpr_ref.expr_attrs.expr_node.as_ref().unwrap().borrow().value.clone()
            } else {
                unaryexpr_ref.codegen_state.return_value.clone()
            };
            
            let aux_value = if unaryexpraux_ref.expr_attrs.array_counter > 0 {
                aux_node.borrow().value.clone()
            } else {
                unaryexpraux_ref.codegen_state.return_value.clone()
            };
            
            // Gerar código e nó da expressão
            let temp_var = generate_temp_var();
            ast_father.codegen_state.return_value = temp_var.clone();
            
            ast_father.generated_code.extend(unaryexpr_ref.generated_code.iter().cloned());
            ast_father.generated_code.extend(unaryexpraux_ref.generated_code.iter().cloned());
            ast_father.generated_code.push(format!(
                "{} = {} {} {}",
                temp_var,
                expr_value,
                unaryexpraux_ref.expr_attrs.operation,
                aux_value
            ));
            
            ast_father.expr_attrs.expr_node = Some(Rc::new(RefCell::new(ExpressionNode::binary(
                unaryexpraux_ref.expr_attrs.operation,
                temp_var,
                unaryexpr_ref.expr_attrs.expr_node.as_ref().unwrap().clone(),
                aux_node.clone()
            )?)));
        } else {
            // Caso sem operação adicional
            let temp_var = generate_temp_var();
            ast_father.codegen_state.return_value = temp_var.clone();
            
            let expr_value = if unaryexpr_ref.expr_attrs.array_counter > 0 {
                unaryexpr_ref.expr_attrs.expr_node.as_ref().unwrap().borrow().value.clone()
            } else {
                unaryexpr_ref.codegen_state.return_value.clone()
            };
            
            ast_father.generated_code.extend(unaryexpr_ref.generated_code.iter().cloned());
            ast_father.generated_code.push(format!(
                "{} = {}",
                temp_var,
                expr_value
            ));
            
            ast_father.expr_attrs.expr_node = unaryexpr_ref.expr_attrs.expr_node.clone();
        }
        
        Ok(())
    }
}