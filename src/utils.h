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

#ifndef __UTILS__
#define __UTILS__

namespace Gisel
{
	#define OUT_RED "\033[1;31m"
	#define OUT_GREEN "\033[1;32m"
	#define OUT_BLUE "\033[1;34m"
	#define OUT_DEF "\033[1;39m"
	#define OUT_BLACK "\033[1;30m"
	#define OUT_YEL "\033[1;33m"
	#define OUT_MAGENTA "\033[1;35m"
	#define OUT_CYAN "\033[1;36m"
	#define OUT_WHITE "\033[1;37m"
	#define OUT_BG_RED "\033[1;41m"
	#define OUT_BG_GREEN "\033[1;42m"
	#define OUT_BG_BLUE "\033[1;44m"
	#define OUT_BG_DEF "\033[1;49m"
	#define OUT_BG_BLACK "\033[1;40m"
	#define OUT_BG_YEL "\033[1;43m"
	#define OUT_BG_MAGENTA "\033[1;45m"
	#define OUT_BG_CYAN "\033[1;46m"
	#define OUT_BG_WHITE "\033[1;47m"
	#define OUT_RESET "\033[1;0m"
	#define OUT_BOLD "\033[1;1m"
	#define OUT_UNDERLINE "\033[1;4m"
	#define OUT_INVERSE "\033[1;7m"
	#define OUT_BOLD_OFF "\033[1;21m"
	#define OUT_UNDERLINE_OFF "\033[1;24m"
	#define OUT_INVERSE_OFF "\033[1;27m"

	template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
	template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;
}

#endif // __UTILS__
