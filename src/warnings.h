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

#ifndef __WARNINGS__
#define __WARNINGS__

#include <string>
#include <iostream>
#include "utils.h"

namespace Gisel
{
	class Warning
	{
		public:
			Warning(const char* message, size_t line);
			Warning(std::string message, size_t line);

			inline const char* what() const noexcept { return _message.c_str(); }
			inline void expose() const noexcept { std::cout << OUT_MAGENTA << "Gisel warning : " << OUT_DEF << _message << ", line : " << _line << std::endl; }
			inline int line() const noexcept { return _line; }
		
		private:
			std::string _message;
			int _line = 0;
	};

	Warning no_end(const char* message, size_t line);
}

#endif // __WARNINGS__
