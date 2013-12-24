#include <string>
#include <memory>
#include <vector>
#include "pp.h"


#ifndef AST_H
#define AST_H

class ASTNode;
class ASTNodeVisitor;

typedef std::shared_ptr<ASTNode> ASTNodePtr;
typedef std::vector<ASTNodePtr> StatementsSequence;

class ASTNode {
    public:
        ASTNode(size_t line_num) : line_num_(line_num) {}
        ASTNode() : line_num_(-1) {}

        size_t get_line_num() const { return line_num_; }
        void set_line_num(size_t line_num ) { line_num_ = line_num; }

        virtual void accept(ASTNodeVisitor * visitor) = 0;
    private:
        size_t line_num_;
};

class RootNode : public ASTNode {
    public:
        RootNode(StatementsSequence functions, StatementsSequence statements) :
            ASTNode(),
            functions_(functions),
            statements_(statements)
        {}

        StatementsSequence const & get_functions() const { return functions_; }
        StatementsSequence const & get_statements() const { return statements_; }
       
        virtual void accept(ASTNodeVisitor * visitor) override; 
    private:
        StatementsSequence functions_;
        StatementsSequence statements_;
};

class FunctionDefinitionNode : public ASTNode {
    public:
        FunctionDefinitionNode(std::string name, std::vector<std::string> params, StatementsSequence statements) :
            ASTNode(),
            name_(name),
            params_(params),
            statements_(statements)
        {}

        std::string const & get_name() const { return name_; }
        std::vector<std::string> const & get_params() const { return params_; }
        StatementsSequence const & get_statements() const { return statements_; }
       
        virtual void accept(ASTNodeVisitor * visitor) override; 
    private:
        std::string name_;
        std::vector<std::string> params_;
        StatementsSequence statements_;
};

class AssignmentNode : public ASTNode {
    public:
        AssignmentNode(std::string var_name, ASTNodePtr expr) :
            ASTNode(),
            var_name_(var_name),
            expr_(expr)
        {}

        std::string const & get_var_name() const { return var_name_; }
        ASTNodePtr get_expr() const { return expr_; }
       
        virtual void accept(ASTNodeVisitor * visitor) override; 
    private:
        std::string var_name_;
        ASTNodePtr expr_;
};

class FunctionCallNode : public ASTNode {
    public:
        FunctionCallNode(std::string name, StatementsSequence params) :
            ASTNode(),
            name_(name),
            params_(params)
        {}

        std::string const & get_name() const { return name_; }
        StatementsSequence const & get_params() const { return params_; }
       
        virtual void accept(ASTNodeVisitor * visitor) override; 
    private:
        std::string name_;
        StatementsSequence params_;
};

class IfStatementNode : public ASTNode {
    public:
        IfStatementNode(ASTNodePtr expr, StatementsSequence statements) :
            ASTNode(),
            expr_(expr),
            statements_(statements)
        {}

        ASTNodePtr get_expr() const { return expr_; }
        StatementsSequence const & get_statements() const { return statements_; }
       
        virtual void accept(ASTNodeVisitor * visitor) override; 
    private:
        ASTNodePtr expr_;
        StatementsSequence statements_;
};

class WhileStatementNode : public ASTNode {
    public:
        WhileStatementNode(ASTNodePtr expr, StatementsSequence statements) :
            ASTNode(),
            expr_(expr),
            statements_(statements)
        {}

        ASTNodePtr get_expr() const { return expr_; }
        StatementsSequence const & get_statements() const { return statements_; }
       
        virtual void accept(ASTNodeVisitor * visitor) override; 
    private:
        ASTNodePtr expr_;
        StatementsSequence statements_;
};

class ReadNode : public ASTNode {
    public:
        ReadNode(std::string var_name) :
            ASTNode(),
            var_name_(var_name)
        {}

        std::string const & get_var_name() const { return var_name_; }
       
        virtual void accept(ASTNodeVisitor * visitor) override; 
    private:
        std::string var_name_;
};

class VariableNode : public ASTNode {
    public:
        VariableNode(std::string var_name) :
            ASTNode(),
            var_name_(var_name)
        {}

        std::string const & get_var_name() const { return var_name_; }
       
        virtual void accept(ASTNodeVisitor * visitor) override; 
    private:
        std::string var_name_;
};

class LiteralNode : public ASTNode {
    public:
        LiteralNode(pp_value_t value) :
            ASTNode(),
            value_(value)
        {}

        pp_value_t get_value() const { return value_; }
       
        virtual void accept(ASTNodeVisitor * visitor) override; 
    private:
        pp_value_t value_;
};

class PrintNode : public ASTNode {
    public:
        PrintNode(ASTNodePtr expr) :
            ASTNode(),
            expr_(expr)
        {}

        ASTNodePtr get_expr() const { return expr_; }
       
        virtual void accept(ASTNodeVisitor * visitor) override; 
    private:
        ASTNodePtr expr_;
};

class ReturnNode : public ASTNode {
    public:
        ReturnNode(ASTNodePtr expr) :
            ASTNode(),
            expr_(expr)
        {}

        ASTNodePtr get_expr() const { return expr_; }
       
        virtual void accept(ASTNodeVisitor * visitor) override; 
    private:
        ASTNodePtr expr_;
};

class UnaryMinusNode : public ASTNode {
    public:
        UnaryMinusNode(ASTNodePtr expr) :
            ASTNode(),
            expr_(expr)
        {}

        ASTNodePtr get_expr() const { return expr_; }
       
        virtual void accept(ASTNodeVisitor * visitor) override; 
    private:
        ASTNodePtr expr_;
};

enum class BinaryOperatorType : short int {
    PLUS, MINUS, MULTIPLY, DIVIDE, 
    EQUALS, NOT_EQUALS, MORE, MORE_OR_EQUALS, LESS, LESS_OR_EQALS 
};

class BinaryOperatorNode : public ASTNode {
    public:
        BinaryOperatorNode(BinaryOperatorType type, ASTNodePtr first_expr, ASTNodePtr second_expr) :
            ASTNode(),
            type_(type),
            first_expr_(first_expr),
            second_expr_(second_expr)
        {}

        BinaryOperatorType get_type() const { return type_; }
        ASTNodePtr get_first_expr() const { return first_expr_; }
        ASTNodePtr get_second_expr() const { return second_expr_; }
       
        virtual void accept(ASTNodeVisitor * visitor) override; 
    private:
        BinaryOperatorType type_;
        ASTNodePtr first_expr_;
        ASTNodePtr second_expr_;
};

#endif //AST_H

