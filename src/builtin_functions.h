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

#ifndef __STD_FUNCTIONS__
#define __STD_FUNCTIONS__

#include <cstdint>

namespace Gisel
{
	class Module;
		
	void add_string_functions(Module& m);
	void add_io_functions(Module& m);

	void add_standard_functions(Module& m);
}

#endif // __STD_FUNCTIONS__
