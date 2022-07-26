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

#include "streamstack.h"

namespace Nir
{
	StreamStack::StreamStack(const func::function<int()>* input) : _input(*input), _line(0) {}
		
	int StreamStack::operator()()
	{
		int ret = -1;

		if(_stack.empty())
			ret = _input();
		else
		{
			ret = _stack.top();
			_stack.pop();
		}

		if(ret == '\n')
			_line++;
		
		return ret;
	}

	void StreamStack::push_back(int c) 
	{
		_stack.push(c);
		
		if(c == '\n')
			_line--;
	}
}
