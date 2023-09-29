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

#include "lexer.h"
#include "errors.h"
#include "warnings.h"
#include "macro.h"

namespace Gisel
{
	enum class char_type
	{
		eof,
		space,
		alphanum,
		punct,
		macro
	};

	char_type get_char_type(int c)
	{
		if(c < 0)
			return char_type::eof;
		if(std::isspace(c))
			return char_type::space;
		if(std::isalpha(c) || std::isdigit(c) || char(c) == '_' || char(c) == '"')
			return char_type::alphanum;
		if(c == (int)Token::macros_token[Macro_Tokens::macro][0])
			return char_type::macro;
		return char_type::punct;
	}

	Token fetch_word(StreamStack& stream)
	{
		size_t line = stream.getline();

		std::string word;
		
		int c = stream();
		
		bool is_string = (bool)(c == '"');

		if(is_string)
		{
			do
			{
				c = stream();
				if(c == '\n')
					unexpected_error("\\n", stream.getline()).expose();
				if(c == '\\')
				{
					switch(c = stream())
					{
						case '\\': c = '\\'; break;
						case 'n': c = '\n'; break;
						case 't': c = '\t'; break;
						case 'r': c = '\r'; break;
						case '"': word.push_back('"'); c = stream(); break;

						default: break;
					}
				}
				if(c == '{')
				{
					std::string macro;
					do
					{
						c = stream();
						macro.push_back(c);
						if(c == '\n')
							unexpected_error("\\n", stream.getline()).expose();
					} while(c != '}' && c != '"');
					macro.pop_back();

					if(Macros::get().get_sets().count(macro) && c != '"')
						word += Macros::get().get_sets()[macro];
					else
					{
						word.push_back('{');
						word += macro;
						word.push_back(c);
					}
					continue;
				}
				word.push_back(char(c));
			} while(c != '"');
			word.pop_back();
			return Token(std::move(word), line);
		}

		bool is_number = std::isdigit(c);

		do
		{
			word.push_back(char(c));
			c = stream();
			
			if(c == '.' && word.back() == '.')
			{
				stream.push_back(word.back());
				word.pop_back();
				break;
			}
		} while(get_char_type(c) == char_type::alphanum || (is_number && c == '.'));

		if(Macros::get().get_sets().count(word))
			word = Macros::get().get_sets()[word];
		
		stream.push_back(c);
		
		if(std::optional<Tokens> t = get_keyword(word))
			return Token(*t, line);
		else
		{
			if(std::isdigit(word.front()))
			{
				char* endptr = nullptr;
				double num = strtol(word.c_str(), &endptr, 0);
				if(*endptr != 0)
				{
					num = strtod(word.c_str(), &endptr);
					if(*endptr != 0)
						unexpected_error(endptr, stream.getline()).expose();
				}
				return Token(num, line);
			}
			return Token(identifier{std::move(word)}, line);
		}
	}

	void fetch_macro(StreamStack& stream)
	{
		size_t line = stream.getline();
		std::vector<std::string> identifiers;
		identifiers.push_back("");
		std::vector<Token> cache;
		int c = stream();
		std::optional<Macro_Tokens> t;

		do
		{
			identifiers.back().push_back(char(c));
			c = stream();
			if(get_char_type(c) == char_type::space)
			{
				c = stream();
				if(t = get_macro(identifiers.back()))
					cache.push_back(Token(*t, line));
				else if(cache.empty())
					unexpected_macro_error(identifiers.back().c_str(), line).expose();
				identifiers.push_back("");
			}
		} while(c != '\n' && get_char_type(c) != char_type::eof && get_char_type(c) != char_type::macro);
		
		if(c != '\n')
			stream.push_back(c);

		switch(cache.front().get_macro())
		{
			case Macro_Tokens::set : Macros::get().new_set(identifiers[1], identifiers[2]); break;
			case Macro_Tokens::unset : Macros::get().remove_set(identifiers[1]); break;

			default : break;
		}
	}

	Token fetch_operator(StreamStack& stream)
	{
		size_t line = stream.getline();

		if(std::optional<Tokens> t = get_operator(stream))
			return Token(*t, line);
		
		std::string unexpected;
		unsigned int err_line_number = stream.getline();
		for(int c = stream(); get_char_type(c) == char_type::punct; c = stream())
			unexpected.push_back(char(c));
		unexpected_error(unexpected.c_str(), err_line_number).expose();
	}

	void skip_line_comment(StreamStack& stream)
	{
		int c = 0;
		do
		{
			c = stream();
		} while(c != '\n' && get_char_type(c) != char_type::eof);
		
		if(c != '\n')
			stream.push_back(c);
	}

	void skip_block_comment(StreamStack& stream)
	{
		bool closing = false;
		int c = 0;
		int line = stream.getline();
		do
		{
			c = stream();
			if(closing && c == '/')
				return;
			closing = (c == '*');
		} while(get_char_type(c) != char_type::eof);

		stream.push_back(c);
		no_end("'*/'", line).expose();
	}

	Token lexe(StreamStack& stream)
	{
		while(true)
		{
			size_t line = stream.getline();
			int c = stream();

			switch(get_char_type(c))
			{
				case char_type::space: continue;
				case char_type::eof:  return {eof(), line};
				case char_type::alphanum: stream.push_back(c); return fetch_word(stream);
				case char_type::macro: fetch_macro(stream); continue;
				case char_type::punct:
				{
					switch(c)
					{
						case '/':
						{
							char c1 = stream();
							switch(c1)
							{
								case '/': skip_line_comment(stream); continue;
								case '*': skip_block_comment(stream); continue;
								
								default: stream.push_back(c1);
							}
						}

						default: stream.push_back(c); return fetch_operator(stream);
					}
				}
				break;
			}
		}
	}
}
