#ifndef __PARSER__
#define __PARSER__

#include <memory>
#include "type.h"

class node;
class tk_iterator;
class compiler_context;

using node_ptr = std::unique_ptr<node>;

node_ptr parse_expression_tree(compiler_context& context, tk_iterator& it, type_handle type_id, bool allow_comma);

#endif // __PARSER__
