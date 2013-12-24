#include <iostream>
#include <fstream>
#include "parser.h"
#include "ast.h"
#include "lexer.h"
#include "interpreter.h"

void display_usage() 
{
    std::cout << "Usage: pp <source-file>" << std::endl;
}

int main(int argc, char const * argv[]) 
{
    if (argc < 2) 
    {
        display_usage();
        return 1;
    }

    std::ifstream src_fstream(argv[1], std::ios::in);
    
    Lexer lexer(src_fstream);
    Parser parser(lexer);
    
    try 
    {
        ASTNodePtr root = parser.parse();
        Interpreter interpreter;
        interpreter.execute(root);
    } 
    catch (LineNumberException &e) 
    {
        std::cerr << "line number " << e.get_line_number() << ": " << e.what() << std::endl;
        return 1;
    }
    return 0;
}

