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

#include "builtin_functions.h"
#include "errors.h"
#include <gisel_api.h>

#include <iostream>
#include <string>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <cstdarg>

namespace Gisel
{
	void add_string_functions(Module& m)
	{
		m.add_external_function("strlen", func::function<number(const std::string&)>(
			[](const std::string& str)
			{
				return str.length();
			}
		));
		
		m.add_external_function("substr", func::function<std::string(const std::string&, number, number)>(
			[](const std::string& str, number from, number count)
			{
				return str.substr(size_t(from), size_t(count));
			}
		));

		m.add_external_function("to_str", func::function<std::string(number)>(
			[](number num)
			{
				return num == (int)num ? std::to_string((int)num) : std::to_string(num);
			}
		));
		m.add_external_function("to_num", func::function<number(const std::string&)>(
			[](const std::string& str)
			{
				for(char const& c : str)
					runtime_assertion(std::isdigit(c), "trying to convert non digit string to number");
				return std::stod(str);
			}
		));
	}

	void add_io_functions(Module& m)
	{
		m.add_external_function("print", func::function<void(const std::string&)>(
			[](const std::string& val)
			{
				std::cout << val << std::endl;
			}
		));

		m.add_external_function("entry", func::function<std::string()>(
			[]()
			{
				std::string str;
				std::getline(std::cin, str);
				return str;
			}
		));
	}

	void add_standard_functions(Module& m)
	{
		add_string_functions(m);
		add_io_functions(m);
	}
}
