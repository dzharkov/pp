#include "pp.h"
#include "ast.h"

#ifndef AST_VISITOR_H
#define AST_VISITOR_H

class ASTNodeVisitor 
{
    public:
        virtual void visit(RootNode * node) = 0;
        virtual void visit(FunctionDefinitionNode * node) = 0;
        virtual void visit(AssignmentNode * node) = 0;
        virtual void visit(FunctionCallNode * node) = 0;
        virtual void visit(IfStatementNode * node) = 0;
        virtual void visit(WhileStatementNode * node) = 0;
        virtual void visit(PrintNode * node) = 0;
        virtual void visit(ReadNode * node) = 0;
        virtual void visit(ReturnNode * node) = 0;
        virtual void visit(VariableNode * node) = 0;
        virtual void visit(LiteralNode * node) = 0;
        virtual void visit(UnaryMinusNode * node) = 0;
        virtual void visit(BinaryOperatorNode * node) = 0;
};

#endif //AST_VISITOR_H

