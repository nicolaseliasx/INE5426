// parser_table.rs
use std::collections::HashMap;
use lazy_static::lazy_static;
use crate::parser_stack_item::{StackItem, StackItemType};
use crate::semantic_actions::SemanticAction;

lazy_static! {
    pub static ref SYNTAX_DIRECTED_TRANSLATION: HashMap<&'static str, Vec<StackItem>> = {
        let mut table = HashMap::new();
        let action = StackItemType::Action;
        let symbol = StackItemType::Symbol;
        
        // Regras de produção
        table.insert("PROGRAM-DEF", vec![
            StackItem::new(action, SemanticAction::ScopeInit),
            StackItem::new(action, SemanticAction::SetFinalLabel),
            StackItem::new(action, SemanticAction::InheritNext),
            StackItem::new(symbol, "FUNCLIST"),
            StackItem::new(symbol, "PROGRAM_TAIL"),
            StackItem::new(action, SemanticAction::GotoMain),
            StackItem::new(action, SemanticAction::GenerateFinalCode),
        ]);
        
        table.insert("PROGRAM-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR", vec![
            StackItem::new(action, SemanticAction::ScopeInit),
            StackItem::new(action, SemanticAction::SetFinalLabel),
            StackItem::new(action, SemanticAction::InheritNext),
            StackItem::new(symbol, "STATEMENT"),
            StackItem::new(action, SemanticAction::CollectChildCode),
            StackItem::new(action, SemanticAction::GenerateFinalCode),
        ]);
        
        table.insert("PROGRAM-$", vec![
            StackItem::new(action, SemanticAction::ScopeInit),
            StackItem::new(symbol, "ε"),
        ]);
        
        table.insert("PROGRAM_TAIL-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR", vec![
            StackItem::new(action, SemanticAction::InheritNext),
            StackItem::new(symbol, "STATEMENT"),
            StackItem::new(action, SemanticAction::CollectChildCode),
        ]);
        
        table.insert("PROGRAM_TAIL-$", vec![
            StackItem::new(symbol, "ε"),
        ]);
        
        table.insert("FUNCLIST-DEF", vec![
            StackItem::new(action, SemanticAction::InheritNext),
            StackItem::new(symbol, "FUNCTION_DEF"),
            StackItem::new(symbol, "FUNCLIST_TAIL"),
            StackItem::new(action, SemanticAction::CollectChildCode),
        ]);
        
        table.insert("FUNCLIST_TAIL-DEF", vec![
            StackItem::new(action, SemanticAction::InheritNext),
            StackItem::new(symbol, "FUNCLIST"),
            StackItem::new(action, SemanticAction::CollectChildCode),
        ]);
        
        table.insert("FUNCLIST_TAIL-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$", vec![
            StackItem::new(symbol, "ε"),
        ]);
        
        table.insert("FUNCTION_DEF-DEF", vec![
            StackItem::new(symbol, "DEF"),
            StackItem::new(symbol, "IDENT"),
            StackItem::new(action, SemanticAction::FunctionHeader),
            StackItem::new(action, SemanticAction::ScopeEnter),
            StackItem::new(action, SemanticAction::DeclareFunction),
            StackItem::new(action, SemanticAction::ScopePush),
            StackItem::new(symbol, "OPEN_PARENTHESIS"),
            StackItem::new(symbol, "PARAM_LIST"),
            StackItem::new(symbol, "CLOSE_PARENTHESIS"),
            StackItem::new(symbol, "OPEN_BRACE"),
            StackItem::new(symbol, "STATEMENT_LIST"),
            StackItem::new(symbol, "CLOSE_BRACE"),
            StackItem::new(action, SemanticAction::ScopeExit),
            StackItem::new(action, SemanticAction::FunctionFooter),
        ]);
        
        table.insert("PARAM_LIST-CLOSE_PARENTHESIS", vec![
            StackItem::new(symbol, "ε"),
        ]);
        
        table.insert("PARAM_LIST-INT", vec![
            StackItem::new(symbol, "INT"),
            StackItem::new(symbol, "IDENT"),
            StackItem::new(action, SemanticAction::ScopeEnter),
            StackItem::new(action, SemanticAction::DeclareIntParam),
            StackItem::new(symbol, "PARAM_LIST_TAIL"),
        ]);
        
        table.insert("PARAM_LIST-FLOAT", vec![
            StackItem::new(symbol, "FLOAT"),
            StackItem::new(symbol, "IDENT"),
            StackItem::new(action, SemanticAction::ScopeEnter),
            StackItem::new(action, SemanticAction::DeclareFloatParam),
            StackItem::new(symbol, "PARAM_LIST_TAIL"),
        ]);
        
        table.insert("PARAM_LIST-STRING", vec![
            StackItem::new(symbol, "STRING"),
            StackItem::new(symbol, "IDENT"),
            StackItem::new(action, SemanticAction::ScopeEnter),
            StackItem::new(action, SemanticAction::DeclareStringParam),
            StackItem::new(symbol, "PARAM_LIST_TAIL"),
        ]);
        
        table.insert("PARAM_LIST_TAIL-CLOSE_PARENTHESIS", vec![
            StackItem::new(symbol, "ε"),
        ]);
        
        table.insert("PARAM_LIST_TAIL-COMMA", vec![
            StackItem::new(symbol, "COMMA"),
            StackItem::new(symbol, "PARAM_LIST"),
        ]);
        
        table.insert("STATEMENT-IDENT", vec![
            StackItem::new(action, SemanticAction::InheritNext),
            StackItem::new(symbol, "ASSIGN_STAT"),
            StackItem::new(symbol, "SEMICOLON"),
            StackItem::new(action, SemanticAction::CollectChildCode),
        ]);
        
        table.insert("STATEMENT-OPEN_BRACE", vec![
            StackItem::new(action, SemanticAction::InheritNext),
            StackItem::new(symbol, "OPEN_BRACE"),
            StackItem::new(action, SemanticAction::ScopeEnter),
            StackItem::new(symbol, "STATEMENT_LIST"),
            StackItem::new(symbol, "CLOSE_BRACE"),
            StackItem::new(action, SemanticAction::ScopeExit),
            StackItem::new(action, SemanticAction::CollectChildCode),
        ]);
        
        table.insert("STATEMENT-INT-FLOAT-STRING", vec![
            StackItem::new(symbol, "VARIABLE_DECL"),
            StackItem::new(symbol, "SEMICOLON"),
            StackItem::new(action, SemanticAction::CollectChildCode),
        ]);
        
        table.insert("STATEMENT-SEMICOLON", vec![
            StackItem::new(symbol, "SEMICOLON"),
        ]);
        
        table.insert("STATEMENT-BREAK", vec![
            StackItem::new(symbol, "BREAK"),
            StackItem::new(action, SemanticAction::HandleBreak),
            StackItem::new(symbol, "SEMICOLON"),
            StackItem::new(action, SemanticAction::GenerateBreakCode),
        ]);
        
        table.insert("STATEMENT-PRINT", vec![
            StackItem::new(action, SemanticAction::InheritNext),
            StackItem::new(symbol, "PRINT_STAT"),
            StackItem::new(symbol, "SEMICOLON"),
            StackItem::new(action, SemanticAction::CollectChildCode),
        ]);
        
        table.insert("STATEMENT-READ", vec![
            StackItem::new(action, SemanticAction::InheritNext),
            StackItem::new(symbol, "READ_STAT"),
            StackItem::new(symbol, "SEMICOLON"),
            StackItem::new(action, SemanticAction::CollectChildCode),
        ]);
        
        table.insert("STATEMENT-RETURN", vec![
            StackItem::new(action, SemanticAction::InheritNext),
            StackItem::new(symbol, "RETURN_STAT"),
            StackItem::new(symbol, "SEMICOLON"),
            StackItem::new(action, SemanticAction::CollectChildCode),
        ]);
        
        table.insert("STATEMENT-IF", vec![
            StackItem::new(action, SemanticAction::InheritNext),
            StackItem::new(symbol, "IF_STAT"),
            StackItem::new(action, SemanticAction::CollectChildCode),
        ]);
        
        table.insert("STATEMENT-FOR", vec![
            StackItem::new(action, SemanticAction::InheritNext),
            StackItem::new(symbol, "FOR_STAT"),
            StackItem::new(action, SemanticAction::CollectChildCode),
        ]);
        
        table.insert("INDEX-SEMICOLON", vec![
            StackItem::new(symbol, "ε"),
            StackItem::new(action, SemanticAction::FinalizeArrayDecl),
        ]);
        
        table.insert("INDEX-OPEN_BRACKET", vec![
            StackItem::new(symbol, "OPEN_BRACKET"),
            StackItem::new(symbol, "NUM_EXPR"),
            StackItem::new(symbol, "CLOSE_BRACKET"),
            StackItem::new(action, SemanticAction::ProcessArrayDim),
            StackItem::new(symbol, "INDEX"),
            StackItem::new(action, SemanticAction::FinalizeArrayDecl),
        ]);
        
        table.insert("VARIABLE_DECL-INT", vec![
            StackItem::new(action, SemanticAction::InheritNext),
            StackItem::new(symbol, "INT"),
            StackItem::new(symbol, "IDENT"),
            StackItem::new(action, SemanticAction::ScopeEnter),
            StackItem::new(action, SemanticAction::DeclareInt),
            StackItem::new(symbol, "INDEX"),
            StackItem::new(action, SemanticAction::FinalizeDeclaration),
            StackItem::new(action, SemanticAction::CollectChildCode),
        ]);
        
        table.insert("VARIABLE_DECL-FLOAT", vec![
            StackItem::new(action, SemanticAction::InheritNext),
            StackItem::new(symbol, "FLOAT"),
            StackItem::new(symbol, "IDENT"),
            StackItem::new(action, SemanticAction::ScopeEnter),
            StackItem::new(action, SemanticAction::DeclareFloat),
            StackItem::new(symbol, "INDEX"),
            StackItem::new(action, SemanticAction::FinalizeDeclaration),
            StackItem::new(action, SemanticAction::CollectChildCode),
        ]);
        
        table.insert("VARIABLE_DECL-STRING", vec![
            StackItem::new(action, SemanticAction::InheritNext),
            StackItem::new(symbol, "STRING"),
            StackItem::new(symbol, "IDENT"),
            StackItem::new(action, SemanticAction::ScopeEnter),
            StackItem::new(action, SemanticAction::DeclareString),
            StackItem::new(symbol, "INDEX"),
            StackItem::new(action, SemanticAction::FinalizeDeclaration),
            StackItem::new(action, SemanticAction::CollectChildCode),
        ]);
        
        table.insert("ASSIGN_STAT-IDENT", vec![
            StackItem::new(action, SemanticAction::InheritNext),
            StackItem::new(symbol, "LVALUE"),
            StackItem::new(symbol, "ASSIGN"),
            StackItem::new(symbol, "ASSIGN_TAIL"),
            StackItem::new(action, SemanticAction::GenerateAssignment),
        ]);
        
        table.insert("ASSIGN_TAIL-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL", vec![
            StackItem::new(symbol, "EXPRESSION"),
            StackItem::new(action, SemanticAction::SetExpressionValue),
        ]);
        
        table.insert("ASSIGN_TAIL-CALL", vec![
            StackItem::new(action, SemanticAction::GetReturnValue),
            StackItem::new(action, SemanticAction::InheritNext),
            StackItem::new(symbol, "FUNC_CALL"),
            StackItem::new(action, SemanticAction::CollectChildCode),
        ]);
        
        table.insert("ASSIGN_TAIL-NEW", vec![
            StackItem::new(symbol, "ALLOC_EXPR"),
        ]);
        
        table.insert("FUNC_CALL-CALL", vec![
            StackItem::new(symbol, "CALL"),
            StackItem::new(symbol, "OPEN_PARENTHESIS"),
            StackItem::new(symbol, "IDENT"),
            StackItem::new(action, SemanticAction::VerifyFunction),
            StackItem::new(symbol, "OPEN_PARENTHESIS"),
            StackItem::new(symbol, "PARAM_LIST_CALL"),
            StackItem::new(symbol, "CLOSE_PARENTHESIS"),
            StackItem::new(symbol, "CLOSE_PARENTHESIS"),
            StackItem::new(action, SemanticAction::GenerateFunctionCall),
        ]);
        
        table.insert("PARAM_LIST_CALL-IDENT", vec![
            StackItem::new(symbol, "IDENT"),
            StackItem::new(action, SemanticAction::VerifyParameter),
            StackItem::new(symbol, "PARAM_LIST_CALL_TAIL"),
            StackItem::new(action, SemanticAction::ProcessParameterList),
        ]);
        
        table.insert("PARAM_LIST_CALL-CLOSE_PARENTHESIS", vec![
            StackItem::new(symbol, "ε"),
        ]);
        
        table.insert("PARAM_LIST_CALL_TAIL-CLOSE_PARENTHESIS", vec![
            StackItem::new(symbol, "ε"),
        ]);
        
        table.insert("PARAM_LIST_CALL_TAIL-COMMA", vec![
            StackItem::new(symbol, "COMMA"),
            StackItem::new(symbol, "PARAM_LIST_CALL"),
            StackItem::new(action, SemanticAction::CollectParameters),
            StackItem::new(action, SemanticAction::CollectChildCode),
        ]);
        
        table.insert("PRINT_STAT-PRINT", vec![
            StackItem::new(symbol, "PRINT"),
            StackItem::new(symbol, "EXPRESSION"),
            StackItem::new(action, SemanticAction::GeneratePrint),
        ]);
        
        table.insert("READ_STAT-READ", vec![
            StackItem::new(symbol, "READ"),
            StackItem::new(symbol, "LVALUE"),
            StackItem::new(action, SemanticAction::GenerateRead),
        ]);
        
        table.insert("RETURN_STAT-RETURN", vec![
            StackItem::new(symbol, "RETURN"),
            StackItem::new(symbol, "RETURN_TAIL"),
            StackItem::new(action, SemanticAction::GenerateReturn),
        ]);
        
        table.insert("RETURN_TAIL-IDENT", vec![
            StackItem::new(symbol, "LVALUE"),
            StackItem::new(action, SemanticAction::SetReturnValue),
        ]);
        
        table.insert("RETURN_TAIL-SEMICOLON", vec![
            StackItem::new(symbol, "ε"),
        ]);
        
        table.insert("IF_STAT-IF", vec![
            StackItem::new(action, SemanticAction::InheritNext),
            StackItem::new(symbol, "IF"),
            StackItem::new(symbol, "OPEN_PARENTHESIS"),
            StackItem::new(symbol, "EXPRESSION"),
            StackItem::new(symbol, "CLOSE_PARENTHESIS"),
            StackItem::new(symbol, "OPEN_BRACE"),
            StackItem::new(action, SemanticAction::ScopeEnterConditional),
            StackItem::new(symbol, "STATEMENT"),
            StackItem::new(symbol, "CLOSE_BRACE"),
            StackItem::new(action, SemanticAction::ScopeExit),
            StackItem::new(symbol, "IF_TAIL"),
            StackItem::new(action, SemanticAction::FinalizeIfStatement),
        ]);
        
        table.insert("IF_TAIL-IDENT-OPEN_BRACE-CLOSE_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$", vec![
            StackItem::new(action, SemanticAction::HandleEmptyElse),
            StackItem::new(symbol, "ε"),
        ]);
        
        table.insert("IF_TAIL-ELSE", vec![
            StackItem::new(action, SemanticAction::ElseHeader),
            StackItem::new(symbol, "ELSE"),
            StackItem::new(symbol, "OPEN_BRACE"),
            StackItem::new(action, SemanticAction::ScopeEnterConditional),
            StackItem::new(symbol, "STATEMENT"),
            StackItem::new(symbol, "CLOSE_BRACE"),
            StackItem::new(action, SemanticAction::ScopeExit),
            StackItem::new(action, SemanticAction::ElseFooter),
        ]);
        
        table.insert("FOR_STAT-FOR", vec![
            StackItem::new(action, SemanticAction::InheritNext),
            StackItem::new(action, SemanticAction::InheritForLabels),
            StackItem::new(symbol, "FOR"),
            StackItem::new(symbol, "OPEN_PARENTHESIS"),
            StackItem::new(symbol, "ASSIGN_STAT"),
            StackItem::new(symbol, "SEMICOLON"),
            StackItem::new(symbol, "EXPRESSION"),
            StackItem::new(symbol, "SEMICOLON"),
            StackItem::new(symbol, "ASSIGN_STAT"),
            StackItem::new(symbol, "CLOSE_PARENTHESIS"),
            StackItem::new(action, SemanticAction::ScopeEnterLoop),
            StackItem::new(symbol, "STATEMENT"),
            StackItem::new(action, SemanticAction::ScopeExit),
            StackItem::new(action, SemanticAction::FinalizeForLoop),
        ]);
        
        table.insert("STATEMENT_LIST-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR", vec![
            StackItem::new(action, SemanticAction::InheritNext),
            StackItem::new(action, SemanticAction::SetLabel),
            StackItem::new(symbol, "STATEMENT"),
            StackItem::new(symbol, "STATEMENT_LIST_TAIL"),
            StackItem::new(action, SemanticAction::CollectChildCode),
        ]);
        
        table.insert("STATEMENT_LIST_TAIL-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR", vec![
            StackItem::new(action, SemanticAction::InheritNext),
            StackItem::new(symbol, "STATEMENT_LIST"),
            StackItem::new(action, SemanticAction::CollectChildCode),
        ]);
        
        table.insert("STATEMENT_LIST_TAIL-CLOSE_BRACE", vec![
            StackItem::new(action, SemanticAction::InheritNext),
            StackItem::new(symbol, "ε"),
            StackItem::new(action, SemanticAction::CollectChildCode),
        ]);
        
        table.insert("ALLOC_AUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_BRACKET-ASSIGN-RELOP-PLUS-MINUS-MULTIPLICATION-DIVISION-MODULUS", vec![
            StackItem::new(symbol, "ε"),
        ]);
        
        table.insert("ALLOC_AUX-OPEN_BRACKET", vec![
            StackItem::new(symbol, "OPEN_BRACKET"),
            StackItem::new(symbol, "NUM_EXPR"),
            StackItem::new(symbol, "CLOSE_BRACKET"),
            StackItem::new(symbol, "ALLOC_AUX"),
            StackItem::new(action, SemanticAction::ProcessArrayDim),
            StackItem::new(action, SemanticAction::IncrementArrayCounter),
        ]);
        
        table.insert("ALLOC_EXPR-NEW", vec![
            StackItem::new(symbol, "NEW"),
            StackItem::new(symbol, "ALLOC_EXPR_TAIL"),
        ]);
        
        table.insert("ALLOC_EXPR_TAIL-INT", vec![
            StackItem::new(symbol, "INT"),
            StackItem::new(symbol, "OPEN_BRACKET"),
            StackItem::new(symbol, "NUM_EXPR"),
            StackItem::new(symbol, "CLOSE_BRACKET"),
            StackItem::new(symbol, "ALLOC_AUX"),
            StackItem::new(action, SemanticAction::GenerateAllocation),
        ]);
        
        table.insert("ALLOC_EXPR_TAIL-FLOAT", vec![
            StackItem::new(symbol, "FLOAT"),
            StackItem::new(symbol, "OPEN_BRACKET"),
            StackItem::new(symbol, "NUM_EXPR"),
            StackItem::new(symbol, "CLOSE_BRACKET"),
            StackItem::new(symbol, "ALLOC_AUX"),
            StackItem::new(action, SemanticAction::GenerateAllocation),
        ]);
        
        table.insert("ALLOC_EXPR_TAIL-STRING", vec![
            StackItem::new(symbol, "STRING"),
            StackItem::new(symbol, "OPEN_BRACKET"),
            StackItem::new(symbol, "NUM_EXPR"),
            StackItem::new(symbol, "CLOSE_BRACKET"),
            StackItem::new(symbol, "ALLOC_AUX"),
            StackItem::new(action, SemanticAction::GenerateAllocation),
        ]);
        
        table.insert("EXPRESSION-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL", vec![
            StackItem::new(symbol, "NUM_EXPR"),
            StackItem::new(action, SemanticAction::ExpressionHeader),
            StackItem::new(symbol, "EXPRESSION_TAIL"),
            StackItem::new(action, SemanticAction::ExpressionFooter),
        ]);
        
        table.insert("EXPRESSION_TAIL-CLOSE_PARENTHESIS-SEMICOLON", vec![
            StackItem::new(symbol, "ε"),
        ]);
        
        table.insert("EXPRESSION_TAIL-RELOP", vec![
            StackItem::new(symbol, "RELOP"),
            StackItem::new(symbol, "NUM_EXPR"),
            StackItem::new(action, SemanticAction::GenerateRelationalOp),
            StackItem::new(action, SemanticAction::HandleRelationalOp),
        ]);
        
        table.insert("NUM_EXPR_AUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_BRACKET-RELOP", vec![
            StackItem::new(symbol, "ε"),
        ]);
        
        table.insert("NUM_EXPR_AUX-PLUS", vec![
            StackItem::new(symbol, "PLUS"),
            StackItem::new(symbol, "NUM_EXPR"),
            StackItem::new(action, SemanticAction::SetOperation),
        ]);
        
        table.insert("NUM_EXPR_AUX-MINUS", vec![
            StackItem::new(symbol, "MINUS"),
            StackItem::new(symbol, "NUM_EXPR"),
            StackItem::new(action, SemanticAction::SetOperation),
        ]);
        
        table.insert("NUM_EXPR-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL", vec![
            StackItem::new(symbol, "TERM"),
            StackItem::new(symbol, "NUM_EXPR_AUX"),
            StackItem::new(action, SemanticAction::GenerateExpressionNode),
        ]);
        
        table.insert("TERM-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL", vec![
            StackItem::new(symbol, "UNARY_EXPR"),
            StackItem::new(symbol, "UNARY_EXPR_AUX"),
            StackItem::new(action, SemanticAction::TermEvaluation),
        ]);
        
        table.insert("UNARY_EXPR_AUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_BRACKET-RELOP-PLUS-MINUS", vec![
            StackItem::new(symbol, "ε"),
        ]);
        
        table.insert("UNARY_EXPR_AUX-MULTIPLICATION", vec![
            StackItem::new(symbol, "MULTIPLICATION"),
            StackItem::new(symbol, "TERM"),
            StackItem::new(action, SemanticAction::SetOperation),
        ]);
        
        table.insert("UNARY_EXPR_AUX-DIVISION", vec![
            StackItem::new(symbol, "DIVISION"),
            StackItem::new(symbol, "TERM"),
            StackItem::new(action, SemanticAction::SetOperation),
        ]);
        
        table.insert("UNARY_EXPR_AUX-MODULUS", vec![
            StackItem::new(symbol, "MODULUS"),
            StackItem::new(symbol, "TERM"),
            StackItem::new(action, SemanticAction::SetOperation),
        ]);
        
        table.insert("UNARY_EXPR-IDENT-OPEN_PARENTHESIS-NI-NPF-STRC-NULL", vec![
            StackItem::new(symbol, "FACTOR"),
            StackItem::new(action, SemanticAction::ValuePropagation),
        ]);
        
        table.insert("UNARY_EXPR-PLUS", vec![
            StackItem::new(symbol, "PLUS"),
            StackItem::new(symbol, "FACTOR"),
            StackItem::new(action, SemanticAction::PositiveValue),
        ]);
        
        table.insert("UNARY_EXPR-MINUS", vec![
            StackItem::new(symbol, "MINUS"),
            StackItem::new(symbol, "FACTOR"),
            StackItem::new(action, SemanticAction::NegativeValue),
        ]);
        
        table.insert("FACTOR-IDENT", vec![
            StackItem::new(symbol, "LVALUE"),
            StackItem::new(action, SemanticAction::EvaluateIdentifier),
        ]);
        
        table.insert("FACTOR-OPEN_PARENTHESIS", vec![
            StackItem::new(symbol, "OPEN_PARENTHESIS"),
            StackItem::new(symbol, "NUM_EXPR"),
            StackItem::new(symbol, "CLOSE_PARENTHESIS"),
            StackItem::new(action, SemanticAction::PassThroughExpression),
        ]);
        
        table.insert("FACTOR-NI", vec![
            StackItem::new(symbol, "NI"),
            StackItem::new(action, SemanticAction::LexemeToValue),
        ]);
        
        table.insert("FACTOR-NPF", vec![
            StackItem::new(symbol, "NPF"),
            StackItem::new(action, SemanticAction::LexemeToValue),
        ]);
        
        table.insert("FACTOR-STRC", vec![
            StackItem::new(symbol, "STRC"),
            StackItem::new(action, SemanticAction::LexemeToValue),
        ]);
        
        table.insert("FACTOR-NULL", vec![
            StackItem::new(symbol, "NULL"),
        ]);
        
        table.insert("LVALUE-IDENT", vec![
            StackItem::new(symbol, "IDENT"),
            StackItem::new(action, SemanticAction::VerifyIdentifier),
            StackItem::new(action, SemanticAction::GetLValue),
            StackItem::new(action, SemanticAction::EvaluateIdentifier),
            StackItem::new(symbol, "ALLOC_AUX"),
            StackItem::new(action, SemanticAction::ArrayElementAccess),
        ]);

        table
    };
}