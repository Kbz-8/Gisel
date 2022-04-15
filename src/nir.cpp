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

#include "nir.h"
#include <cctype>

int main(int argc, char** argv)
{
	if(argc < 2)
		Error("no input file given", -2).expose();
	
	std::array<uint8_t, 2> opts;
	opts.fill(0);
	if(argc > 2)
	{
		for(int i = 2; i < argc; i++)
		{
			if(argv[i][0] == '-')
			{
				switch(argv[i][1])
				{
					case 'O':
					case 'o':
					{
						if(!isdigit(argv[i][2]))
							Error("option '-o' needs to be followed by a digit", -2).expose();
						opts[0] = (int)argv[i][2] - 48;
						if(opts[0] > 1)
							Error("option '-o' to high (needs to be '-o0' or '-o1')", -2).expose();
						break;
					}

					case 'D':
					case 'd':
						opts[1] = 1;
						if(opts[1] > 1)
							Error("option '-d' cannot have index", -2).expose();
						break;

					default : Error(std::string("unknown option '" + std::string(argv[i]) + "'").c_str(), -2).expose();
				}
			}
			else
				Error("too much args", -2).expose();
		}
	}

	std::ios::sync_with_stdio(false);
	
	module m;
	
	add_standard_functions(m, opts[0]);
	
	auto nir_main = m.create_external_function_caller<void>("main");

	m.load(argv[1]);
	if(opts[1] == 1)
		Message("code compiled successfully").expose();
	
	nir_main();

    return 0;
}
