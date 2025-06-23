// parser_table.rs
use std::collections::HashMap;
use lazy_static::lazy_static;
use crate::parser_stack_item::{StackItem, StackItemType};
use crate::semantic_actions::SemanticAction;


lazy_static! {
    pub static ref SYNTAX_DIRECTED_TRANSLATION: HashMap<&'static str, Vec<StackItem>> = {
        let mut table = HashMap::new();
        
        // Regras de produção
        table.insert("PROGRAM-DEF", vec![
            StackItem::new_action(SemanticAction::ScopeInit),
            StackItem::new_action(SemanticAction::SetFinalLabel),
            StackItem::new_action(SemanticAction::InheritNext),
            StackItem::new_symbol("FUNCLIST"),
            StackItem::new_symbol("PROGRAM_TAIL"),
            StackItem::new_action(SemanticAction::GotoMain),
            StackItem::new_action(SemanticAction::GenerateFinalCode),
        ]);

        table.insert("PROGRAM-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR", vec![
            StackItem::new_action(SemanticAction::ScopeInit),
            StackItem::new_action(SemanticAction::SetFinalLabel),
            StackItem::new_action(SemanticAction::InheritNext),
            StackItem::new_symbol("STATEMENT"),
            StackItem::new_action(SemanticAction::CollectChildCode),
            StackItem::new_action(SemanticAction::GenerateFinalCode),
        ]);
        
        table.insert("PROGRAM-$", vec![
            StackItem::new_action(SemanticAction::ScopeInit),
            StackItem::new_symbol("ε"),
        ]);
        
        table.insert("PROGRAM_TAIL-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR", vec![
            StackItem::new_action(SemanticAction::InheritNext),
            StackItem::new_symbol("STATEMENT"),
            StackItem::new_action(SemanticAction::CollectChildCode),
        ]);
        
        table.insert("PROGRAM_TAIL-$", vec![
            StackItem::new_symbol("ε"),
        ]);
        
        table.insert("FUNCLIST-DEF", vec![
            StackItem::new_action(SemanticAction::InheritNext),
            StackItem::new_symbol("FUNCTION_DEF"),
            StackItem::new_symbol("FUNCLIST_TAIL"),
            StackItem::new_action(SemanticAction::CollectChildCode),
        ]);
        
        table.insert("FUNCLIST_TAIL-DEF", vec![
            StackItem::new_action(SemanticAction::InheritNext),
            StackItem::new_symbol("FUNCLIST"),
            StackItem::new_action(SemanticAction::CollectChildCode),
        ]);
        
        table.insert("FUNCLIST_TAIL-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$", vec![
            StackItem::new_symbol("ε"),
        ]);
        
        table.insert("FUNCTION_DEF-DEF", vec![
            StackItem::new_symbol("DEF"),
            StackItem::new_symbol("IDENT"),
            StackItem::new_action(SemanticAction::FunctionHeader),
            StackItem::new_action(SemanticAction::ScopeEnter),
            StackItem::new_action(SemanticAction::DeclareFunction),
            StackItem::new_action(SemanticAction::ScopePush),
            StackItem::new_symbol("OPEN_PARENTHESIS"),
            StackItem::new_symbol("PARAM_LIST"),
            StackItem::new_symbol("CLOSE_PARENTHESIS"),
            StackItem::new_symbol("OPEN_BRACE"),
            StackItem::new_symbol("STATEMENT_LIST"),
            StackItem::new_symbol("CLOSE_BRACE"),
            StackItem::new_action(SemanticAction::ScopeExit),
            StackItem::new_action(SemanticAction::FunctionFooter),
        ]);
        
        table.insert("PARAM_LIST-CLOSE_PARENTHESIS", vec![
            StackItem::new_symbol("ε"),
        ]);
        
        table.insert("PARAM_LIST-Int", vec![
            StackItem::new_symbol("INT"),
            StackItem::new_symbol("IDENT"),
            StackItem::new_action(SemanticAction::ScopeEnter),
            StackItem::new_action(SemanticAction::DeclareIntParam),
            StackItem::new_symbol("PARAM_LIST_TAIL"),
        ]);
        
        table.insert("PARAM_LIST-FLOAT", vec![
            StackItem::new_symbol("FLOAT"),
            StackItem::new_symbol("IDENT"),
            StackItem::new_action(SemanticAction::ScopeEnter),
            StackItem::new_action(SemanticAction::DeclareFloatParam),
            StackItem::new_symbol("PARAM_LIST_TAIL"),
        ]);
        
        table.insert("PARAM_LIST-STRING", vec![
            StackItem::new_symbol("STRING"),
            StackItem::new_symbol("IDENT"),
            StackItem::new_action(SemanticAction::ScopeEnter),
            StackItem::new_action(SemanticAction::DeclareStringParam),
            StackItem::new_symbol("PARAM_LIST_TAIL"),
        ]);
        
        table.insert("PARAM_LIST_TAIL-CLOSE_PARENTHESIS", vec![
            StackItem::new_symbol("ε"),
        ]);
        
        table.insert("PARAM_LIST_TAIL-COMMA", vec![
            StackItem::new_symbol("COMMA"),
            StackItem::new_symbol("PARAM_LIST"),
        ]);
        
        table.insert("STATEMENT-IDENT", vec![
            StackItem::new_action(SemanticAction::InheritNext),
            StackItem::new_symbol("ASSIGN_STAT"),
            StackItem::new_symbol("SEMICOLON"),
            StackItem::new_action(SemanticAction::CollectChildCode),
        ]);
        
        table.insert("STATEMENT-OPEN_BRACE", vec![
            StackItem::new_action(SemanticAction::InheritNext),
            StackItem::new_symbol("OPEN_BRACE"),
            StackItem::new_action(SemanticAction::ScopeEnter),
            StackItem::new_symbol("STATEMENT_LIST"),
            StackItem::new_symbol("CLOSE_BRACE"),
            StackItem::new_action(SemanticAction::ScopeExit),
            StackItem::new_action(SemanticAction::CollectChildCode),
        ]);
        
        table.insert("STATEMENT-INT-FLOAT-STRING", vec![
            StackItem::new_symbol("VARIABLE_DECL"),
            StackItem::new_symbol("SEMICOLON"),
            StackItem::new_action(SemanticAction::CollectChildCode),
        ]);
        
        table.insert("STATEMENT-SEMICOLON", vec![
            StackItem::new_symbol("SEMICOLON"),
        ]);
        
        table.insert("STATEMENT-BREAK", vec![
            StackItem::new_symbol("BREAK"),
            StackItem::new_action(SemanticAction::HandleBreak),
            StackItem::new_symbol("SEMICOLON"),
            StackItem::new_action(SemanticAction::GenerateBreakCode),
        ]);
        
        table.insert("STATEMENT-PRINT", vec![
            StackItem::new_action(SemanticAction::InheritNext),
            StackItem::new_symbol("PRINT_STAT"),
            StackItem::new_symbol("SEMICOLON"),
            StackItem::new_action(SemanticAction::CollectChildCode),
        ]);
        
        table.insert("STATEMENT-READ", vec![
            StackItem::new_action(SemanticAction::InheritNext),
            StackItem::new_symbol("READ_STAT"),
            StackItem::new_symbol("SEMICOLON"),
            StackItem::new_action(SemanticAction::CollectChildCode),
        ]);
        
        table.insert("STATEMENT-RETURN", vec![
            StackItem::new_action(SemanticAction::InheritNext),
            StackItem::new_symbol("RETURN_STAT"),
            StackItem::new_symbol("SEMICOLON"),
            StackItem::new_action(SemanticAction::CollectChildCode),
        ]);
        
        table.insert("STATEMENT-IF", vec![
            StackItem::new_action(SemanticAction::InheritNext),
            StackItem::new_symbol("IF_STAT"),
            StackItem::new_action(SemanticAction::CollectChildCode),
        ]);
        
        table.insert("STATEMENT-FOR", vec![
            StackItem::new_action(SemanticAction::InheritNext),
            StackItem::new_symbol("FOR_STAT"),
            StackItem::new_action(SemanticAction::CollectChildCode),
        ]);
        
        table.insert("INDEX-SEMICOLON", vec![
            StackItem::new_symbol("ε"),
            StackItem::new_action(SemanticAction::FinalizeArrayDecl),
        ]);
        
        table.insert("INDEX-OPEN_BRACKET", vec![
            StackItem::new_symbol("OPEN_BRACKET"),
            StackItem::new_symbol("NUM_EXPR"),
            StackItem::new_symbol("CLOSE_BRACKET"),
            StackItem::new_action(SemanticAction::ProcessArrayDim),
            StackItem::new_symbol("INDEX"),
            StackItem::new_action(SemanticAction::FinalizeArrayDecl),
        ]);
        
        table.insert("VARIABLE_DECL-INT", vec![
            StackItem::new_action(SemanticAction::InheritNext),
            StackItem::new_symbol("INT"),
            StackItem::new_symbol("IDENT"),
            StackItem::new_action(SemanticAction::ScopeEnter),
            StackItem::new_action(SemanticAction::DeclareInt),
            StackItem::new_symbol("INDEX"),
            StackItem::new_action(SemanticAction::FinalizeDeclaration),
            StackItem::new_action(SemanticAction::CollectChildCode),
        ]);
        
        table.insert("VARIABLE_DECL-FLOAT", vec![
            StackItem::new_action(SemanticAction::InheritNext),
            StackItem::new_symbol("FLOAT"),
            StackItem::new_symbol("IDENT"),
            StackItem::new_action(SemanticAction::ScopeEnter),
            StackItem::new_action(SemanticAction::DeclareFloat),
            StackItem::new_symbol("INDEX"),
            StackItem::new_action(SemanticAction::FinalizeDeclaration),
            StackItem::new_action(SemanticAction::CollectChildCode),
        ]);
        
        table.insert("VARIABLE_DECL-STRING", vec![
            StackItem::new_action(SemanticAction::InheritNext),
            StackItem::new_symbol("STRING"),
            StackItem::new_symbol("IDENT"),
            StackItem::new_action(SemanticAction::ScopeEnter),
            StackItem::new_action(SemanticAction::DeclareString),
            StackItem::new_symbol("INDEX"),
            StackItem::new_action(SemanticAction::FinalizeDeclaration),
            StackItem::new_action(SemanticAction::CollectChildCode),
        ]);
        
        table.insert("ASSIGN_STAT-IDENT", vec![
            StackItem::new_action(SemanticAction::InheritNext),
            StackItem::new_symbol("LVALUE"),
            StackItem::new_symbol("ASSIGN"),
            StackItem::new_symbol("ASSIGN_TAIL"),
            StackItem::new_action(SemanticAction::GenerateAssignment),
        ]);
        
        table.insert("ASSIGN_TAIL-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL", vec![
            StackItem::new_symbol("EXPRESSION"),
            StackItem::new_action(SemanticAction::SetExpressionValue),
        ]);
        
        table.insert("ASSIGN_TAIL-CALL", vec![
            StackItem::new_action(SemanticAction::GetReturnValue),
            StackItem::new_action(SemanticAction::InheritNext),
            StackItem::new_symbol("FUNC_CALL"),
            StackItem::new_action(SemanticAction::CollectChildCode),
        ]);
        
        table.insert("ASSIGN_TAIL-NEW", vec![
            StackItem::new_symbol("ALLOC_EXPR"),
        ]);
        
        table.insert("FUNC_CALL-CALL", vec![
            StackItem::new_symbol("CALL"),
            StackItem::new_symbol("OPEN_PARENTHESIS"),
            StackItem::new_symbol("IDENT"),
            StackItem::new_action(SemanticAction::VerifyFunction),
            StackItem::new_symbol("OPEN_PARENTHESIS"),
            StackItem::new_symbol("PARAM_LIST_CALL"),
            StackItem::new_symbol("CLOSE_PARENTHESIS"),
            StackItem::new_symbol("CLOSE_PARENTHESIS"),
            StackItem::new_action(SemanticAction::GenerateFunctionCall),
        ]);
        
        table.insert("PARAM_LIST_CALL-IDENT", vec![
            StackItem::new_symbol("IDENT"),
            StackItem::new_action(SemanticAction::VerifyParameter),
            StackItem::new_symbol("PARAM_LIST_CALL_TAIL"),
            StackItem::new_action(SemanticAction::ProcessParameterList),
        ]);
        
        table.insert("PARAM_LIST_CALL-CLOSE_PARENTHESIS", vec![
            StackItem::new_symbol("ε"),
        ]);
        
        table.insert("PARAM_LIST_CALL_TAIL-CLOSE_PARENTHESIS", vec![
            StackItem::new_symbol("ε"),
        ]);
        
        table.insert("PARAM_LIST_CALL_TAIL-COMMA", vec![
            StackItem::new_symbol("COMMA"),
            StackItem::new_symbol("PARAM_LIST_CALL"),
            StackItem::new_action(SemanticAction::CollectParameters),
            StackItem::new_action(SemanticAction::CollectChildCode),
        ]);
        
        table.insert("PRINT_STAT-PRINT", vec![
            StackItem::new_symbol("PRINT"),
            StackItem::new_symbol("EXPRESSION"),
            StackItem::new_action(SemanticAction::GeneratePrint),
        ]);
        
        table.insert("READ_STAT-READ", vec![
            StackItem::new_symbol("READ"),
            StackItem::new_symbol("LVALUE"),
            StackItem::new_action(SemanticAction::GenerateRead),
        ]);
        
        table.insert("RETURN_STAT-RETURN", vec![
            StackItem::new_symbol("RETURN"),
            StackItem::new_symbol("RETURN_TAIL"),
            StackItem::new_action(SemanticAction::GenerateReturn),
        ]);
        
        table.insert("RETURN_TAIL-IDENT", vec![
            StackItem::new_symbol("LVALUE"),
            StackItem::new_action(SemanticAction::SetReturnValue),
        ]);
        
        table.insert("RETURN_TAIL-SEMICOLON", vec![
            StackItem::new_symbol("ε"),
        ]);
        
        table.insert("IF_STAT-IF", vec![
            StackItem::new_action(SemanticAction::InheritNext),
            StackItem::new_symbol("IF"),
            StackItem::new_symbol("OPEN_PARENTHESIS"),
            StackItem::new_symbol("EXPRESSION"),
            StackItem::new_symbol("CLOSE_PARENTHESIS"),
            StackItem::new_symbol("OPEN_BRACE"),
            StackItem::new_action(SemanticAction::ScopeEnterConditional),
            StackItem::new_symbol("STATEMENT"),
            StackItem::new_symbol("CLOSE_BRACE"),
            StackItem::new_action(SemanticAction::ScopeExit),
            StackItem::new_symbol("IF_TAIL"),
            StackItem::new_action(SemanticAction::FinalizeIfStatement),
        ]);
        
        table.insert("IF_TAIL-IDENT-OPEN_BRACE-CLOSE_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$", vec![
            StackItem::new_action(SemanticAction::HandleEmptyElse),
            StackItem::new_symbol("ε"),
        ]);
        
        table.insert("IF_TAIL-ELSE", vec![
            StackItem::new_action(SemanticAction::ElseHeader),
            StackItem::new_symbol("ELSE"),
            StackItem::new_symbol("OPEN_BRACE"),
            StackItem::new_action(SemanticAction::ScopeEnterConditional),
            StackItem::new_symbol("STATEMENT"),
            StackItem::new_symbol("CLOSE_BRACE"),
            StackItem::new_action(SemanticAction::ScopeExit),
            StackItem::new_action(SemanticAction::ElseFooter),
        ]);
        
        table.insert("FOR_STAT-FOR", vec![
            StackItem::new_action(SemanticAction::InheritNext),
            StackItem::new_action(SemanticAction::InheritForLabels),
            StackItem::new_symbol("FOR"),
            StackItem::new_symbol("OPEN_PARENTHESIS"),
            StackItem::new_symbol("ASSIGN_STAT"),
            StackItem::new_symbol("SEMICOLON"),
            StackItem::new_symbol("EXPRESSION"),
            StackItem::new_symbol("SEMICOLON"),
            StackItem::new_symbol("ASSIGN_STAT"),
            StackItem::new_symbol("CLOSE_PARENTHESIS"),
            StackItem::new_action(SemanticAction::ScopeEnterLoop),
            StackItem::new_symbol("STATEMENT"),
            StackItem::new_action(SemanticAction::ScopeExit),
            StackItem::new_action(SemanticAction::FinalizeForLoop),
        ]);
        
        table.insert("STATEMENT_LIST-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR", vec![
            StackItem::new_action(SemanticAction::InheritNext),
            StackItem::new_action(SemanticAction::SetLabel),
            StackItem::new_symbol("STATEMENT"),
            StackItem::new_symbol("STATEMENT_LIST_TAIL"),
            StackItem::new_action(SemanticAction::CollectChildCode),
        ]);
        
        table.insert("STATEMENT_LIST_TAIL-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREak-PRINT-READ-RETURN-IF-FOR", vec![
            StackItem::new_action(SemanticAction::InheritNext),
            StackItem::new_symbol("STATEMENT_LIST"),
            StackItem::new_action(SemanticAction::CollectChildCode),
        ]);
        
        table.insert("STATEMENT_LIST_TAIL-CLOSE_BRACE", vec![
            StackItem::new_action(SemanticAction::InheritNext),
            StackItem::new_symbol("ε"),
            StackItem::new_action(SemanticAction::CollectChildCode),
        ]);
        
        table.insert("ALLOC_AUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_BRACKET-ASSIGN-RELOP-PLUS-MINUS-MULTIPLICATION-DIVISION-MODULUS", vec![
            StackItem::new_symbol("ε"),
        ]);
        
        table.insert("ALLOC_AUX-OPEN_BRACKET", vec![
            StackItem::new_symbol("OPEN_BRACKET"),
            StackItem::new_symbol("NUM_EXPR"),
            StackItem::new_symbol("CLOSE_BRACKET"),
            StackItem::new_symbol("ALLOC_AUX"),
            StackItem::new_action(SemanticAction::ProcessArrayDim),
            StackItem::new_action(SemanticAction::IncrementArrayCounter),
        ]);
        
        table.insert("ALLOC_EXPR-NEW", vec![
            StackItem::new_symbol("NEW"),
            StackItem::new_symbol("ALLOC_EXPR_TAIL"),
        ]);
        
        table.insert("ALLOC_EXPR_TAIL-INT", vec![
            StackItem::new_symbol("INT"),
            StackItem::new_symbol("OPEN_BRACKET"),
            StackItem::new_symbol("NUM_EXPR"),
            StackItem::new_symbol("CLOSE_BRACKET"),
            StackItem::new_symbol("ALLOC_AUX"),
            StackItem::new_action(SemanticAction::GenerateAllocation),
        ]);
        
        table.insert("ALLOC_EXPR_TAIL-FLOAT", vec![
            StackItem::new_symbol("FLOAT"),
            StackItem::new_symbol("OPEN_BRACKET"),
            StackItem::new_symbol("NUM_EXPR"),
            StackItem::new_symbol("CLOSE_BRACKET"),
            StackItem::new_symbol("ALLOC_AUX"),
            StackItem::new_action(SemanticAction::GenerateAllocation),
        ]);
        
        table.insert("ALLOC_EXPR_TAIL-STRING", vec![
            StackItem::new_symbol("STRING"),
            StackItem::new_symbol("OPEN_BRACKET"),
            StackItem::new_symbol("NUM_EXPR"),
            StackItem::new_symbol("CLOSE_BRACKET"),
            StackItem::new_symbol("ALLOC_AUX"),
            StackItem::new_action(SemanticAction::GenerateAllocation),
        ]);
        
        table.insert("EXPRESSION-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL", vec![
            StackItem::new_symbol("NUM_EXPR"),
            StackItem::new_action(SemanticAction::ExpressionHeader),
            StackItem::new_symbol("EXPRESSION_TAIL"),
            StackItem::new_action(SemanticAction::ExpressionFooter),
        ]);
        
        table.insert("EXPRESSION_TAIL-CLOSE_PARENTHESIS-SEMICOLON", vec![
            StackItem::new_symbol("ε"),
        ]);
        
        table.insert("EXPRESSION_TAIL-RELOP", vec![
            StackItem::new_symbol("RELOP"),
            StackItem::new_symbol("NUM_EXPR"),
            StackItem::new_action(SemanticAction::GenerateRelationalOp),
            StackItem::new_action(SemanticAction::HandleRelationalOp),
        ]);
        
        table.insert("NUM_EXPR_AUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_BRACKET-RELOP", vec![
            StackItem::new_symbol("ε"),
        ]);
        
        table.insert("NUM_EXPR_AUX-PLUS", vec![
            StackItem::new_symbol("PLUS"),
            StackItem::new_symbol("NUM_EXPR"),
            StackItem::new_action(SemanticAction::SetOperation),
        ]);
        
        table.insert("NUM_EXPR_AUX-MINUS", vec![
            StackItem::new_symbol("MINUS"),
            StackItem::new_symbol("NUM_EXPR"),
            StackItem::new_action(SemanticAction::SetOperation),
        ]);
        
        table.insert("NUM_EXPR-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL", vec![
            StackItem::new_symbol("TERM"),
            StackItem::new_symbol("NUM_EXPR_AUX"),
            StackItem::new_action(SemanticAction::GenerateExpressionNode),
        ]);
        
        table.insert("TERM-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL", vec![
            StackItem::new_symbol("UNARY_EXPR"),
            StackItem::new_symbol("UNARY_EXPR_AUX"),
            StackItem::new_action(SemanticAction::TermEvaluation),
        ]);
        
        table.insert("UNARY_EXPR_AUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_BRACKET-RELOP-PLUS-MINUS", vec![
            StackItem::new_symbol("ε"),
        ]);
        
        table.insert("UNARY_EXPR_AUX-MULTIPLICATION", vec![
            StackItem::new_symbol("MULTIPLICATION"),
            StackItem::new_symbol("TERM"),
            StackItem::new_action(SemanticAction::SetOperation),
        ]);
        
        table.insert("UNARY_EXPR_AUX-DIVISION", vec![
            StackItem::new_symbol("DIVISION"),
            StackItem::new_symbol("TERM"),
            StackItem::new_action(SemanticAction::SetOperation),
        ]);
        
        table.insert("UNARY_EXPR_AUX-MODULUS", vec![
            StackItem::new_symbol("MODULUS"),
            StackItem::new_symbol("TERM"),
            StackItem::new_action(SemanticAction::SetOperation),
        ]);
        
        table.insert("UNARY_EXPR-IDENT-OPEN_PARENTHESIS-NI-NPF-STRC-NULL", vec![
            StackItem::new_symbol("FACTOR"),
            StackItem::new_action(SemanticAction::ValuePropagation),
        ]);
        
        table.insert("UNARY_EXPR-PLUS", vec![
            StackItem::new_symbol("PLUS"),
            StackItem::new_symbol("FACTOR"),
            StackItem::new_action(SemanticAction::PositiveValue),
        ]);
        
        table.insert("UNARY_EXPR-MINUS", vec![
            StackItem::new_symbol("MINUS"),
            StackItem::new_symbol("FACTOR"),
            StackItem::new_action(SemanticAction::NegativeValue),
        ]);
        
        table.insert("FACTOR-IDENT", vec![
            StackItem::new_symbol("LVALUE"),
            StackItem::new_action(SemanticAction::EvaluateIdentifier),
        ]);
        
        table.insert("FACTOR-OPEN_PARENTHESIS", vec![
            StackItem::new_symbol("OPEN_PARENTHESIS"),
            StackItem::new_symbol("NUM_EXPR"),
            StackItem::new_symbol("CLOSE_PARENTHESIS"),
            StackItem::new_action(SemanticAction::PassThroughExpression),
        ]);
        
        table.insert("FACTOR-NI", vec![
            StackItem::new_symbol("NI"),
            StackItem::new_action(SemanticAction::LexemeToValue),
        ]);
        
        table.insert("FACTOR-NPF", vec![
            StackItem::new_symbol("NPF"),
            StackItem::new_action(SemanticAction::LexemeToValue),
        ]);
        
        table.insert("FACTOR-STRC", vec![
            StackItem::new_symbol("STRC"),
            StackItem::new_action(SemanticAction::LexemeToValue),
        ]);
        
        table.insert("FACTOR-NULL", vec![
            StackItem::new_symbol("NULL"),
        ]);
        
        table.insert("LVALUE-IDENT", vec![
            StackItem::new_symbol("IDENT"),
            StackItem::new_action(SemanticAction::VerifyIdentifier),
            StackItem::new_action(SemanticAction::GetLValue),
            StackItem::new_action(SemanticAction::EvaluateIdentifier),
            StackItem::new_symbol("ALLOC_AUX"),
            StackItem::new_action(SemanticAction::ArrayElementAccess),
        ]);

        table
    };
}



