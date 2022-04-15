/**
 * This file is a part of the Nir Interpreter
 *
 * Copyright (C) 2022 @kbz_8
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

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
