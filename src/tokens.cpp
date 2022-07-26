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

#include "tokens.h"

namespace Nir
{
	std::optional<Tokens> get_keyword(const std::string& word)
	{
		if(!Token::kw_tokens.have(word))
			return std::nullopt;
		return std::make_optional(Token::kw_tokens[word]);
	}

	std::optional<Macro_Tokens> get_macro(const std::string& word)
	{
		if(!Token::macros_token.have(word))
			return std::nullopt;
		return std::make_optional(Token::macros_token[word]);
	}

	std::optional<Tokens> get_operator(StreamStack& stream)
	{
		std::string c;
		c.push_back(char(stream()));
		c.push_back(char(stream()));
		if(Token::operators_token.have(c))
			return std::make_optional(Token::operators_token[std::move(c)]);

		stream.push_back(c[1]);
		c.pop_back();
		if(Token::operators_token.have(c))
			return std::make_optional(Token::operators_token[std::move(c)]);

		return std::nullopt;
	}

	Token::Token(token_value value, unsigned int line) : _value(std::move(value)), _line(line) {}
}
