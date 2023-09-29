/**
 * This file is a part of the Gisel Interpreter
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

#ifndef __INCOMPLETE_FUNCTION__
#define __INCOMPLETE_FUNCTION__

#include "tokens.h"
#include "type.h"
#include <deque>
#include "function.h"

namespace Gisel
{
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
			inline const function_declaration& get_decl() const noexcept { return _decl; }
			function compile(compiler_context& ctx);

		private:
			function_declaration _decl;
			std::deque<Token> _tokens;
			size_t _index;
	};
}

#endif // __INCOMPLETE_FUNCTION__
