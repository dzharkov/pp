#include <iostream>
#include "interpreter.h"

void Interpreter::execute(ASTNodePtr root)
{
    root_context_ = ContextPtr(new Context());
    contexts_stack_.push_back(root_context_);

    root->accept(this);
    clear();
}

void Interpreter::visit(RootNode * node)
{
    for (const ASTNodePtr & function : node->get_functions())
    {
        function->accept(this);
    }

    for (const ASTNodePtr & statement : node->get_statements()) 
    {
        statement->accept(this);
    }
}

void Interpreter::visit(FunctionDefinitionNode * node)
{
    FunctionDefinitionPtr function(new FunctionDefinition(node->get_name(), node->get_params(), node->get_statements()));

    functions_[node->get_name()] = function;
}

void Interpreter::visit(AssignmentNode * node)
{
    std::string var_name = node->get_var_name();
    current_context()->set_var_value(var_name, value_of(node->get_expr()));
}

void Interpreter::visit(FunctionCallNode * node)
{
    std::string func_name = node->get_name();

    assert_runtime_error(
        functions_.count(func_name), 
        "undefined function " + func_name, 
        node
    );

    FunctionDefinition function = *(functions_[func_name]);
    assert_runtime_error(
        function.get_params().size() == node->get_params().size(), 
        "arguments number mismatch for " + func_name, 
        node
    );
    
    ContextPtr func_context(new Context(root_context_));    
    
    for (size_t i = 0; i < function.get_params().size(); i++) 
    {
        func_context->set_var_value(
            function.get_params()[i],
            value_of(node->get_params()[i])
        );
    }
    
    contexts_stack_.push_back(func_context);
    
    execute_sequence(function.get_statements(), true);
    contexts_stack_.pop_back();
}

void Interpreter::execute_sequence(StatementsSequence const & statements, bool within_function) 
{
    for (const ASTNodePtr & statement : statements)
    {
        statement->accept(this);
        if (was_return_) 
        {
           if (within_function) {
               //return handled
               was_return_ = false;
           }

           return;
        }
    }
}

void Interpreter::visit(IfStatementNode * node)
{
    if (value_of(node->get_expr()) > 0)
    {
        execute_sequence(node->get_statements());
    }
}

void Interpreter::visit(WhileStatementNode * node)
{
    while (value_of(node->get_expr()) > 0)
    {
        execute_sequence(node->get_statements());
        if (was_return_) 
        {
            return;
        }
    }
}

void Interpreter::visit(ReadNode * node)
{
    pp_value_t value;
    std::cin >> value;

    current_context()->set_var_value(node->get_var_name(), value);
}

void Interpreter::visit(PrintNode * node)
{
    std::cout << value_of(node->get_expr()) << std::endl;
}

void Interpreter::visit(ReturnNode * node)
{
    last_value_ = value_of(node->get_expr());
    was_return_ = true;
}

void Interpreter::visit(VariableNode * node)
{
    std::string var_name = node->get_var_name();
    assert_runtime_error(
        current_context()->isset_variable(var_name), 
        "undefined variable " + var_name, 
        node
    );

    last_value_ = current_context()->get_var_value(var_name);
}

void Interpreter::visit(LiteralNode * node)
{
    last_value_ = node->get_value();
}

void Interpreter::visit(UnaryMinusNode * node)
{
    last_value_ = -value_of(node->get_expr());
}

void Interpreter::visit(BinaryOperatorNode * node)
{
    pp_value_t first_operand  = value_of(node->get_first_expr());
    pp_value_t second_operand = value_of(node->get_second_expr());


    switch (node->get_type()) 
    {
        case BinaryOperatorType::PLUS:
            last_value_ = first_operand + second_operand;
            break;
        case BinaryOperatorType::MINUS:
            last_value_ = first_operand - second_operand;
            break;
        case BinaryOperatorType::MULTIPLY:
            last_value_ = first_operand * second_operand;
            break;
        case BinaryOperatorType::DIVIDE:
            assert_runtime_error(
                second_operand != 0, 
                "division by zero", 
                node
            );

            last_value_ = first_operand / second_operand;
            break;
        case BinaryOperatorType::EQUALS:
            last_value_ = first_operand == second_operand;
            break;
        case BinaryOperatorType::NOT_EQUALS:
            last_value_ = first_operand != second_operand;
            break;
        case BinaryOperatorType::LESS:
            last_value_ = first_operand < second_operand;
            break;
        case BinaryOperatorType::LESS_OR_EQALS:
            last_value_ = first_operand <= second_operand;
            break;
        case BinaryOperatorType::MORE:
            last_value_ = first_operand > second_operand;
            break;
        case BinaryOperatorType::MORE_OR_EQUALS:
            last_value_ = first_operand >= second_operand;
            break;
        default:
            throw_error("unknown operator type", node);
    }
}

