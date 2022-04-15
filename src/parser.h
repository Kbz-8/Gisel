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
