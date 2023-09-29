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

#include "warnings.h"
#include <string.h>

namespace Gisel
{
	Warning::Warning(const char* message, size_t line)
	{
		_message = message;
		_line = line + 1;
	}
	Warning::Warning(std::string message, size_t line)
	{
		_message = std::move(message);
		_line = line + 1;
	}

	Warning no_end(const char* message, size_t line)
	{
		std::string dest(strlen(message) + 55, '\0');
		sprintf(&dest[0], "expected closing %s, but none was found", message);
		return Warning(std::move(dest), line);
	}
}
