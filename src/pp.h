#include <iostream>
#include <exception>
#include <string>

#ifndef PP_H
#define PP_H

typedef int pp_value_t;

#ifdef DEBUG
#define dbg(a) std::cerr << a << std::endl;
#else
#define dbg(a)
#endif //DEBUG

class LineNumberException : public std::exception {
    public:
        LineNumberException(size_t line, std::string msg) : 
            line_(line), 
            msg_(msg)
        {
            
        }
        virtual const char* what() const throw()
        {
            return msg_.c_str();
        }
        size_t get_line_number() const { return line_; }
    private:
        size_t line_;
        std::string msg_;
};

#endif //PP_H

