#include <memory>
#include <map>
#include <string>
#include "pp.h"

#ifndef CONTEXT_H
#define CONTEXT_H

class Context;

typedef std::shared_ptr<Context> ContextPtr;

class Context 
{
    public:
        Context() : parent_context_(nullptr) {}
        Context(ContextPtr parent_context) :
            parent_context_(parent_context) {}
        
        bool isset_variable(std::string const & name) const 
        {
            if (isset_variable_local(name)) 
            {
                return true;
            }
            
            if (parent_context_ != nullptr) 
            {
                return parent_context_->isset_variable(name);
            }

            return false;
        }
         
        pp_value_t get_var_value(std::string name)
        {
            if (isset_variable_local(name)) 
            {
                return variables_[name];
            }
            
            if (parent_context_ != nullptr) 
            {
                return parent_context_->get_var_value(name);
            }
            
            return 0;
        }

        void set_var_value(std::string name, pp_value_t value) 
        {
            variables_[name] = value;
        }

    private:
        bool isset_variable_local(std::string const & name) const 
        {
            return variables_.count(name) != 0;
        }

        std::map<std::string, pp_value_t> variables_;
        ContextPtr parent_context_;
};

#endif //CONTEXT_H

