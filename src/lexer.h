#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include <utility>
#include <algorithm>
#include "pp.h"
#include "parser.h"

#ifndef LEXER_H
#define LEXER_H

const size_t chars_num = 256;
const char comment_char = '#';


class Lexer : public IScanner 
{
    public:
        explicit
        Lexer(std::istream &is) : inp_stream_(is), 
            current_lexeme_(LexemeType::UNDEFINED),
            eof_(false),
            was_new_line_(false),
            line_num_(0)
        { 
            if (sorted_key_tokens_.size() == 0) {
                init_key_tokens();
            }
        }

        virtual ~Lexer() {}

        virtual LexemeType current_lexeme() const override { return current_lexeme_; }
        virtual std::string get_lexeme_value() const override { return lexeme_value_; }
        virtual size_t get_current_line_number() const override 
        { 
            return line_num_ - (current_lexeme_ == LexemeType::EOL); 
        } 
        virtual LexemeType next_lexeme() override;

        Lexer &operator=(Lexer const &a) = delete;
        Lexer(Lexer const &a) = delete;
    private:

        bool eof() const { return eof_; }
        bool read_line();

        typedef int (*char_matcher)(int);
        void read_next_chunk();
        bool try_match_token();
        void greedy_match(char_matcher matcher, LexemeType type);
        
        std::string current_chunk_;
        size_t chunk_position_;

        std::istream & inp_stream_;
        std::stringstream chunks_stream_;

        LexemeType current_lexeme_;
        std::string lexeme_value_;
        bool eof_;
        bool was_new_line_;
        size_t line_num_;
        
        static void init_key_tokens();
        static std::vector< std::pair<std::string, LexemeType> > sorted_key_tokens_; 
};

#endif

