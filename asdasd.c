#include "ast_node.h"

#include "errors.h"
#include "expression_tree_resolver.h"
void ASTNode::add_child(std::shared_ptr<ASTNode> node) {
    children.push_back(node);
}
void ASTNode::set_children(
    std::vector<std::shared_ptr<ASTNode>> &new_children) {
    children = new_children;
}

Node::Node(char operation, const std::string &value, std::shared_ptr<Node> &A)
    : operation(operation), value(value), A(A) {
    type = A->type;
}

Node::Node(char operation, const std::string &value, std::shared_ptr<Node> &A,
           std::shared_ptr<Node> &B)
    : operation(operation), value(value), A(A), B(B) {
    if (A->type == B->type) {
        type = A->type;
    } else {
        ExpressionTreeResolver::printError(operation, A, B);
        throw SemanticError("Type error on Expression tree construction");
    }
}

int ASTNode::label_count = 0;

std::string ASTNode::new_label() {
    label_count++;
    std::string label = "L";
    label += std::to_string(label_count);
    return label;
}

#ifndef __ast_node_h
#define __ast_node_h

#include <list>
#include <memory>
#include <string>
#include <vector>

#include "tokens.h"
#include "type_enum.h"
struct Node {
    char operation = 'N';
    std::string type;
    std::string value;
    std::shared_ptr<Node> A;
    std::shared_ptr<Node> B;
    Node(char operation, const std::string &value, std::shared_ptr<Node> &A,
         std::shared_ptr<Node> &B);
    Node(char operation, const std::string &value, std::shared_ptr<Node> &A);
    Node(char operation, std::string type, const std::string &value)
        : operation(operation), type(type), value(value) {}

    Node(char operation, const std::string &value)
        : operation(operation), value(value) {}
};

struct MathSDT {
    int array_counter = 0;
    std::shared_ptr<std::list<std::string>> arrayVar = std::make_shared<std::list<std::string>>();
    char operation;
    std::shared_ptr<Node> node;
};

struct DECSDT {
    std::string initial_type = "";
    std::string type = "";
};

struct Code {
    std::string var = "";
    std::string begin = "";

    std::string ret = "";
    std::string ret_val = "0";
    int param_cnt = 0;
};

struct ASTNode {
    std::string id;
    std::shared_ptr<Token> token;
    std::vector<std::shared_ptr<ASTNode>> children;

    std::shared_ptr<std::list<std::string>> code =
        std::make_shared<std::list<std::string>>();
    std::string nxt;
    std::string for_nxt;

    static int label_count;

    DECSDT decSDT;
    Code code_res_var;
    MathSDT mathSDT;

    ASTNode(const std::string &id) : id(id) {}
    ASTNode(const std::string &id,
            std::vector<std::shared_ptr<ASTNode>> &children)
        : id(id), children(children) {}
    void set_token(std::shared_ptr<Token> token) { this->token = token; }

    void add_child(std::shared_ptr<ASTNode> node);
    void set_children(std::vector<std::shared_ptr<ASTNode>> &children);

    std::string new_label();
};

#endif

// Arquivo gerado de forma semi automática pelo script em
// utils/LL-table-constructor.py

#include "auxiliar_sdt_table.h"

#include "semantic_actions.h"

static auto ACTION = StackItem::StackItemType::ACTION;
static auto SYMBOL = StackItem::StackItemType::SYMBOL;

const std::unordered_map<std::string, std::vector<StackItem>> sdt_table = {
    {"PROGRAM-DEF",
     {
         StackItem(ACTION, SCOPE::action1),
         StackItem(ACTION, CODE::set_final_label),
         StackItem(ACTION, CODE::inherit_next),
         StackItem(SYMBOL, "FUNCLIST"),
         StackItem(SYMBOL, "PROGRAM'"),
         StackItem(ACTION, CODE::goto_main),
         StackItem(ACTION, CODE::gen_final_code),
     }},
    {"PROGRAM-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-"
     "RETURN-IF-FOR",
     {
         StackItem(ACTION, SCOPE::action1),
         StackItem(ACTION, CODE::set_final_label),
         StackItem(ACTION, CODE::inherit_next),
         StackItem(SYMBOL, "STATEMENT"),
         StackItem(ACTION, CODE::get_code_from_childs),
         StackItem(ACTION, CODE::gen_final_code),
     }},
    {"PROGRAM-$",
     {
         StackItem(ACTION, SCOPE::action1),
         StackItem(SYMBOL, "ε"),
     }},
    {"PROGRAM'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-"
     "RETURN-IF-FOR",
     {
         StackItem(ACTION, CODE::inherit_next),
         StackItem(SYMBOL, "STATEMENT"),
         StackItem(ACTION, CODE::get_code_from_childs),
     }},
    {"PROGRAM'-$", {StackItem(SYMBOL, "ε")}},
    {"FUNCLIST-DEF",
     {
         StackItem(ACTION, CODE::inherit_next),
         StackItem(SYMBOL, "FUNCDEF"),
         StackItem(SYMBOL, "FUNCLIST'"),
         StackItem(ACTION, CODE::get_code_from_childs),
     }},
    {"FUNCLIST'-DEF",
     {
         StackItem(ACTION, CODE::inherit_next),
         StackItem(SYMBOL, "FUNCLIST"),
         StackItem(ACTION, CODE::get_code_from_childs),
     }},
    {"FUNCLIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-"
     "RETURN-IF-FOR-$",
     {
         StackItem(SYMBOL, "ε"),
     }},
    {"FUNCDEF-DEF",
     {
         StackItem(SYMBOL, "DEF"),
         StackItem(SYMBOL, "IDENT"),
         StackItem(ACTION, CODE::funcdef_h),
         StackItem(ACTION, SCOPE::action3),
         StackItem(ACTION, DEC::action1),
         StackItem(ACTION, SCOPE::action4),
         StackItem(SYMBOL, "OPEN_PARENTHESIS"),
         StackItem(SYMBOL, "PARAMLIST"),
         StackItem(SYMBOL, "CLOSE_PARENTHESIS"),
         StackItem(SYMBOL, "OPEN_BRACE"),
         StackItem(SYMBOL, "STATELIST"),
         StackItem(SYMBOL, "CLOSE_BRACE"),
         StackItem(ACTION, SCOPE::action2),
         StackItem(ACTION, CODE::funcdef_s),
     }},
    {"PARAMLIST-CLOSE_PARENTHESIS", {StackItem(SYMBOL, "ε")}},
    {"PARAMLIST-INT",
     {
         StackItem(SYMBOL, "INT"),
         StackItem(SYMBOL, "IDENT"),
         StackItem(ACTION, SCOPE::action3),
         StackItem(ACTION, DEC::action2),
         StackItem(SYMBOL, "PARAMLIST'"),
     }},
    {"PARAMLIST-FLOAT",
     {
         StackItem(SYMBOL, "FLOAT"),
         StackItem(SYMBOL, "IDENT"),
         StackItem(ACTION, SCOPE::action3),
         StackItem(ACTION, DEC::action3),
         StackItem(SYMBOL, "PARAMLIST'"),
     }},
    {"PARAMLIST-STRING",
     {
         StackItem(SYMBOL, "STRING"),
         StackItem(SYMBOL, "IDENT"),
         StackItem(ACTION, SCOPE::action3),
         StackItem(ACTION, DEC::action4),
         StackItem(SYMBOL, "PARAMLIST'"),
     }},
    {"PARAMLIST'-CLOSE_PARENTHESIS", {StackItem(SYMBOL, "ε")}},
    {"PARAMLIST'-COMMA",
     {
         StackItem(SYMBOL, "COMMA"),
         StackItem(SYMBOL, "PARAMLIST"),
     }},
    {"STATEMENT-IDENT",
     {
         StackItem(ACTION, CODE::inherit_next),
         StackItem(SYMBOL, "ATRIBSTAT"),
         StackItem(SYMBOL, "SEMICOLON"),
         StackItem(ACTION, CODE::get_code_from_childs),
     }},
    {"STATEMENT-OPEN_BRACE",
     {
         StackItem(ACTION, CODE::inherit_next),
         StackItem(SYMBOL, "OPEN_BRACE"),
         StackItem(ACTION, SCOPE::action1),
         StackItem(SYMBOL, "STATELIST"),
         StackItem(SYMBOL, "CLOSE_BRACE"),
         StackItem(ACTION, SCOPE::action2),
         StackItem(ACTION, CODE::get_code_from_childs),

     }},
    {"STATEMENT-INT-FLOAT-STRING",
     {
         StackItem(SYMBOL, "VARDECL"),
         StackItem(SYMBOL, "SEMICOLON"),
         StackItem(ACTION, CODE::get_code_from_childs),
     }},
    {"STATEMENT-SEMICOLON", {StackItem(SYMBOL, "SEMICOLON")}},
    {"STATEMENT-BREAK",
     {

         StackItem(SYMBOL, "BREAK"),
         StackItem(ACTION, BREAK::action1),
         StackItem(SYMBOL, "SEMICOLON"),
         StackItem(ACTION, CODE::handle_break),
     }},
    {"STATEMENT-PRINT",
     {
         StackItem(ACTION, CODE::inherit_next),
         StackItem(SYMBOL, "PRINTSTAT"),
         StackItem(SYMBOL, "SEMICOLON"),
         StackItem(ACTION, CODE::get_code_from_childs),
     }},
    {"STATEMENT-READ",
     {
         StackItem(ACTION, CODE::inherit_next),
         StackItem(SYMBOL, "READSTAT"),
         StackItem(SYMBOL, "SEMICOLON"),
         StackItem(ACTION, CODE::get_code_from_childs),
     }},
    {"STATEMENT-RETURN",
     {
         StackItem(ACTION, CODE::inherit_next),
         StackItem(SYMBOL, "RETURNSTAT"),
         StackItem(SYMBOL, "SEMICOLON"),
         StackItem(ACTION, CODE::get_code_from_childs),
     }},
    {"STATEMENT-IF",
     {
         StackItem(ACTION, CODE::inherit_next),
         StackItem(SYMBOL, "IFSTAT"),
         StackItem(ACTION, CODE::get_code_from_childs),
     }},
    {"STATEMENT-FOR",
     {
         StackItem(ACTION, CODE::inherit_next),
         StackItem(SYMBOL, "FORSTAT"),
         StackItem(ACTION, CODE::get_code_from_childs),
     }},
    {"INDEX-SEMICOLON",
     {
         StackItem(SYMBOL, "ε"),
         StackItem(ACTION, DEC::action7),
     }},
    {"INDEX-OPEN_BRACKET",
     {
         StackItem(SYMBOL, "OPEN_BRACKET"),
         StackItem(SYMBOL, "NI"),
         StackItem(SYMBOL, "CLOSE_BRACKET"),
         StackItem(ACTION, DEC::action5),
         StackItem(SYMBOL, "INDEX"),
         StackItem(ACTION, DEC::action6),
     }},
    {"VARDECL-INT",
     {

         StackItem(ACTION, CODE::inherit_next),
         StackItem(SYMBOL, "INT"),
         StackItem(SYMBOL, "IDENT"),
         StackItem(ACTION, SCOPE::action3),
         StackItem(ACTION, DEC::action8),
         StackItem(SYMBOL, "INDEX"),
         StackItem(ACTION, DEC::action11),
         StackItem(ACTION, CODE::get_code_from_childs),
     }},
    {"VARDECL-FLOAT",
     {
         StackItem(ACTION, CODE::inherit_next),
         StackItem(SYMBOL, "FLOAT"),
         StackItem(SYMBOL, "IDENT"),
         StackItem(ACTION, SCOPE::action3),
         StackItem(ACTION, DEC::action9),
         StackItem(SYMBOL, "INDEX"),
         StackItem(ACTION, DEC::action11),
         StackItem(ACTION, CODE::get_code_from_childs),
     }},
    {"VARDECL-STRING",
     {

         StackItem(ACTION, CODE::inherit_next),
         StackItem(SYMBOL, "STRING"),
         StackItem(SYMBOL, "IDENT"),
         StackItem(ACTION, SCOPE::action3),
         StackItem(ACTION, DEC::action10),
         StackItem(SYMBOL, "INDEX"),
         StackItem(ACTION, DEC::action11),
         StackItem(ACTION, CODE::get_code_from_childs),
     }},
    {"ATRIBSTAT-IDENT",
     {
         StackItem(ACTION, CODE::inherit_next),
         StackItem(SYMBOL, "LVALUE"),
         StackItem(SYMBOL, "ASSIGN"),
         StackItem(SYMBOL, "ATRIBSTAT'"),
         StackItem(ACTION, CODE::assign),

     }},
    {"ATRIBSTAT'-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL",
     {
         StackItem(SYMBOL, "EXPRESSION"),
         StackItem(ACTION, CODE::set_expression_val),
     }},
    {"ATRIBSTAT'-CALL",
     {
         StackItem(ACTION, CODE::get_return_val),
         StackItem(ACTION, CODE::inherit_next),
         StackItem(SYMBOL, "FUNCCALL"),
         StackItem(ACTION, CODE::get_code_from_childs),
     }},
    {"ATRIBSTAT'-NEW",
     {
         StackItem(SYMBOL, "ALLOCEXPRESSION"),
     }},
    {"FUNCCALL-CALL",
     {
         StackItem(SYMBOL, "CALL"),
         StackItem(SYMBOL, "OPEN_PARENTHESIS"),
         StackItem(SYMBOL, "IDENT"),
         StackItem(ACTION, DECLARE_VERIFY::action1),
         StackItem(SYMBOL, "OPEN_PARENTHESIS"),
         StackItem(SYMBOL, "PARAMLISTCALL"),
         StackItem(SYMBOL, "CLOSE_PARENTHESIS"),
         StackItem(SYMBOL, "CLOSE_PARENTHESIS"),
         StackItem(ACTION, CODE::funccall),
     }},
    {"PARAMLISTCALL-IDENT",
     {
         StackItem(SYMBOL, "IDENT"),
         StackItem(ACTION, DECLARE_VERIFY::action2),
         StackItem(SYMBOL, "PARAMLISTCALL'"),
         StackItem(ACTION, CODE::paramlistcall),
     }},
    {"PARAMLISTCALL-CLOSE_PARENTHESIS", {StackItem(SYMBOL, "ε")}},
    {"PARAMLISTCALL'-CLOSE_PARENTHESIS", {StackItem(SYMBOL, "ε")}},
    {"PARAMLISTCALL'-COMMA",
     {
         StackItem(SYMBOL, "COMMA"),
         StackItem(SYMBOL, "PARAMLISTCALL"),
         StackItem(ACTION, CODE::get_params),
         StackItem(ACTION, CODE::get_code_from_childs),
     }},
    {"PRINTSTAT-PRINT",
     {
         StackItem(SYMBOL, "PRINT"),
         StackItem(SYMBOL, "EXPRESSION"),
         StackItem(ACTION, CODE::handle_printstat),
     }},
    {"READSTAT-READ",
     {
         StackItem(SYMBOL, "READ"),
         StackItem(SYMBOL, "LVALUE"),
         StackItem(ACTION, CODE::handle_readstat),
     }},
    {"RETURNSTAT-RETURN",
     {
         StackItem(SYMBOL, "RETURN"),
         StackItem(SYMBOL, "RETURNSTAT'"),
         StackItem(ACTION, CODE::handle_return),
     }},
    {"RETURNSTAT'-IDENT",
     {
         StackItem(SYMBOL, "LVALUE"),
         StackItem(ACTION, CODE::handle_return2),
     }},
    {"RETURNSTAT'-SEMICOLON", {StackItem(SYMBOL, "ε")}},
    {"IFSTAT-IF",
     {
         StackItem(ACTION, CODE::inherit_next),
         StackItem(SYMBOL, "IF"),
         StackItem(SYMBOL, "OPEN_PARENTHESIS"),
         StackItem(SYMBOL, "EXPRESSION"),
         StackItem(SYMBOL, "CLOSE_PARENTHESIS"),
         StackItem(SYMBOL, "OPEN_BRACE"),
         StackItem(ACTION, SCOPE::action5),
         StackItem(SYMBOL, "STATEMENT"),
         StackItem(SYMBOL, "CLOSE_BRACE"),
         StackItem(ACTION, SCOPE::action2),
         StackItem(SYMBOL, "IFSTAT'"),
         StackItem(ACTION, CODE::if_action_s),

     }},
    {"IFSTAT'-IDENT-OPEN_BRACE-CLOSE_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-"
     "PRINT-READ-RETURN-IF-FOR-$",
     {
         StackItem(ACTION, CODE::empty_else_action),
         StackItem(SYMBOL, "ε"),
     }},
    {"IFSTAT'-ELSE",
     {
         StackItem(ACTION, CODE::else_action_h),
         StackItem(SYMBOL, "ELSE"),
         StackItem(SYMBOL, "OPEN_BRACE"),
         StackItem(ACTION, SCOPE::action6),
         StackItem(SYMBOL, "STATEMENT"),
         StackItem(SYMBOL, "CLOSE_BRACE"),
         StackItem(ACTION, SCOPE::action2),
         StackItem(ACTION, CODE::else_action_s),
     }},
    {"FORSTAT-FOR",
     {
         StackItem(ACTION, CODE::inherit_next),
         StackItem(ACTION, CODE::inherit_for_next),
         StackItem(SYMBOL, "FOR"),
         StackItem(SYMBOL, "OPEN_PARENTHESIS"),
         StackItem(SYMBOL, "ATRIBSTAT"),
         StackItem(SYMBOL, "SEMICOLON"),
         StackItem(SYMBOL, "EXPRESSION"),
         StackItem(SYMBOL, "SEMICOLON"),
         StackItem(SYMBOL, "ATRIBSTAT"),
         StackItem(SYMBOL, "CLOSE_PARENTHESIS"),
         StackItem(ACTION, SCOPE::action7),
         StackItem(SYMBOL, "STATEMENT"),
         StackItem(ACTION, SCOPE::action2),
         StackItem(ACTION, CODE::for_action),
     }},
    {"STATELIST-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-"
     "RETURN-IF-FOR",
     {
         StackItem(ACTION, CODE::inherit_next),
         StackItem(ACTION, CODE::set_label),
         StackItem(SYMBOL, "STATEMENT"),
         StackItem(SYMBOL, "STATELIST'"),
         StackItem(ACTION, CODE::get_code_from_childs_2),
     }},
    {"STATELIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-SEMICOLON-BREAK-PRINT-READ-"
     "RETURN-IF-FOR",
     {
         StackItem(ACTION, CODE::inherit_next),
         StackItem(SYMBOL, "STATELIST"),
         StackItem(ACTION, CODE::get_code_from_childs),
     }},
    {"STATELIST'-CLOSE_BRACE",
     {
         StackItem(ACTION, CODE::inherit_next),
         StackItem(SYMBOL, "ε"),
         StackItem(ACTION, CODE::get_code_from_childs),
     }},
    {"ALLOCAUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_BRACKET-ASSIGN-RELOP-PLUS-"
     "MINUS-MULTIPLICATION-DIVISION-MODULUS",
     {StackItem(SYMBOL, "ε")}},
    {"ALLOCAUX-OPEN_BRACKET",
     {
         StackItem(SYMBOL, "OPEN_BRACKET"),
         StackItem(SYMBOL, "NUMEXPRESSION"),
         StackItem(SYMBOL, "CLOSE_BRACKET"),
         StackItem(SYMBOL, "ALLOCAUX"),
         StackItem(ACTION, EXPA::printExpression1),
         StackItem(ACTION, EXPA::arrayCounter),
     }},
    {"ALLOCEXPRESSION-NEW",
     {
         StackItem(SYMBOL, "NEW"),
         StackItem(SYMBOL, "ALLOCEXPRESSION'"),
     }},
    {"ALLOCEXPRESSION'-INT",
     {
         StackItem(SYMBOL, "INT"),
         StackItem(SYMBOL, "OPEN_BRACKET"),
         StackItem(SYMBOL, "NUMEXPRESSION"),
         StackItem(SYMBOL, "CLOSE_BRACKET"),
         StackItem(SYMBOL, "ALLOCAUX"),
         StackItem(ACTION, EXPA::printExpression2),
     }},
    {"ALLOCEXPRESSION'-FLOAT",
     {
         StackItem(SYMBOL, "FLOAT"),
         StackItem(SYMBOL, "OPEN_BRACKET"),
         StackItem(SYMBOL, "NUMEXPRESSION"),
         StackItem(SYMBOL, "CLOSE_BRACKET"),
         StackItem(SYMBOL, "ALLOCAUX"),
         StackItem(ACTION, EXPA::printExpression2),
     }},
    {"ALLOCEXPRESSION'-STRING",
     {
         StackItem(SYMBOL, "STRING"),
         StackItem(SYMBOL, "OPEN_BRACKET"),
         StackItem(SYMBOL, "NUMEXPRESSION"),
         StackItem(SYMBOL, "CLOSE_BRACKET"),
         StackItem(SYMBOL, "ALLOCAUX"),
         StackItem(ACTION, EXPA::printExpression2),
     }},
    {"EXPRESSION-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL",
     {
         StackItem(SYMBOL, "NUMEXPRESSION"),
         StackItem(ACTION, EXPA::printExpression0_h),
         StackItem(SYMBOL, "EXPRESSION'"),
         StackItem(ACTION, EXPA::printExpression0),
     }},
    {"EXPRESSION'-CLOSE_PARENTHESIS-SEMICOLON", {StackItem(SYMBOL, "ε")}},
    {"EXPRESSION'-RELOP",
     {
         StackItem(SYMBOL, "RELOP"),
         StackItem(SYMBOL, "NUMEXPRESSION"),
         StackItem(ACTION, EXPA::printExpression1),
         StackItem(ACTION, CODE::relop_action),
     }},
    {"NUMEXPRESSIONAUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_BRACKET-RELOP",
     {StackItem(SYMBOL, "ε")}},
    {"NUMEXPRESSIONAUX-PLUS",
     {StackItem(SYMBOL, "PLUS"), StackItem(SYMBOL, "NUMEXPRESSION"),
      StackItem(ACTION, EXPA::setOperation)}},
    {"NUMEXPRESSIONAUX-MINUS",
     {StackItem(SYMBOL, "MINUS"), StackItem(SYMBOL, "NUMEXPRESSION"),
      StackItem(ACTION, EXPA::setOperation)}},
    {"NUMEXPRESSION-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL",
     {StackItem(SYMBOL, "TERM"), StackItem(SYMBOL, "NUMEXPRESSIONAUX"),
      StackItem(ACTION, EXPA::generateNode)}},
    {"TERM-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-STRC-NULL",
     {StackItem(SYMBOL, "UNARYEXPR"), StackItem(SYMBOL, "UNARYEXPRAUX"),
      StackItem(ACTION, EXPA::term)}},
    {"UNARYEXPRAUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_BRACKET-RELOP-PLUS-MINUS",
     {StackItem(SYMBOL, "ε")}},
    {"UNARYEXPRAUX-MULTIPLICATION",
     {StackItem(SYMBOL, "MULTIPLICATION"), StackItem(SYMBOL, "TERM"),
      StackItem(ACTION, EXPA::setOperation2)}},
    {"UNARYEXPRAUX-DIVISION",
     {StackItem(SYMBOL, "DIVISION"), StackItem(SYMBOL, "TERM"),
      StackItem(ACTION, EXPA::setOperation2)}},
    {"UNARYEXPRAUX-MODULUS",
     {StackItem(SYMBOL, "MODULUS"), StackItem(SYMBOL, "TERM"),
      StackItem(ACTION, EXPA::setOperation2)}},
    {"UNARYEXPR-IDENT-OPEN_PARENTHESIS-NI-NPF-STRC-NULL",
     {StackItem(SYMBOL, "FACTOR"), StackItem(ACTION, EXPA::valueUp)}},
    {"UNARYEXPR-PLUS",
     {StackItem(SYMBOL, "PLUS"), StackItem(SYMBOL, "FACTOR"),
      StackItem(ACTION, EXPA::secondChildValueUp)}},
    {"UNARYEXPR-MINUS",
     {StackItem(SYMBOL, "MINUS"), StackItem(SYMBOL, "FACTOR"),
      StackItem(ACTION, EXPA::secondChildValueUp)}},
    {"FACTOR-IDENT",
     {StackItem(SYMBOL, "LVALUE"), StackItem(ACTION, EXPA::identUp)}},
    {"FACTOR-OPEN_PARENTHESIS",
     {StackItem(SYMBOL, "OPEN_PARENTHESIS"), StackItem(SYMBOL, "NUMEXPRESSION"),
      StackItem(SYMBOL, "CLOSE_PARENTHESIS"),
      StackItem(ACTION, EXPA::passNum)}},
    {"FACTOR-NI",
     {StackItem(SYMBOL, "NI"), StackItem(ACTION, EXPA::lexemeToValue)}},
    {"FACTOR-NPF",
     {StackItem(SYMBOL, "NPF"), StackItem(ACTION, EXPA::lexemeToValue)}},
    {"FACTOR-STRC",
     {StackItem(SYMBOL, "STRC"), StackItem(ACTION, EXPA::lexemeToValue)}},
    {"FACTOR-NULL", {StackItem(SYMBOL, "NULL")}},
    {"LVALUE-IDENT",
     {StackItem(SYMBOL, "IDENT"), StackItem(ACTION, DECLARE_VERIFY::action2),
      StackItem(ACTION, CODE::get_lvalue_var),
      StackItem(ACTION, EXPA::evaluateIdent), StackItem(SYMBOL, "ALLOCAUX"),
      StackItem(ACTION, EXPA::evaluateIdent)}},
};

