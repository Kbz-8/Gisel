#ifndef __INCOMPLETE_FUNCTION__
#define __INCOMPLETE_FUNCTION__

#include "tokens.h"
#include "type.h"
#include <deque>
#include "function.h"

class compiler_context;
class runtime_context;
class tk_iterator;
using function = func::function<void(runtime_context&)>;

struct function_declaration
{
    std::string name;
    type_handle type_id;
    std::vector<std::string> params;
};

function_declaration parse_function_declaration(compiler_context& ctx, tk_iterator& it);

class incomplete_function
{
    public:
        incomplete_function(compiler_context& ctx, tk_iterator& it);
        incomplete_function(incomplete_function&& orig) noexcept;
        inline const function_declaration& get_decl() const { return _decl; }
        function compile(compiler_context& ctx);

    private:
        function_declaration _decl;
        std::deque<Token> _tokens;
        size_t _index;
};

#endif // __INCOMPLETE_FUNCTION__
