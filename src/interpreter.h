#include <string>
#include <map>
#include <memory>
#include <vector>
#include "pp.h"
#include "ast.h"
#include "ast_visitor.h"
#include "context.h"

#ifndef INTERPRETER_H
#define INTERPRETER_H

class FunctionDefinition
{
    public:
       FunctionDefinition(std::string name, std::vector<std::string> params, StatementsSequence const & statements) :
           name_(name),
           params_(params),
           statements_(statements) {}

       std::string const & get_name() const { return name_; }
       std::vector<std::string> const & get_params() const { return params_; }
       StatementsSequence const & get_statements() const { return statements_; } 

    private:
        std::string name_;
        std::vector<std::string> params_;
        StatementsSequence const & statements_;
};

typedef std::shared_ptr<FunctionDefinition> FunctionDefinitionPtr;

class InterpreterRuntimeException : public LineNumberException
{
    public:
        InterpreterRuntimeException(size_t line, std::string const & msg) :
            LineNumberException(line, ""+msg)
        {}
};

class Interpreter : public ASTNodeVisitor 
{
    public:
        Interpreter() : was_return_(false) {}

        void execute(ASTNodePtr root);

        virtual void visit(RootNode * node) override;
        virtual void visit(FunctionDefinitionNode * node) override;
        virtual void visit(AssignmentNode * node) override;
        virtual void visit(FunctionCallNode * node) override;
        virtual void visit(IfStatementNode * node) override;
        virtual void visit(WhileStatementNode * node) override;
        virtual void visit(PrintNode * node) override;
        virtual void visit(ReadNode * node) override;
        virtual void visit(ReturnNode * node) override;
        virtual void visit(VariableNode * node) override;
        virtual void visit(LiteralNode * node) override;
        virtual void visit(UnaryMinusNode * node) override;
        virtual void visit(BinaryOperatorNode * node) override;

    private:
        void throw_error(std::string const & msg, ASTNode const * in_node)
        {
            throw InterpreterRuntimeException(in_node->get_line_num(), msg);
        }

        void assert_runtime_error(bool condition, std::string const & msg, ASTNode const * in_node) 
        {
            if (!condition) {
                throw_error(msg, in_node);
            }
        }

        pp_value_t value_of(ASTNodePtr node) 
        {
            node->accept(this);
            return last_value_;
        }

        void clear() 
        {
            root_context_.reset();
            contexts_stack_.clear();
            functions_.clear();
        }

        ContextPtr current_context() 
        {
            return contexts_stack_.back(); 
        }

        void execute_sequence(StatementsSequence const & sequence, bool within_function);
        void execute_sequence(StatementsSequence const & sequence) 
        {
            execute_sequence(sequence, false);
        }

        ContextPtr root_context_;
        std::vector<ContextPtr> contexts_stack_;
        std::map<std::string, FunctionDefinitionPtr> functions_;
        
        pp_value_t last_value_;
        bool was_return_;
};

#endif //INTERPRETER_H