#ifndef COMPILADORES_SDT_TABLE_H
#define COMPILADORES_SDT_TABLE_H

#include <string>
#include <unordered_map>
#include <vector>

#include "parser_stack_item.h"
#include "semantic_actions.h"

#endif  // COMPILADORES_SDT_TABLE_H

#ifndef __errors_h
#define __errors_h

#include <exception>
#include <string>

class LexicalError : public std::exception {
   public:
    LexicalError(const std::string &message) : msg(message) {}

    virtual const char *what() const noexcept override { return msg.c_str(); }

   private:
    std::string msg;
};

class ParsingError : public std::exception {
   public:
    ParsingError(const std::string &message) : msg(message) {}

    virtual const char *what() const noexcept override { return msg.c_str(); }

   private:
    std::string msg;
};

class SemanticError : public std::exception {
   public:
    SemanticError(const std::string &message) : msg(message) {}

    virtual const char *what() const noexcept override { return msg.c_str(); }

   private:
    std::string msg;
};

#endif

//
// Created by vitorh on 05/12/24.
//

#include "expression_tree_resolver.h"

#include "type_enum.h"

int ExpressionTreeResolver::tempVal = 0;

void ExpressionTreeResolver::printBT(const std::string &prefix,
                                     std::shared_ptr<Node> node, bool isLeft) {
    if (node != nullptr) {
        std::cout << prefix;

        std::cout << (isLeft ? "├── " : "└── ");

        // print the value of the node
        if (node->value == "N") {
            std::cout << node->operation << std::endl;

        } else {
            std::cout << node->value << std::endl;
        }
        // enter the next tree level - left and right branch
        printBT(prefix + (isLeft ? "│    " : "     "), node->A, true);
        printBT(prefix + (isLeft ? "│    " : "     "), node->B, false);
    }
}

void ExpressionTreeResolver::printError(char operation, std::shared_ptr<Node> A,
                                        std::shared_ptr<Node> B) {
    std::cout << "===================================================="
              << std::endl;

    std::cout << "error" << ' ' << operation << std::endl;

    // enter the next tree level - left and right branch
    printBT("      ", A, true);
    printBT("      ", B, false);
}

void ExpressionTreeResolver::printBT(std::shared_ptr<Node> node) {
    std::cout << "===================================================="
              << std::endl;

    std::cout << node->type << std::endl;

    printBT("", node, false);
}
std::string ExpressionTreeResolver::generateTempVar() {
    std::string newtempVal = "$";
    newtempVal += std::to_string(tempVal);
    tempVal += 1;
    return newtempVal;
}

//
// Created by vitorh on 05/12/24.
//
#ifndef COMPILADORES_COMPILER_EXPRESSION_TREE_RESOLVER_H_
#define COMPILADORES_COMPILER_EXPRESSION_TREE_RESOLVER_H_

#include <iostream>
#include <memory>

#include "ast_node.h"
class ExpressionTreeResolver {
   public:
    ExpressionTreeResolver() = default;

    static void printBT(const std::string& prefix, std::shared_ptr<Node> node,
                        bool isLeft);

    static void printBT(std::shared_ptr<Node> node);
    static void printError(char operation, std::shared_ptr<Node> A,
                           std::shared_ptr<Node> B);

    static std::string generateTempVar();

   private:
    static int tempVal;
};

#endif  // COMPILADORES_COMPILER_EXPRESSION_TREE_RESOLVER_H_

#include "fsm.h"

void FiniteStateMachine::reset() {
    _current_state = _initial_state;
    _lexeme = "";
    _status = FSMStatus::IDLE;
}

FSMStatus FiniteStateMachine::transition(char c, bool is_eof) {
    if (_status == FSMStatus::ERROR || _status == FSMStatus::SUCCESS) return _status;

    if (_transitions.find(_current_state) == _transitions.end()) {
        _status = FSMStatus::ERROR;
        return _status;
    }
    auto next_state = _transitions.at(_current_state)(c, is_eof);
    _current_state = next_state;
    _status = FSMStatus::RUNNING;

    if (next_state == "dead") {
        _status = FSMStatus::ERROR;
        return _status;
    }

    if (_final_states.find(next_state) != _final_states.end()) {
        _status = FSMStatus::SUCCESS;
    }

    if (!has_to_move_cursor_back()) _lexeme += c;

    return _status;
}

bool FiniteStateMachine::has_to_move_cursor_back() {
    if (_status != FSMStatus::SUCCESS) return false;
    if (_final_states.find(_current_state) == _final_states.end()) return false;
    return _cursor_back_final_states.find(_current_state) !=
           _cursor_back_final_states.end();
}

#ifndef __fsm_h
#define __fsm_h

#include <functional>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

enum FSMStatus { IDLE, RUNNING, SUCCESS, ERROR };

class FiniteStateMachine {
   private:
    std::unordered_map<std::string, std::function<std::string(char, bool)>>
        _transitions;
    std::string _initial_state;
    std::set<std::string> _final_states;
    std::set<std::string> _cursor_back_final_states;
    std::string _current_state;
    std::string _lexeme;
    FSMStatus _status = FSMStatus::IDLE;

   public:
    FiniteStateMachine(
        std::unordered_map<std::string, std::function<std::string(char, bool)>>
            transitions,
        std::string initial_state, std::set<std::string> final_states,
        std::set<std::string> cursor_back_final_states = {})
        : _transitions(transitions),
          _initial_state(initial_state),
          _final_states(final_states),
          _cursor_back_final_states(cursor_back_final_states),
          _current_state(initial_state) {}

    FSMStatus get_status() { return _status; }

    std::string get_lexeme() { return _lexeme; }

    void reset();
    FSMStatus transition(char c, bool is_eof);
    bool has_to_move_cursor_back();
};

#endif

#ifndef __lexial_analyzer_h
#define __lexial_analyzer_h

#include "lexical_analyzer.h"

#include "errors.h"

std::shared_ptr<LexicalAnalyzer::PossibleToken>
LexicalAnalyzer::get_longest_token() {
    std::shared_ptr<PossibleToken> response;
    uint longest = 0;
    int index = 0;
    for (const auto &possible_token : possible_tokens) {
        uint token_size = possible_token->token->lexeme.size();
        if (index == 0) {
            longest = token_size;
            response = possible_token;
        } else if (token_size > longest) {
            longest = token_size;
            response = possible_token;
        }
    }
    return response;
}

std::shared_ptr<Token> LexicalAnalyzer::get_next_token() {
    if (_file.is_open()) {
        while (_file.good()) {
            _file.get(c);
            bool eof = _file.eof();
            if (eof && !reading_token) break;
            column++;
            if (c == '\n') {
                line++;
                column = 0;
                if (reading_token) line_between_current_token_reading++;
            }

            if (!eof && isWhitespace(c) && !reading_token) continue;

            int on_running = 0;
            for (auto &token_identifier : token_identifiers) {
                if (token_identifier->has_already_succeeded()) continue;
                auto result = token_identifier->analyze(
                    c, eof, static_cast<int>(_file.tellg()) - 1, line, column);
                if (result == FSMStatus::RUNNING) {
                    on_running++;
                    continue;
                }
                if (result == FSMStatus::SUCCESS) {
                    auto token_id =
                        get_token_id(token_identifier->get_id(),
                                     token_identifier->get_lexeme());
                    auto token = std::make_shared<Token>(
                        token_identifier->get_lexeme(), token_id,
                        token_identifier->start_line,
                        token_identifier->start_column);
                    possible_tokens.push_back(std::make_shared<PossibleToken>(
                        token, token_identifier->cursor_start));
                }
            }

            reading_token = on_running > 0;

            if (on_running > 0) continue;

            if (possible_tokens.size() == 0) {
                std::string error_msg = "Error starting on " +
                                        std::to_string(line) + ':' +
                                        std::to_string(column);
                throw LexicalError(error_msg);
            }

            auto choosen_token = get_longest_token();
            auto token = choosen_token->token;
            tokens.push_back(token);

            column = token->column + token->lexeme.size() - 1;
            if (line_between_current_token_reading) {
                line -= line_between_current_token_reading;
                line_between_current_token_reading = 0;
            }

            _file.clear();
            int next_cursor_position =
                choosen_token->start + token->lexeme.size();
            _file.seekg(next_cursor_position);

            possible_tokens.clear();
            for (auto &token_identifier : token_identifiers) {
                token_identifier->reset();
            }

            return token;
        }
    }

    return nullptr;
}

std::string LexicalAnalyzer::get_token_id(const std::string &id,
                                          const std::string &lexeme) {
    if (id == "IDENT" && std::find(_keywords.begin(), _keywords.end(),
                                   lexeme) != _keywords.end()) {
        std::string new_id = lexeme;
        std::transform(new_id.begin(), new_id.end(), new_id.begin(), ::toupper);
        return new_id;
    }
    if (id != "MATHOP" && id != "ONECHAR") return id;
    return _token_id_by_lexeme.at(lexeme);
}

#endif

#ifndef __lexical_analyzer_h
#define __lexical_analyzer_h

#include <exception>
#include <fstream>
#include <string>

#include "symbols_table.h"
#include "token_identifiers.h"

class LexicalAnalyzer {
   public:
    struct PossibleToken {
        std::shared_ptr<Token> token;
        int start;

        PossibleToken(std::shared_ptr<Token> token, int start)
            : token(token), start(start) {}
    };

    LexicalAnalyzer(std::ifstream &file, std::vector<std::string> &keywords)
        : _file(file), _keywords(keywords) {}

    std::shared_ptr<Token> get_next_token();

