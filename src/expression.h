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

#ifndef __EXPRESSION__
#define __EXPRESSION__

#include "variable.h"
#include "type.h"

#include <string>

namespace Nir
{
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
	expression<string>::ptr build_string_expression(compiler_context& context, tk_iterator& it);
	expression<lvalue>::ptr build_initialization_expression(compiler_context& context, tk_iterator& it, type_handle type_id, bool allow_comma);
	expression<lvalue>::ptr build_default_initialization(type_handle type_id);
}

#endif // __EXPRESSION__
