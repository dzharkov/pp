#include <cctype>
#include "lexer.h"

std::vector< std::pair<std::string, LexemeType> > Lexer::sorted_key_tokens_;

void Lexer::init_key_tokens() 
{
    sorted_key_tokens_ = std::vector< std::pair<std::string, LexemeType> >{
            std::make_pair("=", LexemeType::ASSIGNMENT),
            std::make_pair("+", LexemeType::PLUS),
            std::make_pair("-", LexemeType::MINUS),
            std::make_pair("*", LexemeType::MULTIPLY),
            std::make_pair("/", LexemeType::DIVIDE),
            std::make_pair("==", LexemeType::EQUALS),
            std::make_pair("!=", LexemeType::NOT_EQUALS),
            std::make_pair(">", LexemeType::MORE),
            std::make_pair(">=", LexemeType::MORE_OR_EQUALS),
            std::make_pair("<", LexemeType::LESS),
            std::make_pair("<=", LexemeType::LESS_OR_EQALS),
            std::make_pair("(", LexemeType::L_PARENTHESIS),
            std::make_pair(")", LexemeType::R_PARENTHESIS),
            std::make_pair("def", LexemeType::DEF),
            std::make_pair("return", LexemeType::RETURN),
            std::make_pair("end", LexemeType::END),
            std::make_pair("if", LexemeType::IF),
            std::make_pair("while", LexemeType::WHILE),
            std::make_pair(":", LexemeType::COLON),
            std::make_pair(",", LexemeType::COMMA),
            std::make_pair("read", LexemeType::READ),
            std::make_pair("print", LexemeType::PRINT)
        };

    std::sort(sorted_key_tokens_.begin(), sorted_key_tokens_.end(), std::greater<std::pair<std::string, LexemeType>>());
}

void Lexer::read_next_chunk() 
{
    current_chunk_ = "";
    was_new_line_ = false;

    while (!(chunks_stream_ >> current_chunk_) || current_chunk_[0] == comment_char) 
    {
        if (chunks_stream_.str().length() != 0) 
        {
            was_new_line_ = true;
        }
        
        if (!read_line()) 
        {
            break;
        }
    }

    chunk_position_ = 0;
}

bool Lexer::read_line() 
{
    std::string current_line;
    
    if (std::getline(inp_stream_, current_line)) 
    {
        line_num_++;

        chunks_stream_.str(current_line);
        chunks_stream_.clear();

        return true;
    } 
    else 
    {
        eof_ = true;
        return false;
    }
}

/*
 * Collects chars into lexeme_value until the end of chunk or matcher(c) returns false 
 * e.g. when matcher is isgidit it collects literal value
 */
void Lexer::greedy_match(char_matcher matcher, LexemeType type) 
{
    lexeme_value_ = "";
    current_lexeme_ = type;

    for (; chunk_position_ < current_chunk_.length() && matcher(current_chunk_[chunk_position_]); chunk_position_++)
    {
        lexeme_value_ += current_chunk_[chunk_position_];
    }
}

inline int isident(int c) 
{
    return isalnum(c) || c == '_';
}

/*
 * returns false when need repeating
 */
bool Lexer::try_match_token() 
{
    if (was_new_line_) 
    {
        was_new_line_ = false;
        if (current_lexeme_ != LexemeType::UNDEFINED) 
        {
            current_lexeme_ = LexemeType::EOL;
            return true;
        }
    }

    if (eof()) 
    {
        current_lexeme_ = LexemeType::EOFL;
        return true;
    }
     
    if (chunk_position_ >= current_chunk_.length()) 
    {
        read_next_chunk();
        return false;
    }
    
    char current_char = current_chunk_[chunk_position_];
    
    if (sorted_key_tokens_[0].first == ">=") {
        return false;
    }

    for (const std::pair<std::string, LexemeType> & key_token : sorted_key_tokens_) 
    {
        std::string key = key_token.first;
        if ((chunk_position_ + key.length() - 1) < current_chunk_.length()) 
        {
            if (current_chunk_.substr(chunk_position_, key.length()) == key) 
            {
                current_lexeme_ = key_token.second;
                lexeme_value_ = key;

                chunk_position_ += key.length();
                return true;
            }
        }
    }

    if (isdigit(current_char)) 
    {
        greedy_match(isdigit, LexemeType::LITERAL);
        return true;
    }
    
    if (isident(current_char) && !isdigit(current_char)) 
    {
        greedy_match(isident, LexemeType::IDENT);
        return true;
    }
    
    //unexpected character
    throw SyntaxErrorException(line_num_, "unexpected char");
}

LexemeType Lexer::next_lexeme() 
{
    while (!try_match_token());
    
    return current_lexeme();
}