   private:
    std::ifstream &_file;
    std::shared_ptr<SymbolsTable> _symbols_table;
    std::vector<std::string> &_keywords;
    std::vector<std::shared_ptr<TokenIdentifier>> token_identifiers{
        identTokenIdentifier, integerTokenIdentifier,
        floatTokenIdentifier, mathOperatorTokenIdentifier,
        relopTokenIdentifier, oneCharTokenIdentifier,
        stringConstCharTokenIdentifier};
    std::vector<std::shared_ptr<PossibleToken>> possible_tokens;
    std::vector<std::shared_ptr<Token>> tokens;
    std::unordered_map<std::string, std::string> _token_id_by_lexeme = {
        {"+", "PLUS"},
        {"-", "MINUS"},
        {"*", "MULTIPLICATION"},
        {"/", "DIVISION"},
        {"%", "MODULUS"},
        {"=", "ASSIGN"},
        {"(", "OPEN_PARENTHESIS"},
        {")", "CLOSE_PARENTHESIS"},
        {"{", "OPEN_BRACE"},
        {"}", "CLOSE_BRACE"},
        {"[", "OPEN_BRACKET"},
        {"]", "CLOSE_BRACKET"},
        {";", "SEMICOLON"},
        {",", "COMMA"},
    };

    char c;
    int line = 1;
    int column = 0;
    int line_between_current_token_reading = 0;
    bool reading_token = false;

    std::shared_ptr<PossibleToken> get_longest_token();
    std::string get_token_id(const std::string &id, const std::string &lexeme);
};

#endif

#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#include "ast_node.h"
#include "errors.h"
#include "lexical_analyzer.h"
#include "parse_table.h"
#include "parser.h"
#include "scope_manager.h"
#include "tokens.h"

void print_ast_tree(std::shared_ptr<ASTNode> node) {
    if (node->children.size() == 0) {
        std::cout << node->id << ' ';
        return;
    }
    for (auto &child : node->children) {
        print_ast_tree(child);
    }
}

void print_code(std::shared_ptr<ASTNode> node) {
    for (auto code : *node->code) {
        std::cout << code << std::endl;
    }
}

int main(int argc, char *argv[]) {
    std::cout << "Grupo do trabalho:" << std::endl;
    std::cout << "Gabriel de Vargas Coelho, João Pedro Lopes de Camargo, Tulio "
                 "Scarabelot Bardini, Vitor Hugo Homem Marzarotto"
              << std::endl;

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>\n";
        return 1;
    }

    std::string filename = argv[1];
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Error: Could not open file '" << filename << "'\n";
        return 1;
    }

    std::vector<std::string> keywords{
        "def",    "if",  "else", "for",   "int",   "float", "string",
        "return", "new", "read", "print", "break", "call"};
    auto scope_manager = std::make_shared<ScopeManager>();
    auto lex_analyzer = LexicalAnalyzer(file, keywords);
    auto parser = Parser(scope_manager, parse_table);
    try {
        while (true) {
            auto token = lex_analyzer.get_next_token();
            if (token) {
                parser.analyze_token(token);
                continue;
            }
            parser.analyze_token(std::make_shared<Token>("$", "$", 0, 0));
            auto result = parser.finish();
            if (result) {
                std::cout << "\n***********************************************************\n" << std::endl;
                std::cout << "Compilation Success!" << std::endl;
                std::cout << "All expression trees are valid!" << std::endl;
                std::cout << "All variable declarations are valid!" << std::endl;
                std::cout << "All break declarations are valid!" << std::endl;
                std::cout << "\n***********************************************************\n" << std::endl;
                std::cout << "Generated Symbols Tables:\n" << std::endl;
                auto ast_root = parser.get_ast_root();
                scope_manager->print_tables(std::cout);
                std::cout << "\n***********************************************************\n" << std::endl;
                std::cout << "Intermediate Code:\n" << std::endl;
                print_code(ast_root);
                return 0;
            } else {
                std::cout << ":(" << std::endl;
                return 1;
            }
        }
    } catch (const LexicalError &e) {
        std::cerr << "Lexic error found.\n";
        std::cerr << e.what() << std::endl;
    } catch (const ParsingError &e) {
        std::cerr << "Parsing error found.\n";
        std::cerr << e.what() << std::endl;
    } catch (const SemanticError &e) {
        std::cerr << "Semantic error found.\n";
        std::cerr << e.what() << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "Unknown error.\n";
        std::cerr << e.what() << std::endl;
    }
}

# Variáveis
CXX = g++
CXXFLAGS = -Wall -std=c++17 -O3

# Flags para depuração
DEBUG_FLAGS = -g -O0

# Arquivos de saída
TARGET = exec

# Lista de arquivos fonte
SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)

# Regra padrão para construir o executável
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Regra para compilar os arquivos .cpp em .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Regra para compilação em modo debug
debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: $(TARGET)

# Limpa os arquivos gerados (objetos e binário)
clean:
	rm -f $(OBJS) $(TARGET)

// Arquivo gerado automaticamente pelo script em utils/LL-table-constructor.py

#include "parse_table.h"

#include "auxiliar_sdt_table.cpp"
#include "semantic_actions.h"

