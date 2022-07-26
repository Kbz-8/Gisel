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

#include "tk_iterator.h"
#include "lexer.h"

namespace Nir
{
	tk_iterator::tk_iterator(StreamStack& stream) : _current(eof(), 0), _get_next_token([&stream](){ return lexe(stream); }) { ++(*this); }

	tk_iterator::tk_iterator(std::deque<Token>& tokens) : _current(eof(), 0),
		_get_next_token([&tokens]()
		{
			if(tokens.empty())
				return Token(eof(), 0);
			Token ret = std::move(tokens.front());
			tokens.pop_front();
			return ret;
		})
	{ ++(*this); }
}
