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

#ifndef __EXPRESSION_TREE__
#define __EXPRESSION_TREE__

#include <memory>
#include <variant>
#include <vector>
#include "tokens.h"
#include "type.h"
#include "compiler_context.h"
#include "errors.h"

namespace Gisel
{
	enum class node_operation
	{
		param,

		preinc,
		predec,
		postinc,
		postdec,
		positive,
		negative,
		bnot,
		lnot,    

		add,
		sub,
		mul,
		div,
		mod,
		assign,
		add_assign,
		sub_assign,
		mul_assign,
		div_assign,
		mod_assign,
		band,
		eq,
		ne,
		lt,
		gt,
		le,
		ge,
		comma,
		land,
		lor,
		
		ternary,

		call,

		import
	};

	struct node;
	using node_ptr = std::unique_ptr<node>;

	using node_value = std::variant<node_operation, std::string, double, identifier>;

	struct node
	{
		node(compiler_context& context, node_value value, std::vector<node_ptr> children, size_t line_number);
		
		inline const node_value& get_value() const { return _value; }

		inline bool is_node_operation() const { return std::holds_alternative<node_operation>(_value); }
		inline bool is_identifier() const { return std::holds_alternative<identifier>(_value); }
		inline bool is_number() const { return std::holds_alternative<double>(_value); }
		inline bool is_string() const { return std::holds_alternative<std::string>(_value); }

		inline node_operation get_node_operation() const { return std::get<node_operation>(_value); }
		inline std::string_view get_identifier() const { return std::get<identifier>(_value).name; }
		inline double get_number() const { return std::get<double>(_value); }
		inline std::string_view get_string() const { return std::get<std::string>(_value); }
		inline const std::vector<node_ptr>& get_children() const { return _children; }
		inline type_handle get_type_id() const { return _type_id; }

		inline bool is_lvalue() const { return _lvalue; }

		inline size_t get_line_number() const { return _line_number; }
		
		void check_conversion(type_handle type_id, bool lvalue) const;
		void check_file(const char* file) const;

		private:
			node_value _value;
			std::vector<node_ptr> _children;
			type_handle _type_id;
			bool _lvalue;
			size_t _line_number;
	};
}

#endif // __EXPRESSION_TREE__