const std::unordered_map<
    std::string, std::unordered_map<std::string, std::vector<StackItem>>>
    parse_table = {
        {
            "PROGRAM",
            {
                {"DEF", sdt_table.at("PROGRAM-DEF")},
                {"IDENT",
                 sdt_table.at("PROGRAM-IDENT-OPEN_BRACE-INT-FLOAT-STRING-"
                              "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR")},
                {"OPEN_BRACE",
                 sdt_table.at("PROGRAM-IDENT-OPEN_BRACE-INT-FLOAT-STRING-"
                              "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR")},
                {"INT",
                 sdt_table.at("PROGRAM-IDENT-OPEN_BRACE-INT-FLOAT-STRING-"
                              "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR")},
                {"FLOAT",
                 sdt_table.at("PROGRAM-IDENT-OPEN_BRACE-INT-FLOAT-STRING-"
                              "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR")},
                {"STRING",
                 sdt_table.at("PROGRAM-IDENT-OPEN_BRACE-INT-FLOAT-STRING-"
                              "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR")},
                {"SEMICOLON",
                 sdt_table.at("PROGRAM-IDENT-OPEN_BRACE-INT-FLOAT-STRING-"
                              "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR")},
                {"BREAK",
                 sdt_table.at("PROGRAM-IDENT-OPEN_BRACE-INT-FLOAT-STRING-"
                              "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR")},
                {"PRINT",
                 sdt_table.at("PROGRAM-IDENT-OPEN_BRACE-INT-FLOAT-STRING-"
                              "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR")},
                {"READ",
                 sdt_table.at("PROGRAM-IDENT-OPEN_BRACE-INT-FLOAT-STRING-"
                              "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR")},
                {"RETURN",
                 sdt_table.at("PROGRAM-IDENT-OPEN_BRACE-INT-FLOAT-STRING-"
                              "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR")},
                {"IF",
                 sdt_table.at("PROGRAM-IDENT-OPEN_BRACE-INT-FLOAT-STRING-"
                              "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR")},
                {"FOR",
                 sdt_table.at("PROGRAM-IDENT-OPEN_BRACE-INT-FLOAT-STRING-"
                              "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR")},
                {"$", sdt_table.at("PROGRAM-$")},
            },
        },
        {
            "PROGRAM'",
            {
                {"IDENT",
                 sdt_table.at("PROGRAM'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-"
                              "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR")},
                {"OPEN_BRACE",
                 sdt_table.at("PROGRAM'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-"
                              "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR")},
                {"INT",
                 sdt_table.at("PROGRAM'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-"
                              "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR")},
                {"FLOAT",
                 sdt_table.at("PROGRAM'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-"
                              "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR")},
                {"STRING",
                 sdt_table.at("PROGRAM'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-"
                              "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR")},
                {"SEMICOLON",
                 sdt_table.at("PROGRAM'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-"
                              "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR")},
                {"BREAK",
                 sdt_table.at("PROGRAM'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-"
                              "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR")},
                {"PRINT",
                 sdt_table.at("PROGRAM'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-"
                              "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR")},
                {"READ",
                 sdt_table.at("PROGRAM'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-"
                              "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR")},
                {"RETURN",
                 sdt_table.at("PROGRAM'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-"
                              "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR")},
                {"IF",
                 sdt_table.at("PROGRAM'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-"
                              "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR")},
                {"FOR",
                 sdt_table.at("PROGRAM'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-"
                              "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR")},
                {"$", sdt_table.at("PROGRAM'-$")},
            },
        },
        {
            "FUNCLIST",
            {
                {"DEF", sdt_table.at("FUNCLIST-DEF")},
            },
        },
        {
            "FUNCLIST'",
            {
                {"DEF", sdt_table.at("FUNCLIST'-DEF")},
                {"IDENT",
                 sdt_table.at("FUNCLIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-"
                              "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$")},
                {"OPEN_BRACE",
                 sdt_table.at("FUNCLIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-"
                              "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$")},
                {"INT",
                 sdt_table.at("FUNCLIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-"
                              "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$")},
                {"FLOAT",
                 sdt_table.at("FUNCLIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-"
                              "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$")},
                {"STRING",
                 sdt_table.at("FUNCLIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-"
                              "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$")},
                {"SEMICOLON",
                 sdt_table.at("FUNCLIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-"
                              "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$")},
                {"BREAK",
                 sdt_table.at("FUNCLIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-"
                              "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$")},
                {"PRINT",
                 sdt_table.at("FUNCLIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-"
                              "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$")},
                {"READ",
                 sdt_table.at("FUNCLIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-"
                              "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$")},
                {"RETURN",
                 sdt_table.at("FUNCLIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-"
                              "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$")},
                {"IF",
                 sdt_table.at("FUNCLIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-"
                              "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$")},
                {"FOR",
                 sdt_table.at("FUNCLIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-"
                              "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$")},
                {"$",
                 sdt_table.at("FUNCLIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-"
                              "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$")},
            },
        },
        {
            "FUNCDEF",
            {
                {"DEF", sdt_table.at("FUNCDEF-DEF")},
            },
        },
        {
            "PARAMLIST",
            {
                {"CLOSE_PARENTHESIS",
                 sdt_table.at("PARAMLIST-CLOSE_PARENTHESIS")},
                {"INT", sdt_table.at("PARAMLIST-INT")},
                {"FLOAT", sdt_table.at("PARAMLIST-FLOAT")},
                {"STRING", sdt_table.at("PARAMLIST-STRING")},
            },
        },
        {
            "PARAMLIST'",
            {
                {"CLOSE_PARENTHESIS",
                 sdt_table.at("PARAMLIST'-CLOSE_PARENTHESIS")},
                {"COMMA", sdt_table.at("PARAMLIST'-COMMA")},
            },
        },
        {
            "STATEMENT",
            {
                {"IDENT", sdt_table.at("STATEMENT-IDENT")},
                {"OPEN_BRACE", sdt_table.at("STATEMENT-OPEN_BRACE")},
                {"INT", sdt_table.at("STATEMENT-INT-FLOAT-STRING")},
                {"FLOAT", sdt_table.at("STATEMENT-INT-FLOAT-STRING")},
                {"STRING", sdt_table.at("STATEMENT-INT-FLOAT-STRING")},
                {"SEMICOLON", sdt_table.at("STATEMENT-SEMICOLON")},
                {"BREAK", sdt_table.at("STATEMENT-BREAK")},
                {"PRINT", sdt_table.at("STATEMENT-PRINT")},
                {"READ", sdt_table.at("STATEMENT-READ")},
                {"RETURN", sdt_table.at("STATEMENT-RETURN")},
                {"IF", sdt_table.at("STATEMENT-IF")},
                {"FOR", sdt_table.at("STATEMENT-FOR")},
            },
        },
        {
            "INDEX",
            {
                {"SEMICOLON", sdt_table.at("INDEX-SEMICOLON")},
                {"OPEN_BRACKET", sdt_table.at("INDEX-OPEN_BRACKET")},
            },
        },
        {
            "VARDECL",
            {
                {"INT", sdt_table.at("VARDECL-INT")},
                {"FLOAT", sdt_table.at("VARDECL-FLOAT")},
                {"STRING", sdt_table.at("VARDECL-STRING")},
            },
        },
        {
            "ATRIBSTAT",
            {
                {"IDENT", sdt_table.at("ATRIBSTAT-IDENT")},
            },
        },
        {
            "ATRIBSTAT'",
            {
                {"IDENT", sdt_table.at("ATRIBSTAT'-IDENT-OPEN_PARENTHESIS-NI-"
                                       "PLUS-MINUS-NPF-STRC-NULL")},
                {"OPEN_PARENTHESIS",
                 sdt_table.at("ATRIBSTAT'-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-"
                              "NPF-STRC-NULL")},
                {"NI", sdt_table.at("ATRIBSTAT'-IDENT-OPEN_PARENTHESIS-NI-PLUS-"
                                    "MINUS-NPF-STRC-NULL")},
                {"PLUS", sdt_table.at("ATRIBSTAT'-IDENT-OPEN_PARENTHESIS-NI-"
                                      "PLUS-MINUS-NPF-STRC-NULL")},
                {"MINUS", sdt_table.at("ATRIBSTAT'-IDENT-OPEN_PARENTHESIS-NI-"
                                       "PLUS-MINUS-NPF-STRC-NULL")},
                {"NPF", sdt_table.at("ATRIBSTAT'-IDENT-OPEN_PARENTHESIS-NI-"
                                     "PLUS-MINUS-NPF-STRC-NULL")},
                {"STRC", sdt_table.at("ATRIBSTAT'-IDENT-OPEN_PARENTHESIS-NI-"
                                      "PLUS-MINUS-NPF-STRC-NULL")},
                {"NULL", sdt_table.at("ATRIBSTAT'-IDENT-OPEN_PARENTHESIS-NI-"
                                      "PLUS-MINUS-NPF-STRC-NULL")},
                {"CALL", sdt_table.at("ATRIBSTAT'-CALL")},
                {"NEW", sdt_table.at("ATRIBSTAT'-NEW")},
            },
        },
        {
            "FUNCCALL",
            {
                {"CALL", sdt_table.at("FUNCCALL-CALL")},
            },
        },
        {
            "PARAMLISTCALL",
            {
                {"IDENT", sdt_table.at("PARAMLISTCALL-IDENT")},
                {"CLOSE_PARENTHESIS",
                 sdt_table.at("PARAMLISTCALL-CLOSE_PARENTHESIS")},
            },
        },
        {
            "PARAMLISTCALL'",
            {
                {"CLOSE_PARENTHESIS",
                 sdt_table.at("PARAMLISTCALL'-CLOSE_PARENTHESIS")},
                {"COMMA", sdt_table.at("PARAMLISTCALL'-COMMA")},
            },
        },
        {
            "PRINTSTAT",
            {
                {"PRINT", sdt_table.at("PRINTSTAT-PRINT")},
            },
        },
        {
            "READSTAT",
            {
                {"READ", sdt_table.at("READSTAT-READ")},
            },
        },
        {
            "RETURNSTAT",
            {
                {"RETURN", sdt_table.at("RETURNSTAT-RETURN")},
            },
        },
        {
            "RETURNSTAT'",
            {
                {"IDENT", sdt_table.at("RETURNSTAT'-IDENT")},
                {"SEMICOLON", sdt_table.at("RETURNSTAT'-SEMICOLON")},
            },
        },
        {
            "IFSTAT",
            {
                {"IF", sdt_table.at("IFSTAT-IF")},
            },
        },
        {
            "IFSTAT'",
            {
                {"IDENT",
                 sdt_table.at(
                     "IFSTAT'-IDENT-OPEN_BRACE-CLOSE_BRACE-INT-FLOAT-STRING-"
                     "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$")},
                {"OPEN_BRACE",
                 sdt_table.at(
                     "IFSTAT'-IDENT-OPEN_BRACE-CLOSE_BRACE-INT-FLOAT-STRING-"
                     "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$")},
                {"CLOSE_BRACE",
                 sdt_table.at(
                     "IFSTAT'-IDENT-OPEN_BRACE-CLOSE_BRACE-INT-FLOAT-STRING-"
                     "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$")},
                {"INT",
                 sdt_table.at(
                     "IFSTAT'-IDENT-OPEN_BRACE-CLOSE_BRACE-INT-FLOAT-STRING-"
                     "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$")},
                {"FLOAT",
                 sdt_table.at(
                     "IFSTAT'-IDENT-OPEN_BRACE-CLOSE_BRACE-INT-FLOAT-STRING-"
                     "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$")},
                {"STRING",
                 sdt_table.at(
                     "IFSTAT'-IDENT-OPEN_BRACE-CLOSE_BRACE-INT-FLOAT-STRING-"
                     "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$")},
                {"SEMICOLON",
                 sdt_table.at(
                     "IFSTAT'-IDENT-OPEN_BRACE-CLOSE_BRACE-INT-FLOAT-STRING-"
                     "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$")},
                {"BREAK",
                 sdt_table.at(
                     "IFSTAT'-IDENT-OPEN_BRACE-CLOSE_BRACE-INT-FLOAT-STRING-"
                     "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$")},
                {"PRINT",
                 sdt_table.at(
                     "IFSTAT'-IDENT-OPEN_BRACE-CLOSE_BRACE-INT-FLOAT-STRING-"
                     "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$")},
                {"READ",
                 sdt_table.at(
                     "IFSTAT'-IDENT-OPEN_BRACE-CLOSE_BRACE-INT-FLOAT-STRING-"
                     "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$")},
                {"RETURN",
                 sdt_table.at(
                     "IFSTAT'-IDENT-OPEN_BRACE-CLOSE_BRACE-INT-FLOAT-STRING-"
                     "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$")},
                {"IF",
                 sdt_table.at(
                     "IFSTAT'-IDENT-OPEN_BRACE-CLOSE_BRACE-INT-FLOAT-STRING-"
                     "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$")},
                {"FOR",
                 sdt_table.at(
                     "IFSTAT'-IDENT-OPEN_BRACE-CLOSE_BRACE-INT-FLOAT-STRING-"
                     "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$")},
                {"$", sdt_table.at(
                          "IFSTAT'-IDENT-OPEN_BRACE-CLOSE_BRACE-INT-FLOAT-"
                          "STRING-SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR-$")},
                {"ELSE", sdt_table.at("IFSTAT'-ELSE")},
            },
        },
        {
            "FORSTAT",
            {
                {"FOR", sdt_table.at("FORSTAT-FOR")},
            },
        },
        {
            "STATELIST",
            {
                {"IDENT",
                 sdt_table.at("STATELIST-IDENT-OPEN_BRACE-INT-FLOAT-STRING-"
                              "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR")},
                {"OPEN_BRACE",
                 sdt_table.at("STATELIST-IDENT-OPEN_BRACE-INT-FLOAT-STRING-"
                              "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR")},
                {"INT",
                 sdt_table.at("STATELIST-IDENT-OPEN_BRACE-INT-FLOAT-STRING-"
                              "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR")},
                {"FLOAT",
                 sdt_table.at("STATELIST-IDENT-OPEN_BRACE-INT-FLOAT-STRING-"
                              "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR")},
                {"STRING",
                 sdt_table.at("STATELIST-IDENT-OPEN_BRACE-INT-FLOAT-STRING-"
                              "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR")},
                {"SEMICOLON",
                 sdt_table.at("STATELIST-IDENT-OPEN_BRACE-INT-FLOAT-STRING-"
                              "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR")},
                {"BREAK",
                 sdt_table.at("STATELIST-IDENT-OPEN_BRACE-INT-FLOAT-STRING-"
                              "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR")},
                {"PRINT",
                 sdt_table.at("STATELIST-IDENT-OPEN_BRACE-INT-FLOAT-STRING-"
                              "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR")},
                {"READ",
                 sdt_table.at("STATELIST-IDENT-OPEN_BRACE-INT-FLOAT-STRING-"
                              "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR")},
                {"RETURN",
                 sdt_table.at("STATELIST-IDENT-OPEN_BRACE-INT-FLOAT-STRING-"
                              "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR")},
                {"IF",
                 sdt_table.at("STATELIST-IDENT-OPEN_BRACE-INT-FLOAT-STRING-"
                              "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR")},
                {"FOR",
                 sdt_table.at("STATELIST-IDENT-OPEN_BRACE-INT-FLOAT-STRING-"
                              "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR")},
            },
        },
        {
            "STATELIST'",
            {
                {"IDENT",
                 sdt_table.at("STATELIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-"
                              "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR")},
                {"OPEN_BRACE",
                 sdt_table.at("STATELIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-"
                              "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR")},
                {"INT",
                 sdt_table.at("STATELIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-"
                              "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR")},
                {"FLOAT",
                 sdt_table.at("STATELIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-"
                              "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR")},
                {"STRING",
                 sdt_table.at("STATELIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-"
                              "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR")},
                {"SEMICOLON",
                 sdt_table.at("STATELIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-"
                              "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR")},
                {"BREAK",
                 sdt_table.at("STATELIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-"
                              "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR")},
                {"PRINT",
                 sdt_table.at("STATELIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-"
                              "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR")},
                {"READ",
                 sdt_table.at("STATELIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-"
                              "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR")},
                {"RETURN",
                 sdt_table.at("STATELIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-"
                              "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR")},
                {"IF",
                 sdt_table.at("STATELIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-"
                              "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR")},
                {"FOR",
                 sdt_table.at("STATELIST'-IDENT-OPEN_BRACE-INT-FLOAT-STRING-"
                              "SEMICOLON-BREAK-PRINT-READ-RETURN-IF-FOR")},
                {"CLOSE_BRACE", sdt_table.at("STATELIST'-CLOSE_BRACE")},
            },
        },
        {
            "ALLOCAUX",
            {
                {"CLOSE_PARENTHESIS",
                 sdt_table.at("ALLOCAUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_"
                              "BRACKET-ASSIGN-RELOP-PLUS-MINUS-MULTIPLICATION-"
                              "DIVISION-MODULUS")},
                {"SEMICOLON",
                 sdt_table.at("ALLOCAUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_"
                              "BRACKET-ASSIGN-RELOP-PLUS-MINUS-MULTIPLICATION-"
                              "DIVISION-MODULUS")},
                {"CLOSE_BRACKET",
                 sdt_table.at("ALLOCAUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_"
                              "BRACKET-ASSIGN-RELOP-PLUS-MINUS-MULTIPLICATION-"
                              "DIVISION-MODULUS")},
                {"ASSIGN", sdt_table.at("ALLOCAUX-CLOSE_PARENTHESIS-SEMICOLON-"
                                        "CLOSE_BRACKET-ASSIGN-RELOP-PLUS-MINUS-"
                                        "MULTIPLICATION-DIVISION-MODULUS")},
                {"RELOP", sdt_table.at("ALLOCAUX-CLOSE_PARENTHESIS-SEMICOLON-"
                                       "CLOSE_BRACKET-ASSIGN-RELOP-PLUS-MINUS-"
                                       "MULTIPLICATION-DIVISION-MODULUS")},
                {"PLUS", sdt_table.at("ALLOCAUX-CLOSE_PARENTHESIS-SEMICOLON-"
                                      "CLOSE_BRACKET-ASSIGN-RELOP-PLUS-MINUS-"
                                      "MULTIPLICATION-DIVISION-MODULUS")},
                {"MINUS", sdt_table.at("ALLOCAUX-CLOSE_PARENTHESIS-SEMICOLON-"
                                       "CLOSE_BRACKET-ASSIGN-RELOP-PLUS-MINUS-"
                                       "MULTIPLICATION-DIVISION-MODULUS")},
                {"MULTIPLICATION",
                 sdt_table.at("ALLOCAUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_"
                              "BRACKET-ASSIGN-RELOP-PLUS-MINUS-MULTIPLICATION-"
                              "DIVISION-MODULUS")},
                {"DIVISION",
                 sdt_table.at("ALLOCAUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_"
                              "BRACKET-ASSIGN-RELOP-PLUS-MINUS-MULTIPLICATION-"
                              "DIVISION-MODULUS")},
                {"MODULUS",
                 sdt_table.at("ALLOCAUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_"
                              "BRACKET-ASSIGN-RELOP-PLUS-MINUS-MULTIPLICATION-"
                              "DIVISION-MODULUS")},
                {"OPEN_BRACKET", sdt_table.at("ALLOCAUX-OPEN_BRACKET")},
            },
        },
        {
            "ALLOCEXPRESSION",
            {
                {"NEW", sdt_table.at("ALLOCEXPRESSION-NEW")},
            },
        },
        {
            "ALLOCEXPRESSION'",
            {
                {"INT", sdt_table.at("ALLOCEXPRESSION'-INT")},
                {"FLOAT", sdt_table.at("ALLOCEXPRESSION'-FLOAT")},
                {"STRING", sdt_table.at("ALLOCEXPRESSION'-STRING")},
            },
        },
        {
            "EXPRESSION",
            {
                {"IDENT", sdt_table.at("EXPRESSION-IDENT-OPEN_PARENTHESIS-NI-"
                                       "PLUS-MINUS-NPF-STRC-NULL")},
                {"OPEN_PARENTHESIS",
                 sdt_table.at("EXPRESSION-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-"
                              "NPF-STRC-NULL")},
                {"NI", sdt_table.at("EXPRESSION-IDENT-OPEN_PARENTHESIS-NI-PLUS-"
                                    "MINUS-NPF-STRC-NULL")},
                {"PLUS", sdt_table.at("EXPRESSION-IDENT-OPEN_PARENTHESIS-NI-"
                                      "PLUS-MINUS-NPF-STRC-NULL")},
                {"MINUS", sdt_table.at("EXPRESSION-IDENT-OPEN_PARENTHESIS-NI-"
                                       "PLUS-MINUS-NPF-STRC-NULL")},
                {"NPF", sdt_table.at("EXPRESSION-IDENT-OPEN_PARENTHESIS-NI-"
                                     "PLUS-MINUS-NPF-STRC-NULL")},
                {"STRC", sdt_table.at("EXPRESSION-IDENT-OPEN_PARENTHESIS-NI-"
                                      "PLUS-MINUS-NPF-STRC-NULL")},
                {"NULL", sdt_table.at("EXPRESSION-IDENT-OPEN_PARENTHESIS-NI-"
                                      "PLUS-MINUS-NPF-STRC-NULL")},
            },
        },
        {
            "EXPRESSION'",
            {
                {"CLOSE_PARENTHESIS",
                 sdt_table.at("EXPRESSION'-CLOSE_PARENTHESIS-SEMICOLON")},
                {"SEMICOLON",
                 sdt_table.at("EXPRESSION'-CLOSE_PARENTHESIS-SEMICOLON")},
                {"RELOP", sdt_table.at("EXPRESSION'-RELOP")},
            },
        },
        {
            "NUMEXPRESSIONAUX",
            {
                {"CLOSE_PARENTHESIS",
                 sdt_table.at("NUMEXPRESSIONAUX-CLOSE_PARENTHESIS-SEMICOLON-"
                              "CLOSE_BRACKET-RELOP")},
                {"SEMICOLON", sdt_table.at("NUMEXPRESSIONAUX-CLOSE_PARENTHESIS-"
                                           "SEMICOLON-CLOSE_BRACKET-RELOP")},
                {"CLOSE_BRACKET",
                 sdt_table.at("NUMEXPRESSIONAUX-CLOSE_PARENTHESIS-SEMICOLON-"
                              "CLOSE_BRACKET-RELOP")},
                {"RELOP", sdt_table.at("NUMEXPRESSIONAUX-CLOSE_PARENTHESIS-"
                                       "SEMICOLON-CLOSE_BRACKET-RELOP")},
                {"PLUS", sdt_table.at("NUMEXPRESSIONAUX-PLUS")},
                {"MINUS", sdt_table.at("NUMEXPRESSIONAUX-MINUS")},
            },
        },
        {
            "NUMEXPRESSION",
            {
                {"IDENT", sdt_table.at("NUMEXPRESSION-IDENT-OPEN_PARENTHESIS-"
                                       "NI-PLUS-MINUS-NPF-STRC-NULL")},
                {"OPEN_PARENTHESIS",
                 sdt_table.at("NUMEXPRESSION-IDENT-OPEN_PARENTHESIS-NI-PLUS-"
                              "MINUS-NPF-STRC-NULL")},
                {"NI", sdt_table.at("NUMEXPRESSION-IDENT-OPEN_PARENTHESIS-NI-"
                                    "PLUS-MINUS-NPF-STRC-NULL")},
                {"PLUS", sdt_table.at("NUMEXPRESSION-IDENT-OPEN_PARENTHESIS-NI-"
                                      "PLUS-MINUS-NPF-STRC-NULL")},
                {"MINUS", sdt_table.at("NUMEXPRESSION-IDENT-OPEN_PARENTHESIS-"
                                       "NI-PLUS-MINUS-NPF-STRC-NULL")},
                {"NPF", sdt_table.at("NUMEXPRESSION-IDENT-OPEN_PARENTHESIS-NI-"
                                     "PLUS-MINUS-NPF-STRC-NULL")},
                {"STRC", sdt_table.at("NUMEXPRESSION-IDENT-OPEN_PARENTHESIS-NI-"
                                      "PLUS-MINUS-NPF-STRC-NULL")},
                {"NULL", sdt_table.at("NUMEXPRESSION-IDENT-OPEN_PARENTHESIS-NI-"
                                      "PLUS-MINUS-NPF-STRC-NULL")},
            },
        },
        {
            "TERM",
            {
                {"IDENT", sdt_table.at("TERM-IDENT-OPEN_PARENTHESIS-NI-PLUS-"
                                       "MINUS-NPF-STRC-NULL")},
                {"OPEN_PARENTHESIS",
                 sdt_table.at("TERM-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-NPF-"
                              "STRC-NULL")},
                {"NI", sdt_table.at("TERM-IDENT-OPEN_PARENTHESIS-NI-PLUS-MINUS-"
                                    "NPF-STRC-NULL")},
                {"PLUS", sdt_table.at("TERM-IDENT-OPEN_PARENTHESIS-NI-PLUS-"
                                      "MINUS-NPF-STRC-NULL")},
                {"MINUS", sdt_table.at("TERM-IDENT-OPEN_PARENTHESIS-NI-PLUS-"
                                       "MINUS-NPF-STRC-NULL")},
                {"NPF", sdt_table.at("TERM-IDENT-OPEN_PARENTHESIS-NI-PLUS-"
                                     "MINUS-NPF-STRC-NULL")},
                {"STRC", sdt_table.at("TERM-IDENT-OPEN_PARENTHESIS-NI-PLUS-"
                                      "MINUS-NPF-STRC-NULL")},
                {"NULL", sdt_table.at("TERM-IDENT-OPEN_PARENTHESIS-NI-PLUS-"
                                      "MINUS-NPF-STRC-NULL")},
            },
        },
        {
            "UNARYEXPRAUX",
            {
                {"CLOSE_PARENTHESIS",
                 sdt_table.at("UNARYEXPRAUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_"
                              "BRACKET-RELOP-PLUS-MINUS")},
                {"SEMICOLON",
                 sdt_table.at("UNARYEXPRAUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_"
                              "BRACKET-RELOP-PLUS-MINUS")},
                {"CLOSE_BRACKET",
                 sdt_table.at("UNARYEXPRAUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_"
                              "BRACKET-RELOP-PLUS-MINUS")},
                {"RELOP",
                 sdt_table.at("UNARYEXPRAUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_"
                              "BRACKET-RELOP-PLUS-MINUS")},
                {"PLUS",
                 sdt_table.at("UNARYEXPRAUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_"
                              "BRACKET-RELOP-PLUS-MINUS")},
                {"MINUS",
                 sdt_table.at("UNARYEXPRAUX-CLOSE_PARENTHESIS-SEMICOLON-CLOSE_"
                              "BRACKET-RELOP-PLUS-MINUS")},
                {"MULTIPLICATION", sdt_table.at("UNARYEXPRAUX-MULTIPLICATION")},
                {"DIVISION", sdt_table.at("UNARYEXPRAUX-DIVISION")},
                {"MODULUS", sdt_table.at("UNARYEXPRAUX-MODULUS")},
            },
        },
        {
            "UNARYEXPR",
            {
                {"IDENT",
                 sdt_table.at(
                     "UNARYEXPR-IDENT-OPEN_PARENTHESIS-NI-NPF-STRC-NULL")},
                {"OPEN_PARENTHESIS",
                 sdt_table.at(
                     "UNARYEXPR-IDENT-OPEN_PARENTHESIS-NI-NPF-STRC-NULL")},
                {"NI",
                 sdt_table.at(
                     "UNARYEXPR-IDENT-OPEN_PARENTHESIS-NI-NPF-STRC-NULL")},
                {"NPF",
                 sdt_table.at(
                     "UNARYEXPR-IDENT-OPEN_PARENTHESIS-NI-NPF-STRC-NULL")},
                {"STRC",
                 sdt_table.at(
                     "UNARYEXPR-IDENT-OPEN_PARENTHESIS-NI-NPF-STRC-NULL")},
                {"NULL",
                 sdt_table.at(
                     "UNARYEXPR-IDENT-OPEN_PARENTHESIS-NI-NPF-STRC-NULL")},
                {"PLUS", sdt_table.at("UNARYEXPR-PLUS")},
                {"MINUS", sdt_table.at("UNARYEXPR-MINUS")},
            },
        },
        {
            "FACTOR",
            {
                {"IDENT", sdt_table.at("FACTOR-IDENT")},
                {"OPEN_PARENTHESIS", sdt_table.at("FACTOR-OPEN_PARENTHESIS")},
                {"NI", sdt_table.at("FACTOR-NI")},
                {"NPF", sdt_table.at("FACTOR-NPF")},
                {"STRC", sdt_table.at("FACTOR-STRC")},
                {"NULL", sdt_table.at("FACTOR-NULL")},
            },
        },
        {
            "LVALUE",
            {
                {"IDENT", sdt_table.at("LVALUE-IDENT")},
            },
        },
};

#ifndef __parse_table_h
#define __parse_table_h

#include <string>
#include <unordered_map>
#include <vector>

#include "parser_stack_item.h"

extern const std::unordered_map<
    std::string, std::unordered_map<std::string, std::vector<StackItem>>>
    parse_table;

#endif

#include "parser.h"

#include <iostream>
#include <memory>
#include <vector>

#include "ast_node.h"
#include "errors.h"

Parser::Parser(
    std::shared_ptr<ScopeManager> scope_manager,
    const std::unordered_map<
        std::string, std::unordered_map<std::string, std::vector<StackItem>>>&
        parse_table)
    : _scope_manager(scope_manager), _parse_table(parse_table) {
    init_stack();
}

void Parser::init_stack() {
    _stack.push(StackItem(StackItem::SYMBOL, "$"));
    auto first_symbol = StackItem(StackItem::SYMBOL, _initial_symbol);
    first_symbol.set_ast_node_ancestry(_ast_root, nullptr);
    _stack.push(first_symbol);
}

