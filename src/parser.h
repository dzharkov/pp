#include <string>
#include <memory>
#include <sstream>

#include "pp.h"
#include "ast.h"

#ifndef PARSER_H
#define PARSER_H

enum class LexemeType : short int 
{
    IDENT, //0
    LITERAL, //1
    ASSIGNMENT, //2
    PLUS, MINUS, MULTIPLY, DIVIDE, //3
    EQUALS, NOT_EQUALS, MORE, MORE_OR_EQUALS, LESS, LESS_OR_EQALS, //7
    L_PARENTHESIS, R_PARENTHESIS, //13
    DEF, RETURN, END, //15
    IF, WHILE, COLON, COMMA, //18
    READ, PRINT, //22
    EOFL, EOL, //24
    UNDEFINED //26
};

inline std::ostream& operator << (std::ostream& os, const LexemeType& obj)
{
    os << (short int)obj;
    return os;
}

class IScanner 
{
    public:
        virtual LexemeType current_lexeme() const = 0;
        virtual std::string get_lexeme_value() const = 0;
        virtual size_t get_current_line_number() const = 0;
        virtual LexemeType next_lexeme() = 0;
};

class SyntaxErrorException : public LineNumberException {
    public:
        SyntaxErrorException(size_t line, std::string const & msg) : 
            LineNumberException(line, "syntax error: " + msg) 
        {
        }
};

class Parser {
    public: 
        Parser(IScanner & scanner) : scanner_(scanner) {}
        
        ASTNodePtr parse();

    private:
        ASTNodePtr         parse_function_definition();
        ASTNodePtr         parse_statement();
        ASTNodePtr         parse_block_statement(LexemeType type);
        ASTNodePtr         parse_io_statement(LexemeType type);
        StatementsSequence parse_statements_sequence();
        ASTNodePtr         parse_function_call(std::string name);
        ASTNodePtr         parse_expression();
        ASTNodePtr         parse_expression_operand();
        ASTNodePtr         parse_expression_identifier();
        ASTNodePtr         parse_literal();
        ASTNodePtr         build_binary_operator_node(ASTNodePtr a, ASTNodePtr b, LexemeType type);
        
        ASTNodePtr build_ast_node_ptr(ASTNode * node) const 
        {
            node->set_line_num(scanner_.get_current_line_number());
            return ASTNodePtr(node);
        }

        void throw_error(std::string const & msg) { throw SyntaxErrorException(scanner_.get_current_line_number(), msg); }
        void assert_syntax_error(bool condition, std::string const & msg) 
        { 
            if (!condition) {
                throw_error(msg);
            }
        }
        
        void assert_current_lexeme(LexemeType type) 
        {
            std::ostringstream os;
            os << type;
            std::string msg = "expected type: " + os.str();
            assert_syntax_error(scanner_.current_lexeme() == type, msg);
        }

        void assert_next_lexeme(LexemeType type) 
        {
            scanner_.next_lexeme();
            assert_current_lexeme(type);
        }

        IScanner & scanner_;
        
        static unsigned short int operator_priority(LexemeType type);
        static bool is_operator(LexemeType type);

        static std::vector<std::vector<LexemeType>> operators_;
};

#endif //PARSER_H

