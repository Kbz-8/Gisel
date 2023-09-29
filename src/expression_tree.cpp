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

#include "expression_tree.h"
#include "utils.h"

#include <filesystem>

namespace Gisel
{
    bool is_convertible(type_handle type_from, bool lvalue_from, type_handle type_to, bool lvalue_to)
    {
        if(type_to == type_registry::get_void_handle())
            return true;
        if(lvalue_to)
            return lvalue_from && type_from == type_to;
        if(type_from == type_to)
            return true;
        return type_from == type_registry::get_number_handle() && type_to == type_registry::get_string_handle();
    }

    bool is_file(const char* file)
    {
        std::filesystem::path f(file);
        return std::filesystem::exists(f) ? true : false;
    }

	void node::check_conversion(type_handle type_id, bool lvalue) const
	{
		if(!is_convertible(_type_id, _lvalue, type_id, lvalue))
			wrong_type_error(std::to_string(_type_id).c_str(), std::to_string(type_id).c_str(), lvalue, _line_number).expose();
	}

	void node::check_file(const char* file) const
	{
		if(!is_file(file))
			file_not_found(file, _line_number).expose();
	}

	node::node(compiler_context& context, node_value value, std::vector<node_ptr> children, size_t line_number) : _value(std::move(value)), _children(std::move(children)), _line_number(line_number)
	{
		const type_handle void_handle = type_registry::get_void_handle();
		const type_handle number_handle = type_registry::get_number_handle();
		const type_handle string_handle = type_registry::get_string_handle();
		
		std::visit(overloaded
		{
			[&](const std::string& value)
			{
				_type_id = string_handle;
				_lvalue = false;
			},
			[&](double value)
			{
				_type_id = number_handle;
				_lvalue = false;
			},
			[&](const identifier& value)
			{
				if(const identifier_info* info = context.find(value.name))
				{
					_type_id = info->type_id();
					_lvalue = (info->get_scope() != identifier_scope::function);
				}
				else
					undeclared_error(value.name.c_str(), _line_number).expose();
			},
			[&](node_operation value)
			{
				switch(value)
				{
					case node_operation::param:
						_type_id = _children[0]->_type_id;
						_lvalue = false;
					break;
					case node_operation::preinc:
					case node_operation::predec:
						_type_id = number_handle;
						_lvalue = true;
						_children[0]->check_conversion(number_handle, true);
					break;
					case node_operation::postinc:
					case node_operation::postdec:
						_type_id = number_handle;
						_lvalue = false;
						_children[0]->check_conversion(number_handle, true);
					break;
					case node_operation::positive:
					case node_operation::negative:
					case node_operation::bnot:
					case node_operation::lnot:
						_type_id = number_handle;
						_lvalue = false;
						_children[0]->check_conversion(number_handle, false);
					break;
					case node_operation::add:
					case node_operation::sub:
					case node_operation::mul:
					case node_operation::div:
					case node_operation::mod:
					case node_operation::band:
					case node_operation::land:
					case node_operation::lor:
						_type_id = number_handle;
						_lvalue = false;
						_children[0]->check_conversion(number_handle, false);
						_children[1]->check_conversion(number_handle, false);
					break;
					case node_operation::eq:
					case node_operation::ne:
					case node_operation::lt:
					case node_operation::gt:
					case node_operation::le:
					case node_operation::ge:
						_type_id = number_handle;
						_lvalue = false;
						if(!_children[0]->is_number() || !_children[1]->is_number())
						{
							_children[0]->check_conversion(string_handle, false);
							_children[1]->check_conversion(string_handle, false);
						}
						else
						{
							_children[0]->check_conversion(number_handle, false);
							_children[1]->check_conversion(number_handle, false);
						}
					break;
					case node_operation::assign:
						_type_id = _children[0]->get_type_id();
						_lvalue = true;
						_children[0]->check_conversion(_type_id, true);
						_children[1]->check_conversion(_type_id, false);
					break;
					case node_operation::add_assign:
					case node_operation::sub_assign:
					case node_operation::mul_assign:
					case node_operation::div_assign:
					case node_operation::mod_assign:
						_type_id = number_handle;
						_lvalue = true;
						_children[0]->check_conversion(number_handle, true);
						_children[1]->check_conversion(number_handle, false);
					break;
					case node_operation::comma:
						for(int i = 0; i < int(_children.size()) - 1; ++i)
							_children[i]->check_conversion(void_handle, false);
						_type_id = _children.back()->get_type_id();
						_lvalue = _children.back()->is_lvalue();
					break;
					case node_operation::ternary:
						_children[0]->check_conversion(number_handle, false);
						if(is_convertible(_children[2]->get_type_id(), _children[2]->is_lvalue(), _children[1]->get_type_id(), _children[1]->is_lvalue()))
						{
							_children[2]->check_conversion(_children[1]->get_type_id(), _children[1]->is_lvalue());
							_type_id = _children[1]->get_type_id();
							_lvalue = _children[1]->is_lvalue();
						}
						else
						{
							_children[1]->check_conversion(_children[2]->get_type_id(), _children[2]->is_lvalue());
							_type_id = _children[2]->get_type_id();
							_lvalue = _children[2]->is_lvalue();
						}
					break;
					case node_operation::call:
						if(const function_type* ft = std::get_if<function_type>(_children[0]->get_type_id()))
						{
							_type_id = ft->return_type_id;
							_lvalue = false;
							if(ft->param_type_id.size() + 1 != _children.size())
								semantic_error(std::string("wrong number of arguments. Expected " + std::to_string(ft->param_type_id.size()) + ", given " + std::to_string(_children.size() - 1)).c_str(), _line_number).expose();
							for(size_t i = 0; i < ft->param_type_id.size(); ++i)
							{
								if(_children[i+1]->is_lvalue() && !ft->param_type_id[i].by_ref)
									semantic_error("reference passed to a function that did not expect one", _children[i + 1]->get_line_number()).expose();
								_children[i+1]->check_conversion(ft->param_type_id[i].type_id, ft->param_type_id[i].by_ref);
							}
						}
						else
						{
							if(_children[0]->is_identifier())
								semantic_error(std::string(std::string(_children[0]->get_identifier()) + " (" + to_string(_children[0]->_type_id) + ") is not callable").c_str(), _line_number).expose();
							semantic_error(std::string(to_string(_children[0]->_type_id) + " is not callable").c_str(), _line_number).expose();
						}
					break;
					case node_operation::import:
						std::cout << "test" << std::endl;
						_children[1]->check_conversion(string_handle, false);
						std::cout << "test2" << std::endl;
						_lvalue = false;
						_children[1]->check_file(_children[1]->get_string().data());
					break;
				}
			}
		}, _value);
	}
}