void Parser::analyze_token(const std::shared_ptr<Token> token_ptr) {
    auto token = *token_ptr;
    while (true) {
        if (_stack.size() == 0) {
            std::string msg_error = "Unexpected token at " +
                                    std::to_string(token.line) + ':' +
                                    std::to_string(token.column);
            throw ParsingError(msg_error);
        }
        auto top = _stack.top();
        if (top.type == StackItem::ACTION) {
            // Chama a ação semântica passando um contexto de execução
            top.action(top.ancestor, _scope_manager);
            _stack.pop();
            continue;
        }
        if (top.symbol == token.id) {
            if (token.lexeme != "$") top.node->set_token(token_ptr);
            _stack.pop();
            return;
        }
        try {
            std::vector<StackItem>& to_stack =
                _parse_table.at(top.symbol).at(token.id);
            _stack.pop();
            // Monta AST
            for (auto it = to_stack.begin(); it != to_stack.end(); it++) {
                auto& item = *it;
                if (item.type == StackItem::ACTION) {
                    // Ações precisam de um ancestral para contextualização
                    // Ações em si não entram para a AST
                    item.set_ast_node_ancestry(nullptr, top.node);
                    continue;
                }
                if (item.type == StackItem::SYMBOL && item.symbol == "ε")
                    continue;
                auto ast_node = std::make_shared<ASTNode>(item.symbol);
                item.set_ast_node_ancestry(ast_node, top.node);
                top.node->add_child(ast_node);
            }
            for (auto it = to_stack.rbegin(); it != to_stack.rend(); it++) {
                const auto& item = *it;
                if (item.type == StackItem::SYMBOL && item.symbol == "ε")
                    continue;
                _stack.push(item);
            }
        } catch (const std::out_of_range& e) {
            std::string msg_error = "Wrong token " + token.lexeme + " at " +
                                    std::to_string(token.line) + ':' +
                                    std::to_string(token.column);
            throw ParsingError(msg_error);
        }
    }
}

bool Parser::finish() { return _stack.size() == 0; }

#ifndef __parser_h
#define __parser_h

#include <functional>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

#include "ast_node.h"
#include "parse_table.h"
#include "parser_stack_item.h"
#include "scope_manager.h"
#include "tokens.h"

class Parser {
   public:
    Parser(std::shared_ptr<ScopeManager> scope_manager,
           const std::unordered_map<std::string, std::unordered_map<std::string, std::vector<StackItem>>>
               &parse_table);
    void analyze_token(const std::shared_ptr<Token> token);
    bool finish();
    std::shared_ptr<ASTNode> get_ast_root() const { return _ast_root; }

   private:
    void init_stack();
    std::string _initial_symbol = "PROGRAM";
    std::shared_ptr<ASTNode> _ast_root = std::make_shared<ASTNode>(_initial_symbol);
    std::shared_ptr<ScopeManager> _scope_manager;
    std::stack<StackItem> _stack;
    std::unordered_map<std::string,
                       std::unordered_map<std::string, std::vector<StackItem>>>
        _parse_table;
};

#endif

#ifndef __parser_stack_item_h
#define __parser_stack_item_h

#include <functional>
#include <string>

#include "ast_node.h"
#include "tokens.h"
#include "scope_manager.h"

struct StackItem {
    enum StackItemType { SYMBOL, ACTION };
    StackItemType type;
    std::string symbol;
    std::function<void(std::shared_ptr<ASTNode> ast_father_node, std::shared_ptr<ScopeManager> scope_manager)> action;
    std::shared_ptr<ASTNode> node;
    std::shared_ptr<ASTNode> ancestor;

    StackItem(StackItemType type, const std::string &symbol)
        : type(type), symbol(symbol) {}
    StackItem(StackItemType type, std::function<void(std::shared_ptr<ASTNode> ast_father_node, std::shared_ptr<ScopeManager> scope_manager)> action)
        : type(type), action(action) {}
    
    void set_ast_node_ancestry(std::shared_ptr<ASTNode> node, std::shared_ptr<ASTNode> ancestor) {
        this->node = node;
        this->ancestor = ancestor;
    }
};

#endif

#include "scope_manager.h"

#include <stack>

#include "errors.h"

void ScopeManager::open_scope(const std::string& scope_name) {
    auto scope_item = std::make_shared<ScopeItem>(
        scope_name, std::make_shared<SymbolsTable>(), _current_scope);

    if (!_scope_root) _scope_root = scope_item;

    if (_current_scope) _current_scope->children_scopes.push_back(scope_item);

    _current_scope = scope_item;
}

void ScopeManager::close_scope() {
    _current_scope = _current_scope->ancestor_scope;
}

void ScopeManager::add_symbol(std::shared_ptr<Token> token) {
    if (_current_scope->symbols_table->is_in_table(token->lexeme)) {
        std::string msg_error = token->lexeme + " redeclared at " +
                                std::to_string(token->line) + ':' +
                                std::to_string(token->column);
        throw SemanticError(msg_error);
    }
    _current_scope->symbols_table->add_symbol(token);
}

void ScopeManager::add_type(std::shared_ptr<Token> token,
                            const std::string& type) {
    auto scope = _current_scope;
    while (true) {
        if (!scope->symbols_table->is_in_table(token->lexeme)) {
            scope = scope->ancestor_scope;
            if (!scope) {
                std::string msg_error = "Impossible to assign a type to " +
                                        token->lexeme + " at " +
                                        std::to_string(token->line) + ':' +
                                        std::to_string(token->column);
                throw SemanticError(msg_error);
            }
            continue;
        }
        scope->symbols_table->add_type(token, type);
        return;
    }
}

bool ScopeManager::is_declared(std::shared_ptr<Token> token) {
    auto scope = _current_scope;
    while (true) {
        if (!scope->symbols_table->is_in_table(token->lexeme)) {
            scope = scope->ancestor_scope;
            if (!scope) return false;
            continue;
        }
        scope->symbols_table->add_symbol(token);
        return true;
    }
}

bool ScopeManager::is_type(std::shared_ptr<Token> token,
                           const std::string& type) {
    auto scope = _current_scope;
    while (true) {
        if (!scope->symbols_table->is_in_table(token->lexeme)) {
            scope = scope->ancestor_scope;
            if (!scope) return false;
            continue;
        }
        return scope->symbols_table->is_type(token, type);
    }
}

bool ScopeManager::is_in_scope(const std::string& scope_type) {
    auto scope = _current_scope;
    while (true) {
        if (scope->name == scope_type) return true;
        scope = scope->ancestor_scope;
        if (!scope) return false;
    }
}

void ScopeManager::print_tables(std::ostream& out) {
    std::stack<std::shared_ptr<ScopeItem>> scopes_stack;
    scopes_stack.push(_scope_root);
    std::shared_ptr<ScopeItem> current_scope;
    while (!scopes_stack.empty()) {
        current_scope = scopes_stack.top();
        scopes_stack.pop();
        out << "Symbol Table";
        if (current_scope->name.size()) out << " type: " << current_scope->name;
        out << std::endl;
        current_scope->symbols_table->print(out);
        for (auto child : current_scope->children_scopes) {
            scopes_stack.push(child);
        }
    }
}
std::string ScopeManager::get_type(std::shared_ptr<Token> token) {
  auto scope = _current_scope;
  while (true) {
    if (!scope->symbols_table->is_in_table(token->lexeme)) {
      scope = scope->ancestor_scope;
      if (!scope) return "";
      continue;
    }
    return scope->symbols_table->get_type(token);
  }
}

#ifndef __scope_manager_h
#define __scope_manager_h

#include <memory>
#include <vector>

#include "symbols_table.h"

struct ScopeItem {
    ScopeItem(const std::string& name,
              std::shared_ptr<SymbolsTable> symbols_table,
              std::shared_ptr<ScopeItem> ancestor)
        : name(name), symbols_table(symbols_table), ancestor_scope(ancestor) {}
    std::string name;
    std::shared_ptr<SymbolsTable> symbols_table;
    std::shared_ptr<ScopeItem> ancestor_scope;
    std::vector<std::shared_ptr<ScopeItem>> children_scopes;
};

class ScopeManager {
   public:
    ScopeManager() : _scope_root(nullptr), _current_scope(nullptr) {};
    void open_scope(const std::string& scope_name);
    void close_scope();
    void add_symbol(std::shared_ptr<Token> token);
    void add_type(std::shared_ptr<Token> token, const std::string& type);
    bool is_declared(std::shared_ptr<Token> token);
    bool is_type(std::shared_ptr<Token> token, const std::string& type);
    std::string get_type(std::shared_ptr<Token> token);
    bool is_in_scope(const std::string& scope_type);
    void print_tables(std::ostream& out);

   private:
    std::shared_ptr<ScopeItem> _scope_root;
    std::shared_ptr<ScopeItem> _current_scope;
};

#endif

#include "semantic_actions.h"

#include <iostream>
#include <string>

#include "errors.h"
#include "expression_tree_resolver.h"
#include "type_enum.h"

namespace CODE {

void relop_action(std::shared_ptr<ASTNode> ast_father_node,
                  std::shared_ptr<ScopeManager> scope_manager) {
  // EXPRESSION' ->  relop NUMEXPRESSION
  // EXPRESSION'.code = NUMEXPRESSION.code ||
  // EXPRESSION'.var = EXPRESSION'.var relop NUMEXPRESSION.var
  // NUMEXPRESSION.var [0] EXPRESSION' ->   relop NUMEXPRESSION [0]
  auto code = ast_father_node->code;
  auto numexpression = ast_father_node->children[1];
  auto relop = ast_father_node->children[0];

  code->splice(code->end(), *numexpression->code);
  code->push_back(ast_father_node->code_res_var.var + " = " +
      ast_father_node->code_res_var.var + " " +
      relop->token->lexeme + " " +
      numexpression->code_res_var.var);
}

void handle_break(std::shared_ptr<ASTNode> ast_father_node,
                  std::shared_ptr<ScopeManager> scope_manager) {
  // STATEMENT  ->  break ;
  // STATEMENT.code = goto STATEMENT.for_nxt [0]
  auto code = ast_father_node->code;
  code->push_back("goto " + ast_father_node->for_nxt);
}
void handle_readstat(std::shared_ptr<ASTNode> ast_father_node,
                     std::shared_ptr<ScopeManager> scope_manager) {
  // READSTAT  ->  read LVALUE
  // READSTAT.code = read LVALUE.var [1]
  auto lvalue = ast_father_node->children[1];
  auto code = ast_father_node->code;

  code->push_back("read " + lvalue->code_res_var.var);
}

void handle_printstat(std::shared_ptr<ASTNode> ast_father_node,
                      std::shared_ptr<ScopeManager> scope_manager) {
  // PRINTSTAT  ->  print EXPRESSION
  // PRINTSTAT.code = EXPRESSION.code || print EXPRESSION.var [0]
  // PRINTSTAT  ->  print EXPRESSION [0]

  auto expression = ast_father_node->children[1];
  auto code = ast_father_node->code;

  code->splice(code->end(), *expression->code);
  code->push_back("print " + expression->code_res_var.var);
}

void get_lvalue_var(std::shared_ptr<ASTNode> ast_father_node,
                    std::shared_ptr<ScopeManager> scope_manager) {
  // LVALUE  ->  ident ALLOCAUX
  // LVALUE.var = ident.lexema [0]
  // LVALUE  ->  ident ALLOCAUX [0]
  auto ident = ast_father_node->children[0];
  ast_father_node->code_res_var.var = ident->token->lexeme;
}

void get_return_val(std::shared_ptr<ASTNode> ast_father_node,
                    std::shared_ptr<ScopeManager> scope_manager) {
  // ATRIBSTAT' ->  FUNCCALL
  // ATRIBSTAT.var = $rv
  ast_father_node->code_res_var.var = "$rv";
}

void handle_return(std::shared_ptr<ASTNode> ast_father_node,
                   std::shared_ptr<ScopeManager> scope_manager) {
  // RETURNSTAT  ->  return RETURNSTAT'
  // RETURNSTAT.code = RETURNSTAT'.code || goto RETURNSTAT.nxt

  auto returnstat = ast_father_node->children[1];
  auto code = ast_father_node->code;
  code->splice(code->end(), *returnstat->code);
  code->push_back("goto " + ast_father_node->nxt);
}
void handle_return2(std::shared_ptr<ASTNode> ast_father_node,
                    std::shared_ptr<ScopeManager> scope_manager) {
  // RETURNSTAT' ->  LVALUE
  // RETURNSTAT'.code = "return_val = LVALUE.var";
  auto lvalue = ast_father_node->children[0];
  auto code = ast_father_node->code;
  code->push_back("$rv = " + lvalue->code_res_var.var);
}

void get_params(std::shared_ptr<ASTNode> ast_father_node,
                std::shared_ptr<ScopeManager> scope_manager) {
  // PARAMLISTCALL' -> , PARAMLISTCALL
  // PARAMLISTCALL'.cnt = PARAMLISTCALL.cnt [0]
  // PARAMLISTCALL' -> , PARAMLISTCALL [0]
  auto paramlist = ast_father_node->children[1];
  ast_father_node->code_res_var.param_cnt = paramlist->code_res_var.param_cnt;
}
void paramlistcall(std::shared_ptr<ASTNode> ast_father_node,
                   std::shared_ptr<ScopeManager> scope_manager) {
  // PARAMLISTCALL  -> ident PARAMLISTCALL'
  // PARAMLISTCALL.cnt = 1 + PARAMLISTCALL'.cnt [1]
  // PARAMLISTCALL.code = param ident || PARAMLISTCALL'.code [0]

  // PARAMLISTCALL  -> ident PARAMLISTCALL' [1][0]

  auto ident = ast_father_node->children[0];
  auto paramlist = ast_father_node->children[1];
  auto code = ast_father_node->code;

  ast_father_node->code_res_var.param_cnt =
      1 + paramlist->code_res_var.param_cnt;

  code->push_back("param " + ident->token->lexeme);
  code->splice(code->end(), *paramlist->code);
}
void funccall(std::shared_ptr<ASTNode> ast_father_node,
              std::shared_ptr<ScopeManager> scope_manager) {
  // FUNCCALL  ->  call ( ident ( PARAMLISTCALL ) )
  // FUNCCALL.return = newLabel [0]
  // FUNCCALL.code = PARAMLISTCALL.code || return = FUNCCALL.return ||
  //                 call ident, PARAMLISTCALL.cnt || FUNCCALL.return: [1]

  // FUNCCALL  ->  call ( ident ( PARAMLISTCALL ) )[0] [1]
  auto ident = ast_father_node->children[2];
  auto paramlistcall = ast_father_node->children[4];

  ast_father_node->code_res_var.ret = ast_father_node->new_label();

  auto code = ast_father_node->code;
  code->splice(code->end(), *paramlistcall->code);
  code->push_back("$r = " + ast_father_node->code_res_var.ret);
  code->push_back("call " + ident->token->lexeme + " " +
      std::to_string(paramlistcall->code_res_var.param_cnt));
  code->push_back(ast_father_node->code_res_var.ret + ":");
}
void goto_main(std::shared_ptr<ASTNode> ast_father_node,
               std::shared_ptr<ScopeManager> scope_manager) {
  // PROGRAM  ->  FUNCLIST PROGRAM'
  // PROGRAM.code -> goto MAIN || FUNCLIST.code || MAIN: ||PROGRAM'.code [1]

  // PROGRAM  ->  FUNCLIST PROGRAM' [1]

  auto code = ast_father_node->code;
  auto funclist = ast_father_node->children[0];
  auto program = ast_father_node->children[1];

  code->push_back("goto MAIN");
  code->splice(code->end(), *funclist->code);
  code->push_back("MAIN:");
  code->splice(code->end(), *program->code);
}

void funcdef_h(std::shared_ptr<ASTNode> ast_father_node,
               std::shared_ptr<ScopeManager> scope_manager) {
  // FUNCDEF  ->  def ident ( PARAMLIST ) { STATELIST }
  // begin = newlabel
  // STATELIST.nxt = 'return'
  // FUNCDEF.cod = begin: || STATELIST.code || goto return

  auto ident = ast_father_node->children[1];
  ast_father_node->code_res_var.begin = ident->token->lexeme;
  auto statelist = ast_father_node->children[6];
  statelist->nxt = "$r";
}
void funcdef_s(std::shared_ptr<ASTNode> ast_father_node,
               std::shared_ptr<ScopeManager> scope_manager) {
  // FUNCDEF  ->  def ident ( PARAMLIST ) { STATELIST }
  // begin = newlabel
  // STATELIST.nxt = 'return'
  // FUNCDEF.cod = begin: || STATELIST.code || goto STATELIST.nxt
  auto code = ast_father_node->code;
  auto statelist = ast_father_node->children[6];

  code->push_back(ast_father_node->code_res_var.begin + ":");
  code->splice(code->end(), *statelist->code);
  code->push_back("goto " + statelist->nxt);
}

void empty_else_action(std::shared_ptr<ASTNode> ast_father_node,
                       std::shared_ptr<ScopeManager> scope_manager) {
  // IFSTAT' -> ''
  // IFSTAT'.begin = IFSTAT'.nxt
  ast_father_node->code_res_var.begin = ast_father_node->nxt;
}
void else_action_h(std::shared_ptr<ASTNode> ast_father_node,
                   std::shared_ptr<ScopeManager> scope_manager) {
  // IFSTAT' -> else { STATEMENT }
  // begin = newlabel()
  // STATEMENT.nxt = IFSTAT'.nxt
  // IFSTAT'.code = begin: || statement.code || goto STATEMENT.nxt
  auto statement = ast_father_node->children[2];
  statement->nxt = ast_father_node->nxt;
  ast_father_node->code_res_var.begin = ast_father_node->new_label();
}

void else_action_s(std::shared_ptr<ASTNode> ast_father_node,
                   std::shared_ptr<ScopeManager> scope_manager) {
  // IFSTAT' -> else { STATEMENT }
  // begin = newlabel()
  // STATEMENT.nxt = IFSTAT'.nxt
  // IFSTAT'.code = begin: || statement.code || goto STATEMENT.nxt
  auto statement = ast_father_node->children[2];

  auto code = ast_father_node->code;
  code->push_back(ast_father_node->code_res_var.begin + ":");
  code->splice(code->end(), *statement->code);
  code->push_back("goto " + statement->nxt);
}
void if_action_s(std::shared_ptr<ASTNode> ast_father_node,
                 std::shared_ptr<ScopeManager> scope_manager) {
  // IFSTAT  -> if ( EXPRESSION ) { STATEMENT } IFSTAT'
  // ALL_PRODS.nxt = IFSTAT.next
  // IFSTAT.code = if not EXPRESS.val goto IFSTAT'.begin ||
  //               STATEMENT.code || goto STATEMENT.nxt
  //               IFSTAT'.code
  auto expression = ast_father_node->children[2];
  auto statement = ast_father_node->children[5];
  auto ifstat = ast_father_node->children[7];

  auto code = ast_father_node->code;
  code->splice(code->end(), *expression->code);
  code->push_back("if False " + expression->code_res_var.var + " goto " +
      ifstat->code_res_var.begin);
  code->splice(code->end(), *statement->code);
  code->push_back("goto " + statement->nxt);
  code->splice(code->end(), *ifstat->code);
}

void for_action(std::shared_ptr<ASTNode> ast_father_node,
                std::shared_ptr<ScopeManager> scope_manager) {
  // FORSTAT ->  for ( ATRIBSTAT1 ; EXPRESSION ; ATRIBSTAT2 ) STATEMENT
  // ALL_PRODS.nxt = FORSTAT.next
  // begin = newlabel()
  // FORSTAT.code = ATRIBSTAT1 || begin: || EXPRESSION
  //                || if not EXPRESSION.val goto FORSTAT.next ||
  //                STATEMENT.code || ATRIBSTAT2
  //                || goto begin

  auto atribstat1 = ast_father_node->children[2];
  auto expression = ast_father_node->children[4];
  auto atribstat2 = ast_father_node->children[6];
  auto statment = ast_father_node->children[8];

  auto code = ast_father_node->code;
  auto label = ast_father_node->new_label();

  code->splice(code->end(), *atribstat1->code);
  code->push_back(label + ':');
  code->splice(code->end(), *expression->code);
  code->push_back("if False " + expression->code_res_var.var + " goto " +
      ast_father_node->nxt);
  code->splice(code->end(), *statment->code);
  code->splice(code->end(), *atribstat2->code);
  code->push_back("goto " + label);
}
void inherit_for_next(std::shared_ptr<ASTNode> ast_father_node,
                      std::shared_ptr<ScopeManager> scope_manager) {
  // FORSTAT ->  for ( ATRIBSTAT1 ; EXPRESSION ; ATRIBSTAT2 ) STATEMENT
  auto statement = ast_father_node->children[8];
  statement->for_nxt = ast_father_node->nxt;
}

void assign(std::shared_ptr<ASTNode> ast_father_node,
            std::shared_ptr<ScopeManager> scope_manager) {
  // ATRIBSTAT  ->  LVALUE = ATRIBSTAT'
  // ATRIBSTAT.code = ATRSBSTAT'.code || LVALUE.lexema = ATRIBSTAT'.var
  auto lvalue = ast_father_node->children[0];
  auto atribstat = ast_father_node->children[2];
  std::string assign_code;
  if (lvalue->mathSDT.array_counter > 0) {
    assign_code = lvalue->mathSDT.node->value + " = " +
        atribstat->code_res_var.var;
  } else {
    assign_code = lvalue->children[0]->token->lexeme + " = " +
        atribstat->code_res_var.var;
  }


  auto code = ast_father_node->code;
  code->splice(code->end(), *lvalue->code);
  code->splice(code->end(), *atribstat->code);
  code->push_back(assign_code);
}

void set_expression_val(std::shared_ptr<ASTNode> ast_father_node,
                        std::shared_ptr<ScopeManager> scope_manager) {
  // ATRIBSTAT' -> EXPRESSION
  auto &atribstat1 = ast_father_node;
  auto &expression = ast_father_node->children[0];
  atribstat1->code_res_var.var = expression->code_res_var.var;
  atribstat1->code = expression->code;
}

void set_label(std::shared_ptr<ASTNode> ast_father_node,
               std::shared_ptr<ScopeManager> scope_manager) {
  // STATELIST  -> STATEMENT STATELIST'

  auto statement = ast_father_node->children[0];
  auto label = ast_father_node->new_label();

  statement->nxt = label;
}

void get_code_from_childs(std::shared_ptr<ASTNode> ast_father_node,
                          std::shared_ptr<ScopeManager> scope_manager) {
  // get code from last child
  auto father_code = ast_father_node->code;
  for (auto child : ast_father_node->children) {
    father_code->splice(father_code->end(), *child->code);
  }
}
void get_code_from_childs_2(std::shared_ptr<ASTNode> ast_father_node,
                            std::shared_ptr<ScopeManager> scope_manager) {
  // STATELIST  -> STATEMENT STATELIST'
  auto statement = ast_father_node->children[0];
  auto statelist = ast_father_node->children[1];

  auto code = ast_father_node->code;
  code->splice(code->end(), *statement->code);
  code->push_back(statement->nxt + ':');
  code->splice(code->end(), *statelist->code);
}

void inherit_next(std::shared_ptr<ASTNode> ast_father_node,
                  std::shared_ptr<ScopeManager> scope_manager) {
  for (auto child : ast_father_node->children) {
    child->nxt = ast_father_node->nxt;
    child->for_nxt = ast_father_node->for_nxt;
  }
}
void set_final_label(std::shared_ptr<ASTNode> ast_father_node,
                     std::shared_ptr<ScopeManager> scope_manager) {
  ast_father_node->nxt = "END";
}
void gen_final_code(std::shared_ptr<ASTNode> ast_father_node,
                    std::shared_ptr<ScopeManager> scope_manager) {
  auto father_code = ast_father_node->code;
  father_code->push_back(ast_father_node->nxt + ":");
}
void print_code(std::shared_ptr<ASTNode> ast_father_node,
                std::shared_ptr<ScopeManager> scope_manager) {
  for (auto code : *ast_father_node->code) {
    std::cout << code << std::endl;
  }
}

}  // namespace CODE

