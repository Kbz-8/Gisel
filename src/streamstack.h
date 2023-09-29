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

#ifndef __STREAM_STACK__
#define __STREAM_STACK__

#include <vector>
#include <string>
#include <stack>
#include "function.h"

namespace Gisel
{
	using get_character = func::function<int()>;

	class StreamStack
	{
		public:
			explicit StreamStack(const func::function<int()>* input);
			
			int operator()();
			
			void push_back(int c);
			
			inline size_t getline() const noexcept { return _line; }

			~StreamStack() = default;

		private:
			const func::function<int()>& _input;
			std::stack<int> _stack;
			size_t _line;
	};
}

#endif // __STREAM_STACK__
