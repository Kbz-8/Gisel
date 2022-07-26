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

#ifndef __TYPE__
#define __TYPE__

#include <variant>
#include <string>
#include <vector>
#include <set>

namespace Nir
{
	enum class simple_type
	{
		nothing,
		number,
		string
	};

	struct function_type;

	using type = std::variant<simple_type, function_type>;
	using type_handle = const type*;

	struct function_type
	{
		struct param
		{
			type_handle type_id;
			bool by_ref;
		};
		type_handle return_type_id;
		std::vector<param> param_type_id;
	};

	class type_registry
	{
		public:
			type_registry() = default;
			
			type_handle get_handle(const type& t);
			
			inline static type_handle get_void_handle() { return &void_type; }
			inline static type_handle get_number_handle() { return &number_type; }
			inline static type_handle get_string_handle() { return &string_type; }

		private:
			struct types_less { bool operator()(const type& t1, const type& t2) const; };
			std::set<type, types_less> _types;
			
			static type void_type;
			static type number_type;
			static type string_type;
	};
}

namespace std { std::string to_string(Nir::type_handle t); }

#endif // __TYPE__
