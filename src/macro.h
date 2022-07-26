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

#ifndef __MACROS__
#define __MACROS__

#include <vector>
#include <unordered_map>
#include <string>

#include "singleton.h"

namespace Nir
{
	class Macros : public Singleton<Macros>
	{
		public:
			inline void new_set(const std::string& source, const std::string& dest) { _sets[source] = dest; }
			inline void remove_set(const std::string& set) { _sets.erase(set); }
			inline std::unordered_map<std::string, std::string>& get_sets() { return _sets; }

		private:
			std::unordered_map<std::string, std::string> _sets;
	};
}

#endif // __MACROS__