lazy_static! {
    pub static ref PARSE_TABLE: HashMap<&'static str, HashMap<&'static str, &'static Vec<StackItem>>> = {
        let mut table = HashMap::new();
        
        macro_rules! add_entry {
            ($non_term:expr, $term:expr, $key:expr) => {
                table.entry($non_term)
                    .or_insert_with(HashMap::new)
                    .insert($term, SYNTAX_DIRECTED_TRANSLATION.get($key).unwrap());
            };
        }

        add_entry!("PROGRAM", "DEF", "PROGRAM-DEF");
        add_entry!("PROGRAM", "IDENT", "PROGRAM-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR");
        add_entry!("PROGRAM", "OPEN_BRACE", "PROGRAM-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR");
        add_entry!("PROGRAM", "INT", "PROGRAM-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR");
        add_entry!("PROGRAM", "FLOAT", "PROGRAM-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR");
        add_entry!("PROGRAM", "STRING", "PROGRAM-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR");
        add_entry!("PROGRAM", "SEMICOLON", "PROGRAM-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR");
        add_entry!("PROGRAM", "BREAK", "PROGRAM-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR");
        add_entry!("PROGRAM", "PRINT", "PROGRAM-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR");
        add_entry!("PROGRAM", "READ", "PROGRAM-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR");
        add_entry!("PROGRAM", "RETURN", "PROGRAM-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR");
        add_entry!("PROGRAM", "IF", "PROGRAM-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR");
        add_entry!("PROGRAM", "FOR", "PROGRAM-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR");
        add_entry!("PROGRAM", "$", "PROGRAM-$");
        
        // PROGRAM_TAIL
        add_entry!("PROGRAM'", "IDENT", "PROGRAM'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR");
        add_entry!("PROGRAM'", "OPEN_BRACE", "PROGRAM'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR");
        add_entry!("PROGRAM'", "INT", "PROGRAM'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR");
        add_entry!("PROGRAM'", "FLOAT", "PROGRAM'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR");
        add_entry!("PROGRAM'", "STRING", "PROGRAM'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR");
        add_entry!("PROGRAM'", "SEMICOLON", "PROGRAM'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR");
        add_entry!("PROGRAM'", "BREAK", "PROGRAM'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR");
        add_entry!("PROGRAM'", "PRINT", "PROGRAM'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR");
        add_entry!("PROGRAM'", "READ", "PROGRAM'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR");
        add_entry!("PROGRAM'", "RETURN", "PROGRAM'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR");
        add_entry!("PROGRAM'", "IF", "PROGRAM'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR");
        add_entry!("PROGRAM'", "FOR", "PROGRAM'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR");
        add_entry!("PROGRAM'", "$", "PROGRAM'-$");

        // FUNCLIST
        add_entry!("FUNCLIST", "DEF", "FUNCLIST-DEF");
        
        // FUNCLIST_TAIL
        add_entry!("FUNCLIST'", "DEF", "FUNCLIST'-DEF");
        add_entry!("FUNCLIST'", "IDENT", "FUNCLIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$");
        add_entry!("FUNCLIST'", "OPEN_BRACE", "FUNCLIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$");
        add_entry!("FUNCLIST'", "INT", "FUNCLIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$");
        add_entry!("FUNCLIST'", "FLOAT", "FUNCLIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$");
        add_entry!("FUNCLIST'", "STRING", "FUNCLIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$");
        add_entry!("FUNCLIST'", "SEMICOLON", "FUNCLIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$");
        add_entry!("FUNCLIST'", "BREAK", "FUNCLIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$");
        add_entry!("FUNCLIST'", "PRINT", "FUNCLIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$");
        add_entry!("FUNCLIST'", "READ", "FUNCLIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$");
        add_entry!("FUNCLIST'", "RETURN", "FUNCLIST'-IDENT-OPEN_BRACE-INT-Float-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$");
        add_entry!("FUNCLIST'", "IF", "FUNCLIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$");
        add_entry!("FUNCLIST'", "FOR", "FUNCLIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$");
        add_entry!("FUNCLIST'", "$", "FUNCLIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$");

        // FUNCTION_DEF
        add_entry!("FUNCDEF", "DEF", "FUNCDEF-DEF");
        
        // PARAM_LIST
        add_entry!("PARAMLIST", "CLOSE_PARENTHESIS", "PARAMLIST-CLOSE_PARENTHESIS");
        add_entry!("PARAMLIST", "INT", "PARAMLIST-INT");
        add_entry!("PARAMLIST", "FLOAT", "PARAMLIST-FLOAT");
        add_entry!("PARAMLIST", "STRING", "PARAMLIST-STRING");
        
        // PARAM_LIST_TAIL
        add_entry!("PARAMLIST'", "CLOSE_PARENTHESIS", "PARAMLIST'-CLOSE_PARENTHESIS");
        add_entry!("PARAMLIST'", "COMMA", "PARAMLIST'-COMMA");
        
        // STATEMENT
        add_entry!("STATEMENT", "IDENT", "STATEMENT-IDENT");
        add_entry!("STATEMENT", "OPEN_BRACE", "STATEMENT-OPEN_BRACE");
        add_entry!("STATEMENT", "INT", "STATEMENT-INT-FLOAT-STRING");
        add_entry!("STATEMENT", "FLOAT", "STATEMENT-INT-FLOAT-STRING");
        add_entry!("STATEMENT", "STRING", "STATEMENT-INT-FLOAT-STRING");
        add_entry!("STATEMENT", "SEMICOLON", "STATEMENT-SEMICOLON");
        add_entry!("STATEMENT", "BREAK", "STATEMENT-BREAK");
        add_entry!("STATEMENT", "PRINT", "STATEMENT-PRINT");
        add_entry!("STATEMENT", "READ", "STATEMENT-READ");
        add_entry!("STATEMENT", "RETURN", "STATEMENT-RETURN");
        add_entry!("STATEMENT", "IF", "STATEMENT-IF");
        add_entry!("STATEMENT", "FOR", "STATEMENT-FOR");
        
        // INDEX
        add_entry!("INDEX", "SEMICOLON", "INDEX-SEMICOLON");
        add_entry!("INDEX", "OPEN_BRACKET", "INDEX-OPEN_BRACKET");
        
        // VARIABLE_DECL
        add_entry!("VARDECL", "INT", "VARDECL-INT");
        add_entry!("VARDECL", "FLOAT", "VARDECL-FLOAT");
        add_entry!("VARDECL", "STRING", "VARDECL-STRING");
        
        // ASSIGN_STAT
        add_entry!("ATRIBSTAT", "IDENT", "ATRIBSTAT-IDENT");
        
        // ASSIGN_TAIL
        add_entry!("ATRIBSTAT'", "IDENT", "ATRIBSTAT'-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL");
        add_entry!("ATRIBSTAT'", "OPEN_PARENTHESIS", "ATRIBSTAT'-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL");
        add_entry!("ATRIBSTAT'", "NI", "ATRIBSTAT'-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL");
        add_entry!("ATRIBSTAT'", "PLUS", "ATRIBSTAT'-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL");
        add_entry!("ATRIBSTAT'", "MINUS", "ATRIBSTAT'-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL");
        add_entry!("ATRIBSTAT'", "NPF", "ATRIBSTAT'-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL");
        add_entry!("ATRIBSTAT'", "STRC", "ATRIBSTAT'-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL");
        add_entry!("ATRIBSTAT'", "NULL", "ATRIBSTAT'-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL");
        add_entry!("ATRIBSTAT'", "CALL", "ATRIBSTAT'-CALL");
        add_entry!("ATRIBSTAT'", "NEW", "ATRIBSTAT'-NEW");
        
        // FUNC_CALL
        add_entry!("FUNCCALL", "CALL", "FUNCCALL-CALL");
        
        // PARAM_LIST_CALL
        add_entry!("PARAMLISTCALL", "IDENT", "PARAMLISTCALL-IDENT");
        add_entry!("PARAMLISTCALL", "CLOSE_PARENTHESIS", "PARAMLISTCALL-CLOSE_PARENTHESIS");
        
        // PARAM_LIST_CALL_TAIL
        add_entry!("PARAMLISTCALL'", "CLOSE_PARENTHESIS", "PARAMLISTCALL'-CLOSE_PARENTHESIS");
        add_entry!("PARAMLISTCALL'", "COMMA", "PARAMLISTCALL'-COMMA");
        
        // PRINT_STAT
        add_entry!("PRINTSTAT", "PRINT", "PRINTSTAT-PRINT");
        
        // READ_STAT
        add_entry!("READSTAT", "READ", "READSTAT-READ");
        
        // RETURN_STAT
        add_entry!("RETURNSTAT", "RETURN", "RETURNSTAT-RETURN");
        
        // RETURN_TAIL
        add_entry!("RETURNSTAT'", "IDENT", "RETURNSTAT'-IDENT");
        add_entry!("RETURNSTAT'", "SEMICOLON", "RETURNSTAT'-SEMICOLON");
        
        // IF_STAT
        add_entry!("IFSTAT", "IF", "IFSTAT-IF");
        
        // IF_TAIL
        add_entry!("IFSTAT'", "IDENT", "IFSTAT'-IDENT-OPEN_BRACE-CLOSE_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$");
        add_entry!("IFSTAT'", "OPEN_BRACE", "IFSTAT'-IDENT-OPEN_BRACE-CLOSE_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$");
        add_entry!("IFSTAT'", "CLOSE_BRACE", "IFSTAT'-IDENT-OPEN_BRACE-CLOSE_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$");
        add_entry!("IFSTAT'", "INT", "IFSTAT'-IDENT-OPEN_BRACE-CLOSE_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$");
        add_entry!("IFSTAT'", "FLOAT", "IFSTAT'-IDENT-OPEN_BRACE-CLOSE_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$");
        add_entry!("IFSTAT'", "STRING", "IFSTAT'-IDENT-OPEN_BRACE-CLOSE_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$");
        add_entry!("IFSTAT'", "SEMICOLON", "IFSTAT'-IDENT-OPEN_BRACE-CLOSE_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$");
        add_entry!("IFSTAT'", "BREAK", "IFSTAT'-IDENT-OPEN_BRACE-CLOSE_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$");
        add_entry!("IFSTAT'", "PRINT", "IFSTAT'-IDENT-OPEN_BRACE-CLOSE_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$");
        add_entry!("IFSTAT'", "READ", "IFSTAT'-IDENT-OPEN_BRACE-CLOSE_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$");
        add_entry!("IFSTAT'", "RETURN", "IFSTAT'-IDENT-OPEN_BRACE-CLOSE_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$");
        add_entry!("IFSTAT'", "IF", "IFSTAT'-IDENT-OPEN_BRACE-CLOSE_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$");
        add_entry!("IFSTAT'", "FOR", "IFSTAT'-IDENT-OPEN_BRACE-CLOSE_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$");
        add_entry!("IFSTAT'", "$", "IFSTAT'-IDENT-OPEN_BRACE-CLOSE_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$");
        add_entry!("IFSTAT'", "ELSE", "IFSTAT'-ELSE");
        
        // FOR_STAT
        add_entry!("FORSTAT", "FOR", "FORSTAT-FOR");
        
        // STATEMENT_LIST
        add_entry!("STATELIST", "IDENT", "STATELIST-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR");
        add_entry!("STATELIST", "OPEN_BRACE", "STATELIST-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR");
        add_entry!("STATELIST", "INT", "STATELIST-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR");
        add_entry!("STATELIST", "FLOAT", "STATELIST-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR");
        add_entry!("STATELIST", "STRING", "STATELIST-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR");
        add_entry!("STATELIST", "SEMICOLON", "STATELIST-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR");
        add_entry!("STATELIST", "BREAK", "STATELIST-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR");
        add_entry!("STATELIST", "PRINT", "STATELIST-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR");
        add_entry!("STATELIST", "READ", "STATELIST-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR");
        add_entry!("STATELIST", "RETURN", "STATELIST-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR");
        add_entry!("STATELIST", "IF", "STATELIST-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR");
        add_entry!("STATELIST", "FOR", "STATELIST-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR");
        
        // STATEMENT_LIST_TAIL
        add_entry!("STATELIST'", "IDENT", "STATELIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR");
        add_entry!("STATELIST'", "OPEN_BRACE", "STATELIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR");
        add_entry!("STATELIST'", "INT", "STATELIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR");
        add_entry!("STATELIST'", "FLOAT", "STATELIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR");
        add_entry!("STATELIST'", "STRING", "STATELIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR");
        add_entry!("STATELIST'", "SEMICOLON", "STATELIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR");
        add_entry!("STATELIST'", "BREAK", "STATELIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR");
        add_entry!("STATELIST'", "PRINT", "STATELIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR");
        add_entry!("STATELIST'", "READ", "STATELIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR");
        add_entry!("STATELIST'", "RETURN", "STATELIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-Read-RETURN-IF-FOR");
        add_entry!("STATELIST'", "IF", "STATELIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR");
        add_entry!("STATELIST'", "FOR", "STATELIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR");
        add_entry!("STATELIST'", "CLOSE_BRACE", "STATELIST'-CLOSE_BRACE");
        
        // ALLOC_AUX
        add_entry!("ALLOCAUX", "CLOSE_PARENTHESIS", "ALLOCAUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_BRACKET-ASSIGN-RELOP-PLUS-MINUS-MULTIPLICATION-DIVISION-MODULUS");
        add_entry!("ALLOCAUX", "SEMICOLON", "ALLOCAUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_BRACKET-ASSIGN-RELOP-PLUS-MINUS-MULTIPLICATION-DIVISION-MODULUS");
        add_entry!("ALLOCAUX", "CLOSE_BRACKET", "ALLOCAUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_BRACKET-ASSIGN-RELOP-PLUS-MINUS-MULTIPLICATION-DIVISION-MODULUS");
        add_entry!("ALLOCAUX", "ASSIGN", "ALLOCAUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_BRACKET-ASSIGN-RELOP-PLUS-MINUS-MULTIPLICATION-DIVISION-MODULUS");
        add_entry!("ALLOCAUX", "RELOP", "ALLOCAUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_BRACKET-ASSIGN-RELOP-PLUS-MINUS-MULTIPLICATION-DIVISION-MODULUS");
        add_entry!("ALLOCAUX", "PLUS", "ALLOCAUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_BRACKET-ASSIGN-RELOP-PLUS-MINUS-MULTIPLICATION-DIVISION-MODULUS");
        add_entry!("ALLOCAUX", "MINUS", "ALLOCAUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_BRACKET-ASSIGN-RELOP-PLUS-MINUS-MULTIPLICATION-DIVISION-MODULUS");
        add_entry!("ALLOCAUX", "MULTIPLICATION", "ALLOCAUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_BRACKET-ASSIGN-RELOP-PLUS-MINUS-MULTIPLICATION-DIVISION-MODULUS");
        add_entry!("ALLOCAUX", "DIVISION", "ALLOCAUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_BRACKET-ASSIGN-RELOP-PLUS-MINUS-MULTIPLICATION-DIVISION-MODULUS");
        add_entry!("ALLOCAUX", "MODULUS", "ALLOCAUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_BRACKET-ASSIGN-RELOP-PLUS-MINUS-MULTIPLICATION-DIVISION-MODULUS");
        add_entry!("ALLOCAUX", "OPEN_BRACKET", "ALLOCAUX-OPEN_BRACKET");
        
        // ALLOC_EXPR
        add_entry!("ALLOCEXPRESSION", "NEW", "ALLOCEXPRESSION-NEW");
        
        // ALLOC_EXPR_TAIL
        add_entry!("ALLOCEXPRESSION'", "INT", "ALLOCEXPRESSION'-INT");
        add_entry!("ALLOCEXPRESSION'", "FLOAT", "ALLOCEXPRESSION'-FLOAT");
        add_entry!("ALLOCEXPRESSION'", "STRING", "ALLOCEXPRESSION'-STRING");
        
        // EXPRESSION
        add_entry!("EXPRESSION", "IDENT", "EXPRESSION-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL");
        add_entry!("EXPRESSION", "OPEN_PARENTHESIS", "EXPRESSION-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL");
        add_entry!("EXPRESSION", "NI", "EXPRESSION-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL");
        add_entry!("EXPRESSION", "PLUS", "EXPRESSION-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL");
        add_entry!("EXPRESSION", "MINUS", "EXPRESSION-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL");
        add_entry!("EXPRESSION", "NPF", "EXPRESSION-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL");
        add_entry!("EXPRESSION", "STRC", "EXPRESSION-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL");
        add_entry!("EXPRESSION", "NULL", "EXPRESSION-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL");
        
        // EXPRESSION_TAIL
        add_entry!("EXPRESSION'", "CLOSE_PARENTHESIS", "EXPRESSION'-CLOSE_PARENTHESIS-SEMICOLON");
        add_entry!("EXPRESSION'", "SEMICOLON", "EXPRESSION'-CLOSE_PARENTHESIS-SEMICOLON");
        add_entry!("EXPRESSION'", "RELOP", "EXPRESSION'-RELOP");
        
        // NUM_EXPR_AUX
        add_entry!("NUMEXPRESSIONAUX", "CLOSE_PARENTHESIS", "NUMEXPRESSIONAUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_BRACKET-RELOP");
        add_entry!("NUMEXPRESSIONAUX", "SEMICOLON", "NUMEXPRESSIONAUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_BRACKET-RELOP");
        add_entry!("NUMEXPRESSIONAUX", "CLOSE_BRACKET", "NUMEXPRESSIONAUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_BRACKET-RELOP");
        add_entry!("NUMEXPRESSIONAUX", "RELOP", "NUMEXPRESSIONAUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_BRACKET-RELOP");
        add_entry!("NUMEXPRESSIONAUX", "PLUS", "NUMEXPRESSIONAUX-PLUS");
        add_entry!("NUMEXPRESSIONAUX", "MINUS", "NUMEXPRESSIONAUX-MINUS");
        
        // NUM_EXPR
        add_entry!("NUMEXPRESSION", "IDENT", "NUMEXPRESSION-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL");
        add_entry!("NUMEXPRESSION", "OPEN_PARENTHESIS", "NUMEXPRESSION-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL");
        add_entry!("NUMEXPRESSION", "NI", "NUMEXPRESSION-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL");
        add_entry!("NUMEXPRESSION", "PLUS", "NUMEXPRESSION-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL");
        add_entry!("NUMEXPRESSION", "MINUS", "NUMEXPRESSION-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL");
        add_entry!("NUMEXPRESSION", "NPF", "NUMEXPRESSION-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL");
        add_entry!("NUMEXPRESSION", "STRC", "NUMEXPRESSION-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL");
        add_entry!("NUMEXPRESSION", "NULL", "NUMEXPRESSION-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL");
        
        // TERM
        add_entry!("TERM", "IDENT", "TERM-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL");
        add_entry!("TERM", "OPEN_PARENTHESIS", "TERM-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL");
        add_entry!("TERM", "NI", "TERM-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL");
        add_entry!("TERM", "PLUS", "TERM-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL");
        add_entry!("TERM", "MINUS", "TERM-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL");
        add_entry!("TERM", "NPF", "TERM-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL");
        add_entry!("TERM", "STRC", "TERM-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL");
        add_entry!("TERM", "NULL", "TERM-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL");
        
        // UNARY_EXPR_AUX
        add_entry!("UNARYEXPRAUX", "CLOSE_PARENTHESIS", "UNARYEXPRAUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_BRACKET-RELOP-PLUS-MINUS");
        add_entry!("UNARYEXPRAUX", "SEMICOLON", "UNARYEXPRAUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_BRACKET-RELOP-PLUS-MINUS");
        add_entry!("UNARYEXPRAUX", "CLOSE_BRACKET", "UNARYEXPRAUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_BRACKET-RELOP-PLUS-MINUS");
        add_entry!("UNARYEXPRAUX", "RELOP", "UNARYEXPRAUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_BRACKET-RELOP-PLUS-MINUS");
        add_entry!("UNARYEXPRAUX", "PLUS", "UNARYEXPRAUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_BRACKET-RELOP-PLUS-MINUS");
        add_entry!("UNARYEXPRAUX", "MINUS", "UNARYEXPRAUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_BRACKET-RELOP-PLUS-MINUS");
        add_entry!("UNARYEXPRAUX", "MULTIPLICATION", "UNARYEXPRAUX-MULTIPLICATION");
        add_entry!("UNARYEXPRAUX", "DIVISION", "UNARYEXPRAUX-DIVISION");
        add_entry!("UNARYEXPRAUX", "MODULUS", "UNARYEXPRAUX-MODULUS");
        
        // UNARY_EXPR
        add_entry!("UNARYEXPR", "IDENT", "UNARYEXPR-IDENT-OPEN_PARENTHESIS-NI-NPF-STRC-NULL");
        add_entry!("UNARYEXPR", "OPEN_PARENTHESIS", "UNARYEXPR-IDENT-OPEN_PARENTHESIS-NI-NPF-STRC-NULL");
        add_entry!("UNARYEXPR", "NI", "UNARYEXPR-IDENT-OPEN_PARENTHESIS-NI-NPF-STRC-NULL");
        add_entry!("UNARYEXPR", "NPF", "UNARYEXPR-IDENT-OPEN_PARENTHESIS-NI-NPF-STRC-NULL");
        add_entry!("UNARYEXPR", "STRC", "UNARYEXPR-IDENT-OPEN_PARENTHESIS-NI-NPF-STRC-NULL");
        add_entry!("UNARYEXPR", "NULL", "UNARYEXPR-IDENT-OPEN_PARENTHESIS-NI-NPF-STRC-NULL");
        add_entry!("UNARYEXPR", "PLUS", "UNARYEXPR-PLUS");
        add_entry!("UNARYEXPR", "MINUS", "UNARYEXPR-MINUS");
        
        // FACTOR
        add_entry!("FACTOR", "IDENT", "FACTOR-IDENT");
        add_entry!("FACTOR", "OPEN_PARENTHESIS", "FACTOR-OPEN_PARENTHESIS");
        add_entry!("FACTOR", "NI", "FACTOR-NI");
        add_entry!("FACTOR", "NPF", "FACTOR-NPF");
        add_entry!("FACTOR", "STRC", "FACTOR-STRC");
        add_entry!("FACTOR", "NULL", "FACTOR-NULL");
        
        // LVALUE
        add_entry!("LVALUE", "IDENT", "LVALUE-IDENT");

        table
    };
}