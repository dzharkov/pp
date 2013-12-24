#include <memory>
#include <vector>
#include <sstream>
#include <algorithm>
#include <vector>
#include <stack>
#include "parser.h"
#include "ast.h"

std::vector<std::vector<LexemeType>> Parser::operators_ = 
{
    std::vector<LexemeType>{LexemeType::MULTIPLY, LexemeType::DIVIDE},
    std::vector<LexemeType>{LexemeType::PLUS, LexemeType::MINUS},
    std::vector<LexemeType>
    {
        LexemeType::MORE, LexemeType::MORE_OR_EQUALS, 
        LexemeType::EQUALS, LexemeType::NOT_EQUALS,
        LexemeType::LESS,LexemeType::LESS_OR_EQALS
    }
};

unsigned short int Parser::operator_priority(LexemeType type)
{
    for (size_t i = 0; i < operators_.size(); i++) 
    {
        for (LexemeType & op_type : operators_[i]) {
            if (op_type == type)
            {
                return operators_.size() - i;
            }
        }
    }

    return 0;
}

bool Parser::is_operator(LexemeType type) 
{
    return operator_priority(type) != 0;
}

ASTNodePtr Parser::parse() 
{
    scanner_.next_lexeme();
    LexemeType current_lex = scanner_.current_lexeme();
    
    StatementsSequence functions;
    StatementsSequence statements;
    
    while (current_lex != LexemeType::EOFL) 
    {
        if (current_lex == LexemeType::DEF) 
        {
            functions.push_back(parse_function_definition());
        } 
        else 
        {
            statements.push_back(parse_statement());
        }

        assert_current_lexeme(LexemeType::EOL);
        current_lex = scanner_.next_lexeme();
    }

    return build_ast_node_ptr(new RootNode(functions, statements));
}

ASTNodePtr Parser::parse_function_definition() 
{
    
    assert_next_lexeme(LexemeType::IDENT);
    
    std::string name = scanner_.get_lexeme_value();

    assert_next_lexeme(LexemeType::L_PARENTHESIS);
    
    std::vector<std::string> params;
    
    while (scanner_.next_lexeme() != LexemeType::R_PARENTHESIS) 
    {
        if (params.size() > 0) 
        {
            assert_current_lexeme(LexemeType::COMMA);
            scanner_.next_lexeme();
        }

        assert_current_lexeme(LexemeType::IDENT);

        params.push_back(scanner_.get_lexeme_value());
    }

    assert_next_lexeme(LexemeType::COLON); 
    assert_next_lexeme(LexemeType::EOL);

    StatementsSequence statements = parse_statements_sequence();

    return build_ast_node_ptr(
            new FunctionDefinitionNode(
                name, params, statements 
            )
    );
}   

ASTNodePtr Parser::parse_statement() 
{
    LexemeType current_lex = scanner_.current_lexeme();
    
    //assignment or void function call
    if (current_lex == LexemeType::IDENT) 
    {
        std::string ident = scanner_.get_lexeme_value();
        
        if (scanner_.next_lexeme() == LexemeType::ASSIGNMENT) 
        {
            scanner_.next_lexeme();
            return build_ast_node_ptr(
                new AssignmentNode(ident, parse_expression())
            );
        }
        
        //function call with EOL
        return parse_function_call(ident);
    }

    //if/while
    if (current_lex == LexemeType::IF || current_lex == LexemeType::WHILE) 
    {
       return parse_block_statement(current_lex); 
    }

    //io
    if (current_lex == LexemeType::READ || current_lex == LexemeType::PRINT)
    {
        return parse_io_statement(current_lex);
    }

    //return
    if (current_lex == LexemeType::RETURN) 
    {
        current_lex = scanner_.next_lexeme();
        
        return build_ast_node_ptr(
            new ReturnNode(parse_expression()) 
        ); 
    }
    
    throw_error("unexpected statement");

    return nullptr;
}

ASTNodePtr Parser::parse_block_statement(LexemeType type) 
{
    scanner_.next_lexeme(); 
    
    ASTNodePtr expression = parse_expression();
    
    assert_current_lexeme(LexemeType::COLON);
    assert_next_lexeme(LexemeType::EOL);

    StatementsSequence statements = parse_statements_sequence();
    assert_current_lexeme(LexemeType::EOL);
    
    if (type == LexemeType::IF) 
    {
        return build_ast_node_ptr(
                new IfStatementNode(
                    expression,
                    statements
                )
        );
    }

    if (type == LexemeType::WHILE) 
    {
        return build_ast_node_ptr(
                new WhileStatementNode(
                    expression,
                    statements
                )
        );
    }

    throw_error("unknown block operator");

    return nullptr;
}

StatementsSequence Parser::parse_statements_sequence() 
{
    //it's always starts with EOL
    scanner_.next_lexeme();
    
    StatementsSequence result;

    LexemeType current_lex = scanner_.current_lexeme();

    while (current_lex != LexemeType::END) 
    {
        result.push_back(parse_statement());

        assert_current_lexeme(LexemeType::EOL);
        current_lex = scanner_.next_lexeme();
    }
    
    scanner_.next_lexeme();

    return result;
}