namespace SCOPE {
void action1(std::shared_ptr<ASTNode> ast_father_node,
             std::shared_ptr<ScopeManager> scope_manager) {
  // PROGRAM  ->  STATEMENT
  // PROGRAM  ->  FUNCLIST PROGRAM'
  // PROGRAM  ->  ''
  // STATEMENT  ->  { STATELIST }
  scope_manager->open_scope("");
}

void action2(std::shared_ptr<ASTNode> ast_father_node,
             std::shared_ptr<ScopeManager> scope_manager) {
  // FUNCDEF  ->  def ident ( PARAMLIST ) { STATELIST }
  // STATEMENT  ->  { STATELIST }
  // IFSTAT  -> if ( EXPRESSION ) { STATEMENT } IFSTAT'
  // IFSTAT' -> else { STATEMENT }
  // FORSTAT  ->  for ( ATRIBSTAT ; EXPRESSION ; ATRIBSTAT ) STATEMENT
  scope_manager->close_scope();
}

void action3(std::shared_ptr<ASTNode> ast_father_node,
             std::shared_ptr<ScopeManager> scope_manager) {
  // FUNCDEF  ->  def ident ( PARAMLIST ) { STATELIST }
  // PARAMLIST  ->  int ident PARAMLIST'
  // PARAMLIST  ->  float ident PARAMLIST'
  // PARAMLIST  ->  string ident PARAMLIST'
  // VARDECL  ->  int ident INDEX
  // VARDECL  ->  float ident INDEX
  // VARDECL  ->  string ident INDEX
  // add_symbol(ident)
  auto ident = ast_father_node->children[1];
  scope_manager->add_symbol(ident->token);
}

void action4(std::shared_ptr<ASTNode> ast_father_node,
             std::shared_ptr<ScopeManager> scope_manager) {
  // FUNCDEF  ->  def ident ( PARAMLIST ) { STATELIST }
  scope_manager->open_scope("function");
}

void action5(std::shared_ptr<ASTNode> ast_father_node,
             std::shared_ptr<ScopeManager> scope_manager) {
  // IFSTAT  -> if ( EXPRESSION ) { STATEMENT } IFSTAT'
  scope_manager->open_scope("if");
}

void action6(std::shared_ptr<ASTNode> ast_father_node,
             std::shared_ptr<ScopeManager> scope_manager) {
  // IFSTAT' -> else { STATEMENT }
  scope_manager->open_scope("else");
}

void action7(std::shared_ptr<ASTNode> ast_father_node,
             std::shared_ptr<ScopeManager> scope_manager) {
  // FORSTAT  ->  for ( ATRIBSTAT ; EXPRESSION ; ATRIBSTAT ) STATEMENT
  scope_manager->open_scope("for");
}
}  // namespace SCOPE

namespace DEC {
void action1(std::shared_ptr<ASTNode> ast_father_node,
             std::shared_ptr<ScopeManager> scope_manager) {
  // FUNCDEF  ->  def ident ( PARAMLIST ) { STATELIST }
  // add_type(ident.val_lex, "function")
  auto ident = ast_father_node->children[1];
  scope_manager->add_type(ident->token, "function");
}

void action2(std::shared_ptr<ASTNode> ast_father_node,
             std::shared_ptr<ScopeManager> scope_manager) {
  // PARAMLIST  ->  int ident PARAMLIST'
  // add_type(ident.val_lex, "int")
  auto ident = ast_father_node->children[1];
  scope_manager->add_type(ident->token, "int");
}

void action3(std::shared_ptr<ASTNode> ast_father_node,
             std::shared_ptr<ScopeManager> scope_manager) {
  // PARAMLIST  ->  float ident PARAMLIST'
  // add_type(ident.val_lex, "float")
  auto ident = ast_father_node->children[1];
  scope_manager->add_type(ident->token, "float");
}

void action4(std::shared_ptr<ASTNode> ast_father_node,
             std::shared_ptr<ScopeManager> scope_manager) {
  // PARAMLIST  ->  string ident PARAMLIST'
  // add_type(ident.val_lex, "string")
  auto ident = ast_father_node->children[1];
  scope_manager->add_type(ident->token, "string");
}

void action5(std::shared_ptr<ASTNode> ast_father_node,
             std::shared_ptr<ScopeManager> scope_manager) {
  // INDEX  ->  [int_constant] INDEX1
  // INDEX1.initial_type = INDEX.initial_type
  auto index = ast_father_node;
  auto index1 = ast_father_node->children[3];
  index1->decSDT.initial_type = index->decSDT.initial_type;
}

void action6(std::shared_ptr<ASTNode> ast_father_node,
             std::shared_ptr<ScopeManager> scope_manager) {
  // INDEX  ->  [int_constant] INDEX1
  // INDEX.type = array(int_constant.val_lex, INDEX1.type)
  auto index = ast_father_node;
  auto index1 = ast_father_node->children[3];
  auto int_const = ast_father_node->children[1];
  index->decSDT.type =
      "array(" + int_const->token->lexeme + ", " + index1->decSDT.type + ')';
}

void action7(std::shared_ptr<ASTNode> ast_father_node,
             std::shared_ptr<ScopeManager> scope_manager) {
  // INDEX->  ''
  // INDEX.type = INDEX.initial_type
  auto index = ast_father_node;
  index->decSDT.type = index->decSDT.initial_type;
}

void action8(std::shared_ptr<ASTNode> ast_father_node,
             std::shared_ptr<ScopeManager> scope_manager) {
  // VARDECL  ->  int ident INDEX
  // INDEX.initial_type = "int"
  auto index = ast_father_node->children[2];
  index->decSDT.initial_type = "int";
}

void action9(std::shared_ptr<ASTNode> ast_father_node,
             std::shared_ptr<ScopeManager> scope_manager) {
  // VARDECL  ->  float ident INDEX
  // INDEX.initial_type = "float"
  auto index = ast_father_node->children[2];
  index->decSDT.initial_type = "float";
}

void action10(std::shared_ptr<ASTNode> ast_father_node,
              std::shared_ptr<ScopeManager> scope_manager) {
  // VARDECL  ->  string ident INDEX
  // INDEX.initial_type = "string"
  auto index = ast_father_node->children[2];
  index->decSDT.initial_type = "string";
}

void action11(std::shared_ptr<ASTNode> ast_father_node,
              std::shared_ptr<ScopeManager> scope_manager) {
  // VARDECL  ->  int ident INDEX
  // VARDECL  ->  float ident INDEX
  // VARDECL  ->  string ident INDEX
  // add_type(ident.val_lex, INDEX.type)
  auto ident = ast_father_node->children[1];
  auto index = ast_father_node->children[2];
  scope_manager->add_type(ident->token, index->decSDT.type);
}
}  // namespace DEC

namespace DECLARE_VERIFY {
void action1(std::shared_ptr<ASTNode> ast_father_node,
             std::shared_ptr<ScopeManager> scope_manager) {
  // FUNCCALL  ->  call ( ident ( PARAMLISTCALL ) )
  // is_declared(ident.val_lex)
  // is_type(ident.val_lex, "function")
  auto ident = ast_father_node->children[2];
  if (!scope_manager->is_declared(ident->token)) {
    std::string msg_error = ident->token->lexeme +
        " used before declaration at " +
        std::to_string(ident->token->line) + ':' +
        std::to_string(ident->token->column);
    throw SemanticError(msg_error);
  }
  if (!scope_manager->is_type(ident->token, "function")) {
    std::string msg_error =
        ident->token->lexeme + " at " + std::to_string(ident->token->line) +
            ':' + std::to_string(ident->token->column) + " is not a function";
    throw SemanticError(msg_error);
  }
}

void action2(std::shared_ptr<ASTNode> ast_father_node,
             std::shared_ptr<ScopeManager> scope_manager) {
  // PARAMLISTCALL  -> ident PARAMLISTCALL'
  // LVALUE  ->  ident ALLOCAUX
  // is_declared(ident.val_lex)
  auto ident = ast_father_node->children[0];
  if (!scope_manager->is_declared(ident->token)) {
    std::string msg_error = ident->token->lexeme +
        " used before declaration at " +
        std::to_string(ident->token->line) + ':' +
        std::to_string(ident->token->column);
    throw SemanticError(msg_error);
  }
}

}  // namespace DECLARE_VERIFY

namespace BREAK {
void action1(std::shared_ptr<ASTNode> ast_father_node,
             std::shared_ptr<ScopeManager> scope_manager) {
  // STATEMENT  ->  break ;
  if (!scope_manager->is_in_scope("for")) {
    auto break_t = ast_father_node->children[0];
    std::string msg_error = "break used outside for statement at " +
        std::to_string(break_t->token->line) + ':' +
        std::to_string(break_t->token->column);
    throw SemanticError(msg_error);
  }
}
}  // namespace BREAK
std::string AUXILIAR::getType(std::string type, int array_counter) {
  char f = type[0];
  std::string final;
  if (f == 'i') {
    return "int";
  } else if (f == 'f') {
    return "float";
  } else if (f == 's') {
    return "string";
  }
  if (!(f == 'a')) {
    throw SemanticError("TIPO INVALIDO" + type);
  }

  int counter = 0;
  for (auto i = type.begin(); i < type.end(); ++i) {
    if (*i == '(') {
      counter++;
    } else if (*i == 'i') {
      final = "int";
    } else if (*i == 'f') {
      final = "float";
    } else if (*i == 's') {
      final = "string";
    }
  }

  if (counter != array_counter) {
    throw SemanticError("tentativa de operações com arrays");
  }
  return final;
}
std::vector<int> AUXILIAR::getArrayMax(std::string type) {
  std::vector<int> values;
  for (auto i = type.begin(); i < type.end(); ++i) {
    int d = *i - '0';
    if (d > 0 && d < 10) {
      values.push_back(d);
    }

  }
  return values;
}

