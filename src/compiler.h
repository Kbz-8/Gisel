#ifndef __COMPILER__
#define __COMPILER__

#include "type.h"
#include "tokens.h"
#include "statement.h"

#include <vector>
#include "function.h"

class compiler_context;
class tk_iterator;
class runtime_context;

using function = func::function<void(runtime_context&)>;

runtime_context compile(tk_iterator& it, const std::vector<std::pair<std::string, function> >& external_functions, std::vector<std::string> public_declarations);
type_handle parse_type(compiler_context& ctx, tk_iterator& it);
std::string parse_declaration_name(compiler_context& ctx, tk_iterator& it);
void parse_token_value(compiler_context& ctx, tk_iterator& it, const token_value& value);
shared_statement_ptr compile_function_block(compiler_context& ctx, tk_iterator& it, type_handle return_type_id);

#endif // __COMPILER__
