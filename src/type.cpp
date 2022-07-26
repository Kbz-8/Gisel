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

#include "type.h"
#include "utils.h"

namespace Nir
{
	bool type_registry::types_less::operator()(const type& t1, const type& t2) const
	{
		const size_t idx1 = t1.index();
		const size_t idx2 = t2.index();
		
		if(idx1 != idx2)
			return idx1 < idx2;
		
		switch(idx1)
		{
			case 0: return std::get<0>(t1) < std::get<0>(t2);
			case 1:
			{
				const function_type& ft1 = std::get<1>(t1);
				const function_type& ft2 = std::get<1>(t2);
				
				if(ft1.return_type_id != ft2.return_type_id)
					return ft1.return_type_id < ft2.return_type_id;
				
				if(ft1.param_type_id.size() != ft2.param_type_id.size())
					return ft1.param_type_id.size() < ft2.param_type_id.size();
				
				for(size_t i = 0; i < ft1.param_type_id.size(); ++i)
				{
					if(ft1.param_type_id[i].type_id != ft2.param_type_id[i].type_id)
						return ft1.param_type_id[i].type_id < ft2.param_type_id[i].type_id;
					if(ft1.param_type_id[i].by_ref != ft2.param_type_id[i].by_ref)
						return ft2.param_type_id[i].by_ref;
				}
			}
		}
		
		return false;
	}

	type_handle type_registry::get_handle(const type& t)
	{
		return std::visit(overloaded{
			[](simple_type t)
			{
				switch(t)
				{
					case simple_type::nothing: return type_registry::get_void_handle();
					case simple_type::number:  return type_registry::get_number_handle();
					case simple_type::string:  return type_registry::get_string_handle();
				}
			},
			[this](const auto& t) { return &(*(_types.insert(t).first)); }
		}, t);
	}

	type type_registry::void_type = simple_type::nothing;
	type type_registry::number_type = simple_type::number;
	type type_registry::string_type = simple_type::string;
}

namespace std
{
	std::string to_string(Nir::type_handle t)
    {
		return std::visit(Nir::overloaded{
			[](Nir::simple_type st)
            {
				switch(st)
                {
					case Nir::simple_type::nothing: return std::string("void");
					case Nir::simple_type::number:  return std::string("number");
					case Nir::simple_type::string: return std::string("string");
				}
			},
			[](const Nir::function_type& ft)
            {
				std::string ret = to_string(ft.return_type_id) + "(";
				const char* separator = "";
				for(const Nir::function_type::param& p: ft.param_type_id)
                {
					ret += separator + std::string(p.by_ref ? "var&" : "var") + "arg : " + to_string(p.type_id);
					separator = ",";
				}
				ret += ")";
				return ret;
			}
		}, *t);
	}
}