ASTNodePtr Parser::parse_io_statement(LexemeType type) 
{
    if (type == LexemeType::READ) 
    {
        assert_next_lexeme(LexemeType::IDENT);
        std::string var_name = scanner_.get_lexeme_value();
        
        scanner_.next_lexeme();

        return build_ast_node_ptr(
            new ReadNode(var_name)
        );
    }

    if (type == LexemeType::PRINT) 
    {
        scanner_.next_lexeme();
        ASTNodePtr expression = parse_expression();
        
        return build_ast_node_ptr(
            new PrintNode(expression)
        );
    }
    
    throw_error("unknown io operation");

    return nullptr;
}

ASTNodePtr Parser::parse_function_call(std::string name)
{
    assert_current_lexeme(LexemeType::L_PARENTHESIS);
    scanner_.next_lexeme();

    std::vector< ASTNodePtr > expressions;

    while (scanner_.current_lexeme() != LexemeType::R_PARENTHESIS) 
    {
        if (expressions.size() > 0) 
        {
            assert_current_lexeme(LexemeType::COMMA);
            scanner_.next_lexeme();
        } 
        
        expressions.push_back(parse_expression());    
    }
    
    scanner_.next_lexeme();
    
    return build_ast_node_ptr(new FunctionCallNode(name, expressions));
}

ASTNodePtr Parser::parse_expression() 
{
    std::stack< LexemeType > operators;
    std::stack< ASTNodePtr > operands;

    auto shift_operations_stacks = [this, &operators, &operands]() mutable 
    {
        ASTNodePtr second = operands.top();
        operands.pop();
        
        ASTNodePtr first = operands.top();
        operands.pop();

        LexemeType op = operators.top();
        operators.pop();

        operands.push(build_binary_operator_node(first, second, op)); 
    };
    
    while (true) 
    {
        LexemeType current_lex = scanner_.current_lexeme();
        
        ASTNodePtr operand = nullptr;

        if (current_lex == LexemeType::MINUS) 
        {
            //unary minus
            scanner_.next_lexeme();
            operand = build_ast_node_ptr(new UnaryMinusNode(parse_expression_operand()));
        }
        else
        {
            operand = parse_expression_operand();
        }

        operands.push(operand); 
         
        current_lex = scanner_.current_lexeme();
        //expect operator or end of expression
        if (!is_operator(current_lex))
        {
            while (!operators.empty()) 
            {
                shift_operations_stacks(); 
            }
            
            return operands.top();
        }

        while (!operators.empty() && operator_priority(operators.top()) >= operator_priority(current_lex))
        {
            shift_operations_stacks();
        }

        operators.push(current_lex);
        scanner_.next_lexeme();
    }
}

ASTNodePtr Parser::build_binary_operator_node(ASTNodePtr a, ASTNodePtr b, LexemeType type) 
{
    BinaryOperatorType bin_op_type;

    switch (type) 
    {
        case LexemeType::MINUS:
            bin_op_type = BinaryOperatorType::MINUS;
            break;
        case LexemeType::PLUS:
            bin_op_type = BinaryOperatorType::PLUS;
            break;
        case LexemeType::MULTIPLY:
            bin_op_type = BinaryOperatorType::MULTIPLY;
            break;
        case LexemeType::DIVIDE:
            bin_op_type = BinaryOperatorType::DIVIDE;
            break;
        case LexemeType::EQUALS:
            bin_op_type = BinaryOperatorType::EQUALS;
            break;
        case LexemeType::NOT_EQUALS:
            bin_op_type = BinaryOperatorType::NOT_EQUALS;
            break;
        case LexemeType::LESS:
            bin_op_type = BinaryOperatorType::LESS;
            break;
        case LexemeType::LESS_OR_EQALS:
            bin_op_type = BinaryOperatorType::LESS_OR_EQALS;
            break;
        case LexemeType::MORE:
            bin_op_type = BinaryOperatorType::MORE;
            break;
        case LexemeType::MORE_OR_EQUALS:
            bin_op_type = BinaryOperatorType::MORE_OR_EQUALS;
            break;
        default:
            throw_error("unknown binary operator");
    }
    
    return build_ast_node_ptr(new BinaryOperatorNode(bin_op_type, a, b));
}

ASTNodePtr Parser::parse_expression_operand()
{
    LexemeType current_lex = scanner_.current_lexeme();
    
    if (current_lex == LexemeType::L_PARENTHESIS) 
    {
        scanner_.next_lexeme();
        auto result = parse_expression();

        assert_current_lexeme(LexemeType::R_PARENTHESIS);
        scanner_.next_lexeme();
        
        return result;
    }

    if (current_lex == LexemeType::IDENT) 
    {
        return parse_expression_identifier();
    }
    
    if (current_lex == LexemeType::LITERAL)
    {
        return parse_literal();
    }
    
    throw_error("unexpected expression operand");

    return nullptr;
}

ASTNodePtr Parser::parse_expression_identifier() 
{
    assert_current_lexeme(LexemeType::IDENT);

    std::string ident = scanner_.get_lexeme_value();
    
    if (scanner_.next_lexeme() == LexemeType::L_PARENTHESIS) 
    {
        //function call
        return parse_function_call(ident);
    } 
    
    //variable identifier
    return build_ast_node_ptr(new VariableNode(ident));
}

ASTNodePtr Parser::parse_literal() 
{
    assert_current_lexeme(LexemeType::LITERAL);
    
    std::stringstream ss;
    ss << scanner_.get_lexeme_value();
    pp_value_t value;
    ss >> value;

    scanner_.next_lexeme();
    return build_ast_node_ptr(new LiteralNode(value));
}

