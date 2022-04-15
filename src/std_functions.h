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

#ifndef __STD_FUNCTIONS__
#define __STD_FUNCTIONS__

#include <cstdint>

class module;
	
void add_math_functions(module& m);
void add_string_functions(module& m);
void add_io_functions(module& m, uint8_t opt);

void add_standard_functions(module& m, uint8_t opt);

#endif // __STD_FUNCTIONS__
