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

#ifndef __TOKENS__
#define __TOKENS__

#include <optional>
#include <string>
#include <variant>
#include "streamstack.h"
#include "duets_arrays.h"
#include "utils.h"

namespace Nir
{
	enum class Tokens
	{
		kw_fn,
		kw_import,
		kw_var,
		kw_for,
		kw_while,
		kw_do,
		kw_break,
		kw_continue,
		kw_return,
		kw_public,

		type_void,
		type_number,
		type_string,

		semicolon,
		type_specifier,
		bracket_b,
		bracket_e,
		embrace_b,
		embrace_e,
		comma,
		dot,
		square_b,
		square_e,

		statement_if,
		statement_else,
		statement_elif,

		inc,
		dec,

		add_assign,
		sub_assign,
		mul_assign,
		div_assign,
		mod_assign,
		
		add,
		sub,
		mul,
		div,
		mod,
		assign,

		bitwise_and,

		eq,
		ne,
		lt,
		gt,
		le,
		ge,

		question,
		logical_not,
		logical_and,
		logical_or
	};

	enum class Macro_Tokens
	{
		macro,
		set,
		unset
	};

	struct eof{};
	struct identifier{ std::string name; };

	std::optional<Tokens> get_keyword(const std::string& word);
	std::optional<Tokens> get_operator(StreamStack& stream);
	std::optional<Macro_Tokens> get_macro(const std::string& word);

	inline bool operator==(const identifier& id1, const identifier& id2) { return id1.name == id2.name; }
	inline bool operator!=(const identifier& id1, const identifier& id2) { return id1.name != id2.name; }
	inline constexpr bool operator==(const eof&, const eof&) noexcept { return true; }
	inline constexpr bool operator!=(const eof&, const eof&) noexcept { return false; }

	using token_value = std::variant<Tokens, Macro_Tokens, identifier, double, std::string, eof>;

	class Token
	{
		public:
			Token(token_value value, unsigned int line);

			static inline duets_array<Tokens, std::string> kw_tokens
			{
				{Tokens::kw_fn, "fn"},
				{Tokens::kw_import, "import"},
				{Tokens::kw_public, "export"},
				{Tokens::kw_var, "var"},
				{Tokens::kw_for, "for"},
				{Tokens::kw_while, "while"},
				{Tokens::kw_do, "do"},
				{Tokens::kw_break, "break"},
				{Tokens::kw_continue, "continue"},
				{Tokens::kw_return, "return"},

				{Tokens::type_void, "void"},
				{Tokens::type_number, "num"},
				{Tokens::type_string, "str"},

				{Tokens::statement_if, "if"},
				{Tokens::statement_else, "else"},
				{Tokens::statement_elif, "elif"}
			};

			static inline duets_array<Tokens, std::string> operators_token
			{
				{Tokens::semicolon, ";"},
				{Tokens::comma, ","},
				{Tokens::dot, "."},
				{Tokens::type_specifier, ":"},
				{Tokens::bitwise_and, "&"},
				{Tokens::bracket_b, "("},
				{Tokens::bracket_e, ")"},
				{Tokens::embrace_b, "{"},
				{Tokens::embrace_e, "}"},
				{Tokens::square_b, "["},
				{Tokens::square_e, "]"},

				{Tokens::eq, "=="},
				{Tokens::ne, "!="},
				{Tokens::lt, "<"},
				{Tokens::gt, ">"},
				{Tokens::le, "<="},
				{Tokens::ge, ">="},

				{Tokens::question, "?"},
				{Tokens::logical_not, "!"},
				{Tokens::logical_and, "&&"},
				{Tokens::logical_or, "||"},

				{Tokens::inc, "++"},
				{Tokens::dec, "--"},
				{Tokens::add_assign, "+="},
				{Tokens::sub_assign, "-="},
				{Tokens::mul_assign, "*="},
				{Tokens::div_assign, "/="},
				{Tokens::mod_assign, "%="},

				{Tokens::add, "+"},
				{Tokens::sub, "-"},
				{Tokens::mul, "*"},
				{Tokens::div, "/"},
				{Tokens::mod, "%"},
				{Tokens::assign, "="}
			};

			static inline duets_array<Macro_Tokens, std::string> macros_token
			{
				{Macro_Tokens::macro, "@"},

				{Macro_Tokens::set, "set"},
				{Macro_Tokens::unset, "unset"}
			};

			inline bool is_keyword() const { return std::holds_alternative<Tokens>(_value); }
			inline bool is_number() const { return std::holds_alternative<double>(_value); }
			inline bool is_identifier() const { return std::holds_alternative<identifier>(_value); }
			inline bool is_eof() const { return std::holds_alternative<eof>(_value); }
			inline bool is_macro() const { return std::holds_alternative<Macro_Tokens>(_value); }
			inline bool is_string() const { return std::holds_alternative<std::string>(_value); }

			inline Tokens get_token() const { return std::get<Tokens>(_value); }
			inline const identifier& get_identifier() const { return std::get<identifier>(_value); }
			inline const std::string& get_string() const { return std::get<std::string>(_value); }
			inline Macro_Tokens get_macro() const { return std::get<Macro_Tokens>(_value); }
			inline double get_number() const { return std::get<double>(_value); }
			inline const token_value& get_value() const { return _value; }
		
			inline size_t get_line_number() const noexcept { return _line; }

			bool has_value(token_value value) const { return _value == std::move(value); }

		private:
			token_value _value;
			unsigned int _line = 0;
	};
}

namespace std
{
	inline std::string to_string(Nir::Tokens t) { return Nir::Token::kw_tokens.have(t) ? std::move(std::string(Nir::Token::kw_tokens[t])) : std::move(std::string(Nir::Token::operators_token[t])); }
	
	inline std::string to_string(const Nir::token_value& t)
    {
		return std::visit(Nir::overloaded
        {
			[](Nir::Tokens rt) { return to_string(rt); },
            [](Nir::Macro_Tokens mt) { return Nir::Token::macros_token[mt]; },
			[](double d) { return to_string(d); },
			[](const std::string& str) { return str; },
			[](const Nir::identifier& id) { return id.name; },
			[](Nir::eof) { return std::string("<EOF>"); }
		}, t);
	}
}

#endif // __TOKENS__