namespace EXPA {

void evaluateIdent(std::shared_ptr<ASTNode> ast_father_node,
                   std::shared_ptr<ScopeManager> scope_manager) {
  // LVALUE -> IDENT ALLOCAUX
  auto &LVALUE = ast_father_node;
  auto &IDENT = ast_father_node->children[0];
  auto &ALLOCAUX = ast_father_node->children[1];

  std::string type = scope_manager->get_type(IDENT->token);
  std::vector<int> max = AUXILIAR::getArrayMax(type);

  LVALUE->mathSDT.array_counter = ALLOCAUX->mathSDT.array_counter;

  LVALUE->code->splice(LVALUE->code->end(), *ALLOCAUX->code);
  if (LVALUE->mathSDT.array_counter > 0) {
    auto arrayIndexation = ALLOCAUX->mathSDT.arrayVar;

    std::string temp;
    int counter = 0;
    std::string somador = ExpressionTreeResolver::generateTempVar();
    LVALUE->code->push_back(somador + " = " + '0');
    for (auto k = arrayIndexation->begin(); k != arrayIndexation->end(); k++) {
      std::string var = ExpressionTreeResolver::generateTempVar();
      temp = var + " = " + *k;
      LVALUE->code->push_back(temp);
      for (int j = 0; j < counter; ++j) {
        temp = var + " = " + var + " * " + (char) (max[j] + 48);
        LVALUE->code->push_back(temp);
      }
      LVALUE->code->push_back(somador + " = " + somador + " + " + var);
      counter++;

    }
    LVALUE->mathSDT.node =
        std::make_shared<Node>('n', type, IDENT->token->lexeme + '[' + somador + ']');

  } else {
    LVALUE->mathSDT.node =
        std::make_shared<Node>('n', type, IDENT->token->lexeme);
  }

}

void lexemeToValue(std::shared_ptr<ASTNode> ast_father_node,
                   std::shared_ptr<ScopeManager> scope_manager) {
  // FACTOR  ->  int_constant
  // Factor.Node = new Node(value)
  auto &FACTOR = ast_father_node;
  auto &NI = ast_father_node->children[0];
  std::string type;
  if (NI->id == "NI") {
    type = "int";
  } else if (NI->id == "NPF") {
    type = "float";
  } else if (NI->id == "STRC") {
    type = "string";
  }
  FACTOR->mathSDT.node = std::make_shared<Node>('n', type, NI->token->lexeme);
}

void setOperation(std::shared_ptr<ASTNode> ast_father_node,
                  std::shared_ptr<ScopeManager> scope_manager) {
  // op= {+,-}
  //   NUMEXPRESSIONAUX  ->  op NUMEXPRESSION
  // NUMEXPRESSIONAUX.character = op NUMEXPRESSIONAUX.node =
  // NUMEXPRESSION.node;
  auto &NUMEXPRESSIONAUX = ast_father_node;
  auto &OPERATION = ast_father_node->children[0];
  auto &NUMEXPRESSION = ast_father_node->children[1];
  NUMEXPRESSIONAUX->mathSDT.operation = OPERATION->token->lexeme[0];
  NUMEXPRESSIONAUX->mathSDT.node = NUMEXPRESSION->mathSDT.node;
  NUMEXPRESSIONAUX->mathSDT.array_counter =
      NUMEXPRESSION->mathSDT.array_counter;
  NUMEXPRESSIONAUX->code = NUMEXPRESSION->code;
  NUMEXPRESSIONAUX->code_res_var = NUMEXPRESSION->code_res_var;
}

void identUp(std::shared_ptr<ASTNode> ast_father_node,
             std::shared_ptr<ScopeManager> scope_manager) {
  //  FACTOR  ->  LVALUE
  auto &FACTOR = ast_father_node;
  auto &LVALUE = ast_father_node->children[0];
  FACTOR->mathSDT.node = LVALUE->mathSDT.node;
  FACTOR->mathSDT.array_counter = LVALUE->mathSDT.array_counter;
  FACTOR->code->splice(FACTOR->code->end(), *LVALUE->code);
}

void valueUp(std::shared_ptr<ASTNode> ast_father_node,
             std::shared_ptr<ScopeManager> scope_manager) {
  // UNARYEXPR -> FACTOR
  auto &UNARYEXPR = ast_father_node;
  auto &FACTOR = ast_father_node->children[0];
  UNARYEXPR->mathSDT.array_counter = FACTOR->mathSDT.array_counter;
  UNARYEXPR->mathSDT.node = FACTOR->mathSDT.node;
  UNARYEXPR->code_res_var = FACTOR->code_res_var;
  UNARYEXPR->code->splice(UNARYEXPR->code->end(), *FACTOR->code);
}

void secondChildValueUp(std::shared_ptr<ASTNode> ast_father_node,
                        std::shared_ptr<ScopeManager> scope_manager) {
  auto &UNARYEXPR = ast_father_node;
  auto &SYMBOL = ast_father_node->children[0];
  auto &FACTOR = ast_father_node->children[1];

  UNARYEXPR->code_res_var.var = ExpressionTreeResolver::generateTempVar();

  UNARYEXPR->code->push_back(UNARYEXPR->code_res_var.var + " = " +
      SYMBOL->token->lexeme[0] +
      FACTOR->mathSDT.node->value);
  UNARYEXPR->mathSDT.node = std::make_shared<Node>(SYMBOL->token->lexeme[0],
                                                   "N", FACTOR->mathSDT.node);
}

void generateNode(std::shared_ptr<ASTNode> ast_father_node,
                  std::shared_ptr<ScopeManager> scope_manager) {
  // NUMEXPRESSION  ->  TERM NUMEXPRESSIONAUX
  //
  auto &NUMEXPRESSION = ast_father_node;
  auto TERM = ast_father_node->children[0];
  auto NUMEXPRESSIONAUX = ast_father_node->children[1];

  if (NUMEXPRESSIONAUX->mathSDT.node != nullptr) {
    TERM->mathSDT.node->type = AUXILIAR::getType(
        TERM->mathSDT.node->type, TERM->mathSDT.array_counter);
    NUMEXPRESSIONAUX->mathSDT.node->type =
        AUXILIAR::getType(NUMEXPRESSIONAUX->mathSDT.node->type,
                          NUMEXPRESSIONAUX->mathSDT.array_counter);

    std::string varA;
    std::string varB;
    if (TERM->code_res_var.var != "") {
      varA = TERM->code_res_var.var;
    } else {
      varA = TERM->mathSDT.node->value;
    }
    if (NUMEXPRESSIONAUX->code_res_var.var != "") {
      varB = NUMEXPRESSIONAUX->code_res_var.var;
    } else {
      varB = NUMEXPRESSIONAUX->mathSDT.node->value;
    }

    NUMEXPRESSION->code->splice(NUMEXPRESSION->code->end(), *TERM->code);
    NUMEXPRESSION->code->splice(NUMEXPRESSION->code->end(),
                                *NUMEXPRESSIONAUX->code);
    NUMEXPRESSION->code_res_var.var =
        ExpressionTreeResolver::generateTempVar();
    NUMEXPRESSION->code->push_back(
        NUMEXPRESSION->code_res_var.var + " = " + varA + ' ' +
            NUMEXPRESSIONAUX->mathSDT.operation + ' ' + varB);

    NUMEXPRESSION->mathSDT.node = std::make_shared<Node>(
        NUMEXPRESSIONAUX->mathSDT.operation, "N", TERM->mathSDT.node,
        NUMEXPRESSIONAUX->mathSDT.node);
  } else {
    NUMEXPRESSION->mathSDT.node = TERM->mathSDT.node;
    NUMEXPRESSION->mathSDT.array_counter = TERM->mathSDT.array_counter;
    NUMEXPRESSION->code_res_var = TERM->code_res_var;
    NUMEXPRESSION->code->splice(NUMEXPRESSION->code->end(), *TERM->code);
  }
}
void setOperation2(std::shared_ptr<ASTNode> ast_father_node,
                   std::shared_ptr<ScopeManager> scope_manager) {
  //  UNARYEXPRAUX  ->  * TERM
  //
  auto &UNARYEXPRAUX = ast_father_node;
  auto &OP = ast_father_node->children[0];
  auto &TERM = ast_father_node->children[1];
  UNARYEXPRAUX->mathSDT.operation = OP->token->lexeme[0];
  UNARYEXPRAUX->mathSDT.node = TERM->mathSDT.node;
  UNARYEXPRAUX->mathSDT.array_counter = TERM->mathSDT.array_counter;
  UNARYEXPRAUX->code->splice(UNARYEXPRAUX->code->end(), *TERM->code);
  UNARYEXPRAUX->code_res_var = TERM->code_res_var;
}
void term(std::shared_ptr<ASTNode> ast_father_node,
          std::shared_ptr<ScopeManager> scope_manager) {
  // TERM -> UNARYEXPR UNARYEXPRAUX
  auto &TERM = ast_father_node;
  auto &UNARYEXPR = ast_father_node->children[0];
  auto &UNARYEXPRAUX = ast_father_node->children[1];
  if (UNARYEXPRAUX->mathSDT.node == nullptr) {
    TERM->mathSDT.node = UNARYEXPR->mathSDT.node;
    TERM->mathSDT.array_counter = UNARYEXPR->mathSDT.array_counter;
    TERM->code->splice(TERM->code->end(), *UNARYEXPR->code);
    TERM->code_res_var.var = ExpressionTreeResolver::generateTempVar();

    std::string varA;
    if (UNARYEXPR->code_res_var.var != "") {
      varA = UNARYEXPR->code_res_var.var;
    } else {
      varA = UNARYEXPR->mathSDT.node->value;
    }
    if (UNARYEXPR->mathSDT.array_counter > 0) {
      TERM->code->push_back(TERM->code_res_var.var + " = " + varA);
    } else {
      TERM->code->push_back(TERM->code_res_var.var + " = " + varA);
    }
  } else {
    UNARYEXPR->mathSDT.node->type = AUXILIAR::getType(
        UNARYEXPR->mathSDT.node->type, UNARYEXPR->mathSDT.array_counter);
    UNARYEXPRAUX->mathSDT.node->type =
        AUXILIAR::getType(UNARYEXPRAUX->mathSDT.node->type,
                          UNARYEXPRAUX->mathSDT.array_counter);

    std::string varA;
    std::string varB;
    if (UNARYEXPR->code_res_var.var != "") {
      varA = UNARYEXPR->code_res_var.var;
    } else {
      varA = UNARYEXPR->mathSDT.node->value;
    }
    if (UNARYEXPRAUX->code_res_var.var != "") {
      varB = UNARYEXPRAUX->code_res_var.var;
    } else {
      varB = UNARYEXPRAUX->mathSDT.node->value;
    }

    TERM->code->splice(TERM->code->end(), *UNARYEXPR->code);
    TERM->code->splice(TERM->code->end(), *UNARYEXPRAUX->code);
    TERM->code_res_var.var = ExpressionTreeResolver::generateTempVar();
    TERM->code->push_back(TERM->code_res_var.var + " = " + varA + ' ' +
        UNARYEXPRAUX->mathSDT.operation + ' ' + varB);

    TERM->mathSDT.node = std::make_shared<Node>(
        UNARYEXPRAUX->mathSDT.operation, "N", UNARYEXPR->mathSDT.node,
        UNARYEXPRAUX->mathSDT.node);
  }
}
void printExpression0_h(std::shared_ptr<ASTNode> ast_father_node,
                        std::shared_ptr<ScopeManager> scope_manager) {
  auto &NUMEXPRESSION = ast_father_node->children[0];
  auto &EXPRESSION2 = ast_father_node->children[1];
  EXPRESSION2->code_res_var.var = NUMEXPRESSION->code_res_var.var;
}
void printExpression0(std::shared_ptr<ASTNode> ast_father_node,
                      std::shared_ptr<ScopeManager> scope_manager) {
  auto &EXPRESSION = ast_father_node;
  auto &NUMEXPRESSION = ast_father_node->children[0];
  auto &EXPRESSION2 = ast_father_node->children[1];

  EXPRESSION->code_res_var = NUMEXPRESSION->code_res_var;

  EXPRESSION->code->splice(EXPRESSION->code->end(), *NUMEXPRESSION->code);
  EXPRESSION->code->splice(EXPRESSION->code->end(), *EXPRESSION2->code);
  ExpressionTreeResolver::printBT(NUMEXPRESSION->mathSDT.node);
}

void passNum(std::shared_ptr<ASTNode> ast_father_node,
             std::shared_ptr<ScopeManager> scope_manager) {
  auto &FACTOR = ast_father_node;
  auto &NUMEXPRESSION = ast_father_node->children[1];
  FACTOR->code_res_var = NUMEXPRESSION->code_res_var;
  FACTOR->code->splice(FACTOR->code->end(), *NUMEXPRESSION->code);
  FACTOR->mathSDT.node = NUMEXPRESSION->mathSDT.node;
}
void printExpression1(std::shared_ptr<ASTNode> ast_father_node,
                      std::shared_ptr<ScopeManager> scope_manager) {
  auto &NUMEXPRESSION = ast_father_node->children[1];
  ExpressionTreeResolver::printBT(NUMEXPRESSION->mathSDT.node);
}

void printExpression2(std::shared_ptr<ASTNode> ast_father_node,
                      std::shared_ptr<ScopeManager> scope_manager) {
  auto &NUMEXPRESSION = ast_father_node->children[2];
  ExpressionTreeResolver::printBT(NUMEXPRESSION->mathSDT.node);
}

void arrayCounter(std::shared_ptr<ASTNode> ast_father_node,
                  std::shared_ptr<ScopeManager> scope_manager) {
  // ALLOCAUX  ->  [ NUMEXPRESSION ] ALLOCAUX1
  auto &ALLOCAUX = ast_father_node;
  auto &NUMEXPRESSION = ast_father_node->children[1];
  auto &ALLOCAUX1 = ast_father_node->children[3];
  ALLOCAUX->mathSDT.arrayVar->push_back(NUMEXPRESSION->code_res_var.var);
  ALLOCAUX->mathSDT.arrayVar->splice(ALLOCAUX->mathSDT.arrayVar->end(), *ALLOCAUX1->mathSDT.arrayVar);
  ALLOCAUX->mathSDT.array_counter = ALLOCAUX1->mathSDT.array_counter + 1;
  ALLOCAUX->code->splice(ALLOCAUX->code->end(), *NUMEXPRESSION->code);
  ALLOCAUX->code->splice(ALLOCAUX->code->end(), *ALLOCAUX1->code);

}

}  // namespace EXPA

#ifndef __semantic_actions_h
#define __semantic_actions_h

#include "ast_node.h"
#include "scope_manager.h"
#include "tokens.h"

namespace CODE {
void relop_action(std::shared_ptr<ASTNode> ast_father_node,
                  std::shared_ptr<ScopeManager> scope_manager);

void handle_break(std::shared_ptr<ASTNode> ast_father_node,
                  std::shared_ptr<ScopeManager> scope_manager);

void handle_readstat(std::shared_ptr<ASTNode> ast_father_node,
                     std::shared_ptr<ScopeManager> scope_manager);

void handle_printstat(std::shared_ptr<ASTNode> ast_father_node,
                      std::shared_ptr<ScopeManager> scope_manager);

void get_lvalue_var(std::shared_ptr<ASTNode> ast_father_node,
                    std::shared_ptr<ScopeManager> scope_manager);
void get_return_val(std::shared_ptr<ASTNode> ast_father_node,
                    std::shared_ptr<ScopeManager> scope_manager);
void handle_return(std::shared_ptr<ASTNode> ast_father_node,
                   std::shared_ptr<ScopeManager> scope_manager);
void handle_return2(std::shared_ptr<ASTNode> ast_father_node,
                    std::shared_ptr<ScopeManager> scope_manager);

void get_params(std::shared_ptr<ASTNode> ast_father_node,
                std::shared_ptr<ScopeManager> scope_manager);
void paramlistcall(std::shared_ptr<ASTNode> ast_father_node,
                   std::shared_ptr<ScopeManager> scope_manager);
void funccall(std::shared_ptr<ASTNode> ast_father_node,
              std::shared_ptr<ScopeManager> scope_manager);

void goto_main(std::shared_ptr<ASTNode> ast_father_node,
               std::shared_ptr<ScopeManager> scope_manager);

void funcdef_h(std::shared_ptr<ASTNode> ast_father_node,
               std::shared_ptr<ScopeManager> scope_manager);
void funcdef_s(std::shared_ptr<ASTNode> ast_father_node,
               std::shared_ptr<ScopeManager> scope_manager);

void empty_else_action(std::shared_ptr<ASTNode> ast_father_node,
                       std::shared_ptr<ScopeManager> scope_manager);
void else_action_h(std::shared_ptr<ASTNode> ast_father_node,
                   std::shared_ptr<ScopeManager> scope_manager);
void else_action_s(std::shared_ptr<ASTNode> ast_father_node,
                   std::shared_ptr<ScopeManager> scope_manager);
void if_action_s(std::shared_ptr<ASTNode> ast_father_node,
                 std::shared_ptr<ScopeManager> scope_manager);

void for_action(std::shared_ptr<ASTNode> ast_father_node,
                std::shared_ptr<ScopeManager> scope_manager);
void inherit_for_next(std::shared_ptr<ASTNode> ast_father_node,
                      std::shared_ptr<ScopeManager> scope_manager);

void assign(std::shared_ptr<ASTNode> ast_father_node,
            std::shared_ptr<ScopeManager> scope_manager);

void set_expression_val(std::shared_ptr<ASTNode> ast_father_node,
                        std::shared_ptr<ScopeManager> scope_manager);

void get_code_from_childs(std::shared_ptr<ASTNode> ast_father_node,
                          std::shared_ptr<ScopeManager> scope_manager);
void get_code_from_childs_2(std::shared_ptr<ASTNode> ast_father_node,
                            std::shared_ptr<ScopeManager> scope_manager);

void inherit_next(std::shared_ptr<ASTNode> ast_father_node,
                  std::shared_ptr<ScopeManager> scope_manager);
void set_final_label(std::shared_ptr<ASTNode> ast_father_node,
                     std::shared_ptr<ScopeManager> scope_manager);
void set_label(std::shared_ptr<ASTNode> ast_father_node,
               std::shared_ptr<ScopeManager> scope_manager);
void gen_final_code(std::shared_ptr<ASTNode> ast_father_node,
                    std::shared_ptr<ScopeManager> scope_manager);
void print_code(std::shared_ptr<ASTNode> ast_father_node,
                std::shared_ptr<ScopeManager> scope_manager);
}  // namespace CODE

// Gerência de escopos e adição de símbolos nas tabelas de símbolo
namespace SCOPE {
void action1(std::shared_ptr<ASTNode> ast_father_node,
             std::shared_ptr<ScopeManager> scope_manager);
void action2(std::shared_ptr<ASTNode> ast_father_node,
             std::shared_ptr<ScopeManager> scope_manager);
void action3(std::shared_ptr<ASTNode> ast_father_node,
             std::shared_ptr<ScopeManager> scope_manager);
void action4(std::shared_ptr<ASTNode> ast_father_node,
             std::shared_ptr<ScopeManager> scope_manager);
void action5(std::shared_ptr<ASTNode> ast_father_node,
             std::shared_ptr<ScopeManager> scope_manager);
void action6(std::shared_ptr<ASTNode> ast_father_node,
             std::shared_ptr<ScopeManager> scope_manager);
void action7(std::shared_ptr<ASTNode> ast_father_node,
             std::shared_ptr<ScopeManager> scope_manager);
}  // namespace SCOPE

