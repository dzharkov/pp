#include "ast.h"
#include "ast_visitor.h"

void RootNode::accept(ASTNodeVisitor * visitor)
{
    visitor->visit(this);
}

void FunctionDefinitionNode::accept(ASTNodeVisitor * visitor)
{
    visitor->visit(this);
}

void AssignmentNode::accept(ASTNodeVisitor * visitor)
{
    visitor->visit(this);
}

void FunctionCallNode::accept(ASTNodeVisitor * visitor)
{
    visitor->visit(this);
}

void IfStatementNode::accept(ASTNodeVisitor * visitor)
{
    visitor->visit(this);
}

void WhileStatementNode::accept(ASTNodeVisitor * visitor)
{
    visitor->visit(this);
}

void ReadNode::accept(ASTNodeVisitor * visitor)
{
    visitor->visit(this);
}

void VariableNode::accept(ASTNodeVisitor * visitor)
{
    visitor->visit(this);
}

void LiteralNode::accept(ASTNodeVisitor * visitor)
{
    visitor->visit(this);
}

void PrintNode::accept(ASTNodeVisitor * visitor)
{
    visitor->visit(this);
}

void ReturnNode::accept(ASTNodeVisitor * visitor)
{
    visitor->visit(this);
}

void UnaryMinusNode::accept(ASTNodeVisitor * visitor)
{
    visitor->visit(this);
}

void BinaryOperatorNode::accept(ASTNodeVisitor * visitor)
{
    visitor->visit(this);
}

