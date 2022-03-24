#ifndef __EXPRESSION__
#define __EXPRESSION__

#include "variable.h"
#include "type.h"

#include <string>

class runtime_context;
class tk_iterator;
class compiler_context;

template <typename R>
class expression
{
    public:
        using ptr = std::unique_ptr<const expression>;
    
        virtual R evaluate(runtime_context& context) const = 0;
        virtual ~expression() = default;
    
    protected:
        expression() = default;
    
    private:
        expression(const expression&) = delete;
        void operator=(const expression&) = delete;
};

expression<void>::ptr build_void_expression(compiler_context& context, tk_iterator& it);
expression<number>::ptr build_number_expression(compiler_context& context, tk_iterator& it);
expression<lvalue>::ptr build_initialization_expression(compiler_context& context, tk_iterator& it, type_handle type_id, bool allow_comma);
expression<lvalue>::ptr build_default_initialization(type_handle type_id);

#endif // __EXPRESSION__