// Inserção de tipos na tabela de símbolos
namespace DEC {
void action1(std::shared_ptr<ASTNode> ast_father_node,
             std::shared_ptr<ScopeManager> scope_manager);
void action2(std::shared_ptr<ASTNode> ast_father_node,
             std::shared_ptr<ScopeManager> scope_manager);
void action3(std::shared_ptr<ASTNode> ast_father_node,
             std::shared_ptr<ScopeManager> scope_manager);
void action4(std::shared_ptr<ASTNode> ast_father_node,
             std::shared_ptr<ScopeManager> scope_manager);
void action5(std::shared_ptr<ASTNode> ast_father_node,
             std::shared_ptr<ScopeManager> scope_manager);
void action6(std::shared_ptr<ASTNode> ast_father_node,
             std::shared_ptr<ScopeManager> scope_manager);
void action7(std::shared_ptr<ASTNode> ast_father_node,
             std::shared_ptr<ScopeManager> scope_manager);
void action8(std::shared_ptr<ASTNode> ast_father_node,
             std::shared_ptr<ScopeManager> scope_manager);
void action9(std::shared_ptr<ASTNode> ast_father_node,
             std::shared_ptr<ScopeManager> scope_manager);
void action10(std::shared_ptr<ASTNode> ast_father_node,
              std::shared_ptr<ScopeManager> scope_manager);
void action11(std::shared_ptr<ASTNode> ast_father_node,
              std::shared_ptr<ScopeManager> scope_manager);
}  // namespace DEC

// Verificação de uso de variável declarada
namespace DECLARE_VERIFY {
void action1(std::shared_ptr<ASTNode> ast_father_node,
             std::shared_ptr<ScopeManager> scope_manager);
void action2(std::shared_ptr<ASTNode> ast_father_node,
             std::shared_ptr<ScopeManager> scope_manager);
}  // namespace DECLARE_VERIFY

// Verificação de break dentro de escopo de comando de repetição
namespace BREAK {
void action1(std::shared_ptr<ASTNode> ast_father_node,
             std::shared_ptr<ScopeManager> scope_manager);
}

namespace AUXILIAR {
std::string getType(std::string type, int array_counter);
std::vector<int> getArrayMax(std::string type);
}
namespace EXPA {
void arrayCounter(std::shared_ptr<ASTNode> ast_father_node,
                  std::shared_ptr<ScopeManager> scope_manager);
void passNum(std::shared_ptr<ASTNode> ast_father_node,
             std::shared_ptr<ScopeManager> scope_manager);
void printExpression0_h(std::shared_ptr<ASTNode> ast_father_node,
                        std::shared_ptr<ScopeManager> scope_manager);
void printExpression0(std::shared_ptr<ASTNode> ast_father_node,
                      std::shared_ptr<ScopeManager> scope_manager);
void printExpression1(std::shared_ptr<ASTNode> ast_father_node,
                      std::shared_ptr<ScopeManager> scope_manager);
void printExpression2(std::shared_ptr<ASTNode> ast_father_node,
                      std::shared_ptr<ScopeManager> scope_manager);
void evaluateIdent(std::shared_ptr<ASTNode> ast_father_node,
                   std::shared_ptr<ScopeManager> scope_manager);
void lexemeToValue(std::shared_ptr<ASTNode> ast_father_node,
                   std::shared_ptr<ScopeManager> scope_manager);
void setOperation(std::shared_ptr<ASTNode> ast_father_node,
                  std::shared_ptr<ScopeManager> scope_manager);
void identUp(std::shared_ptr<ASTNode> ast_father_node,
             std::shared_ptr<ScopeManager> scope_manager);
void valueUp(std::shared_ptr<ASTNode> ast_father_node,
             std::shared_ptr<ScopeManager> scope_manager);
void secondChildValueUp(std::shared_ptr<ASTNode> ast_father_node,
                        std::shared_ptr<ScopeManager> scope_manager);
void generateNode(std::shared_ptr<ASTNode> ast_father_node,
                  std::shared_ptr<ScopeManager> scope_manager);
void setOperation2(std::shared_ptr<ASTNode> ast_father_node,
                   std::shared_ptr<ScopeManager> scope_manager);
void term(std::shared_ptr<ASTNode> ast_father_node,
          std::shared_ptr<ScopeManager> scope_manager);
}  // namespace EXPA

#endif

#include "symbols_table.h"
#include "errors.h"

void SymbolsTable::add_symbol(std::shared_ptr<Token> token) {
    if (_table.find(token->lexeme) == _table.end()) {
        _table[token->lexeme] = std::make_shared<TableEntry>();
    }
    _table[token->lexeme]->lines.push_back(token->line);
}

void SymbolsTable::add_type(std::shared_ptr<Token> token, const std::string& type) {
    if (!is_in_table(token->lexeme)) return;
    _table[token->lexeme]->type = type;
}

bool SymbolsTable::is_in_table(std::string& lexeme) {
    return _table.find(lexeme) != _table.end();
}

bool SymbolsTable::is_type(std::shared_ptr<Token> token, const std::string& type) {
    if (!is_in_table(token->lexeme)) return false;
    return _table[token->lexeme]->type == type;
}

void SymbolsTable::print(std::ostream &out) {
    for (const auto &pair : _table) {
        out << "\tSymbol: " << pair.first << "\n\t\ttype: " << pair.second->type;
        out << "\n\t\tlines: [";
        for (uint i = 0; i < pair.second->lines.size(); i++) {
            if (i == pair.second->lines.size() - 1) {
                out << pair.second->lines[i] << "]" << std::endl;
            } else {
                out << pair.second->lines[i] << ",";
            }
        }
    }
}
std::string SymbolsTable::get_type(std::shared_ptr<Token> token) {
  if (is_in_table(token->lexeme)) {
      return _table[token->lexeme]->type;
  }
  throw SemanticError("Lexeme type not found");
}

#ifndef __symbol_table_h
#define __symbol_table_h

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "tokens.h"

struct TableEntry {
    std::string type = "";
    std::vector<int> lines = {};
};

class SymbolsTable {
   private:
    std::unordered_map<std::string, std::shared_ptr<TableEntry>> _table;

   public:
    void add_symbol(std::shared_ptr<Token> token);
    void add_type(std::shared_ptr<Token> token, const std::string& type);
    bool is_in_table(std::string& lexeme);
    bool is_type(std::shared_ptr<Token> token, const std::string& type);
    std::string get_type(std::shared_ptr<Token> token);
    void print(std::ostream& out);
};

#endif

#include "token_identifiers.h"

std::shared_ptr<TokenIdentifier> identTokenIdentifier =
    std::make_shared<TokenIdentifier>(
        "IDENT", std::make_shared<FiniteStateMachine>(
                     std::unordered_map<std::string,
                                        std::function<std::string(char, bool)>>{
                         {"0",
                          [](char c, bool is_eof) {
                              if (is_eof) return "dead";
                              if (isAlpha(c) || c == '_') return "1";
                              return "dead";
                          }},
                         {"1",
                          [](char c, bool is_eof) {
                              if (is_eof) return "final";
                              if (isAlpha(c) || isNumeric(c) || c == '_') return "1";
                              return "final";
                          }},
                     },
                     "0", std::set<std::string>{"final"},
                     std::set<std::string>{"final"}));

std::shared_ptr<TokenIdentifier> integerTokenIdentifier =
    std::make_shared<TokenIdentifier>(
        "NI", std::make_shared<FiniteStateMachine>(
                  std::unordered_map<std::string,
                                     std::function<std::string(char, bool)>>{
                      {"0",
                       [](char c, bool is_eof) {
                           if (is_eof) return "dead";
                           if (isNumeric(c)) return "1";
                           return "dead";
                       }},
                      {"1",
                       [](char c, bool is_eof) {
                           if (is_eof) return "final";

                           if (isNumeric(c)) return "1";
                           if (c == 'e') return "2";
                           return "final";
                       }},
                      {"2",
                       [](char c, bool is_eof) {
                           if (is_eof) return "dead";
                           if (isNumeric(c)) return "3";
                           if (c == '+') return "4";
                           return "dead";
                       }},
                      {"3",
                       [](char c, bool is_eof) {
                           if (is_eof) return "final";
                           if (isNumeric(c)) return "3";
                           return "final";
                       }},
                      {"4",
                       [](char c, bool is_eof) {
                           if (is_eof) return "dead";
                           if (isNumeric(c)) return "3";
                           return "dead";
                       }},
                  },
                  "0", std::set<std::string>{"final"},
                  std::set<std::string>{"final"}));

std::shared_ptr<TokenIdentifier>
    floatTokenIdentifier =
        std::make_shared<TokenIdentifier>("NPF",
                                          std::make_shared<FiniteStateMachine>(
                                              std::unordered_map<
                                                  std::string,
                                                  std::function<std::string(
                                                      char, bool)>>{
                                                  {"0",
                                                   [](char c, bool is_eof) {
                                                       if (is_eof)
                                                           return "dead";
                                                       if (c == '.') return "1";
                                                       if (isNumeric(c))
                                                           return "6";
                                                       return "dead";
                                                   }},
                                                  {"1",
                                                   [](char c, bool is_eof) {
                                                       if (is_eof)
                                                           return "dead";
                                                       if (isNumeric(c))
                                                           return "2";
                                                       return "dead";
                                                   }},
                                                  {"2",
                                                   [](char c, bool is_eof) {
                                                       if (is_eof)
                                                           return "final";
                                                       if (isNumeric(c))
                                                           return "2";
                                                       if (c == 'e') return "3";
                                                       return "final";
                                                   }},
                                                  {"3",
                                                   [](char c, bool is_eof) {
                                                       if (is_eof)
                                                           return "dead";
                                                       if (isNumeric(c))
                                                           return "4";
                                                       if (c == '+' || c == '-')
                                                           return "5";
                                                       return "dead";
                                                   }},
                                                  {"4",
                                                   [](char c, bool is_eof) {
                                                       if (is_eof)
                                                           return "final";
                                                       if (isNumeric(c))
                                                           return "4";
                                                       return "final";
                                                   }},
                                                  {"5",
                                                   [](char c, bool is_eof) {
                                                       if (is_eof)
                                                           return "dead";
                                                       if (isNumeric(c))
                                                           return "4";
                                                       return "dead";
                                                   }},
                                                  {"6",
                                                   [](char c, bool is_eof) {
                                                       if (is_eof)
                                                           return "dead";
                                                       if (isNumeric(c))
                                                           return "6";
                                                       if (c == '.') return "2";
                                                       if (c == 'e') return "7";
                                                       return "dead";
                                                   }},
                                                  {"7",
                                                   [](char c, bool is_eof) {
                                                       if (is_eof)
                                                           return "final";
                                                       if (c == '-') return "5";
                                                       return "final";
                                                   }},
                                              },
                                              "0",
                                              std::set<std::string>{"final"},
                                              std::set<std::string>{"final"}));

std::shared_ptr<TokenIdentifier> mathOperatorTokenIdentifier =
    std::make_shared<TokenIdentifier>(
        "MATHOP",
        std::make_shared<FiniteStateMachine>(
            std::unordered_map<std::string,
                               std::function<std::string(char, bool)>>{
                {"0",
                 [](char c, bool is_eof) {
                     if (is_eof) return "dead";
                     if (c == '+' || c == '-' || c == '*' || c == '/' ||
                         c == '%' || c == '=')
                         return "final";
                     return "dead";
                 }},
            },
            "0", std::set<std::string>{"final"}));

std::shared_ptr<TokenIdentifier> oneCharTokenIdentifier =
    std::make_shared<TokenIdentifier>(
        "ONECHAR",
        std::make_shared<FiniteStateMachine>(
            std::unordered_map<std::string,
                               std::function<std::string(char, bool)>>{
                {"0",
                 [](char c, bool is_eof) {
                     if (is_eof) return "dead";
                     if (c == '(' || c == ')' || c == '{' || c == '}' ||
                         c == '[' || c == ']' || c == ';' || c == ',')
                         return "final";
                     return "dead";
                 }},
            },
            "0", std::set<std::string>{"final"}));

std::shared_ptr<TokenIdentifier> stringConstCharTokenIdentifier =
    std::make_shared<TokenIdentifier>(
        "STRC", std::make_shared<FiniteStateMachine>(
                    std::unordered_map<std::string,
                                       std::function<std::string(char, bool)>>{
                        {"0",
                         [](char c, bool is_eof) {
                             if (is_eof) return "dead";
                             if (c == '"') return "1";
                             return "dead";
                         }},
                        {"1",
                         [](char c, bool is_eof) {
                             if (is_eof) return "dead";
                             if (c != '"') return "1";
                             return "final";
                         }},
                    },
                    "0", std::set<std::string>{"final"}));

std::shared_ptr<TokenIdentifier> relopTokenIdentifier =
    std::make_shared<TokenIdentifier>(
        "RELOP", std::make_shared<FiniteStateMachine>(
                     std::unordered_map<std::string,
                                        std::function<std::string(char, bool)>>{
                         {"0",
                          [](char c, bool is_eof) {
                              if (is_eof) return "dead";
                              if (c == '>' || c == '<') return "1";
                              if (c == '=' || c == '!') return "2";
                              return "dead";
                          }},
                         {"1",
                          [](char c, bool is_eof) {
                              if (is_eof) return "final_1";
                              if (c == '=') return "final_2";
                              return "final_1";
                          }},
                         {"2",
                          [](char c, bool is_eof) {
                              if (is_eof) return "dead";
                              if (c == '=') return "final_2";
                              return "dead";
                          }},
                     },
                     "0", std::set<std::string>{"final_1", "final_2"},
                     std::set<std::string>{"final_1"}));

#ifndef __token_identifiers_h
#define __token_identifiers_h

#include <memory>

#include "fsm.h"
#include "tokens.h"

extern std::shared_ptr<TokenIdentifier> identTokenIdentifier;
extern std::shared_ptr<TokenIdentifier> integerTokenIdentifier;
extern std::shared_ptr<TokenIdentifier> floatTokenIdentifier;
extern std::shared_ptr<TokenIdentifier> mathOperatorTokenIdentifier;
extern std::shared_ptr<TokenIdentifier> relopTokenIdentifier;
extern std::shared_ptr<TokenIdentifier> oneCharTokenIdentifier;
extern std::shared_ptr<TokenIdentifier> stringConstCharTokenIdentifier;

#endif

#include "tokens.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#include "fsm.h"

bool isAlpha(char ch) { return std::isalpha(static_cast<unsigned char>(ch)); }

bool isNumeric(char ch) { return std::isdigit(static_cast<unsigned char>(ch)); }

bool isWhitespace(char c) {
    return std::isspace(static_cast<unsigned char>(c));
}

FSMStatus TokenIdentifier::analyze(char c, bool is_eof, int cursor_position, int line,
                                   int column) {
    auto prev_status = _identifier_machine->get_status();
    if (prev_status == FSMStatus::ERROR || prev_status == FSMStatus::SUCCESS)
        return prev_status;
    auto current_status = _identifier_machine->transition(c, is_eof);
    if (prev_status == FSMStatus::IDLE &&
        (current_status == FSMStatus::RUNNING ||
         current_status == FSMStatus::SUCCESS)) {
        cursor_start = cursor_position;
        start_line = line;
        start_column = column;
    }
    return current_status;
}

#ifndef __tokens_h
#define __tokens_h

#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#include "fsm.h"

bool isAlpha(char ch);

bool isNumeric(char ch);

bool isWhitespace(char c);

struct Token {
    std::string lexeme;
    std::string id;
    int line;
    int column;

    Token(const std::string &lexeme, const std::string &id, int line,
          int column)
        : lexeme(lexeme), id(id), line(line), column(column) {}
};

class TokenIdentifier {
   public:
    int cursor_start;
    int start_line;
    int start_column;

   protected:
    std::string _id;
    std::shared_ptr<FiniteStateMachine> _identifier_machine;

   public:
    TokenIdentifier(const std::string &id,
                    std::shared_ptr<FiniteStateMachine> identifier_machine)
        : _id(id), _identifier_machine(identifier_machine) {}
    std::string get_id() { return _id; }
    void reset() { _identifier_machine->reset(); }
    std::string get_lexeme() { return _identifier_machine->get_lexeme(); }
    bool has_already_succeeded() {
        return _identifier_machine->get_status() == FSMStatus::SUCCESS;
    }
    bool has_to_move_cursor_back() {
        return _identifier_machine->has_to_move_cursor_back();
    }
    FSMStatus analyze(char c, bool is_eof, int cursor_position, int line, int column);
};

#endif

//
// Created by vitorh on 13/12/24.
//
#include "type_enum.h"
#include "errors.h"

std::string enum_to_string(const Type type) {
  switch(type)
  {
    case Function:    return "FUNCTION";
    case Int:    return "INT";
    case Float:   return "FLOAT";
    case String: return "STRING";
  }
  throw SemanticError("Forbidden type");
}

std::ostream &operator<<(std::ostream &os, const Type type) {
  return os << enum_to_string(type);
}

std::string operator+(std::string str, const Type type) {
  return str + enum_to_string(type);
}


//
// Created by vitorh on 13/12/24.
//

#ifndef COMPILADORES_COMPILER_AST_NODE_CPP_TYPE_ENUM_H_
#define COMPILADORES_COMPILER_AST_NODE_CPP_TYPE_ENUM_H_

#include <iostream>

enum Type {Function,Int,Float,String};

std::string enum_to_string(const Type type);
std::ostream& operator<<(std::ostream& os, const Type type);
std::string operator+(std::string str, const Type type);
#endif //COMPILADORES_COMPILER_AST_NODE_CPP_TYPE_ENUM_H_

