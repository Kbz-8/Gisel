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

#include <vector>
#include <cstdio>
#include <nir_api.h>
#include "errors.h"
#include "streamstack.h"
#include "lexer.h"
#include "compiler.h"
#include "file.h"
#include "tk_iterator.h"

namespace Nir
{
	class Module_impl
	{
		public:
			Module_impl() {}
			
			inline runtime_context* get_runtime_context() { return _context.get(); }
			
			inline void add_public_function_declaration(std::string declaration, std::string name, std::shared_ptr<function> fptr)
			{
				_public_declarations.push_back(std::move(declaration));
				_public_functions.emplace(std::move(name), std::move(fptr));
			}
			
			inline void add_external_function_impl(std::string declaration, function f) { _external_functions.emplace_back(std::move(declaration), std::move(f)); }
			
			inline void load(const char* path)
			{
				File f(path);
				get_character get = [&](){ return f(); };
				StreamStack stream(&get);
				
				tk_iterator it(stream);
				
				_context = std::make_unique<runtime_context>(compile(it, _external_functions, _public_declarations));
				
				for(const auto& p : _public_functions)
					*p.second = _context->get_public_function(p.first.c_str());
			}
			
			inline void reset_globals() { if(_context) _context->initialize(); }

		private:
			std::vector<std::pair<std::string, function> > _external_functions;
			std::vector<std::string> _public_declarations;
			std::unordered_map<std::string, std::shared_ptr<function> > _public_functions;
			std::unique_ptr<runtime_context> _context;
	};

	Module::Module() : _impl(std::make_unique<Module_impl>()) {}

	runtime_context* Module::get_runtime_context() { return _impl->get_runtime_context(); }
	void Module::add_external_function_impl(std::string declaration, function f) { _impl->add_external_function_impl(std::move(declaration), std::move(f)); }
	void Module::add_public_function_declaration(std::string declaration, std::string name, std::shared_ptr<function> fptr) { _impl->add_public_function_declaration(std::move(declaration), std::move(name), std::move(fptr)); }
	void Module::load(const char* path) { _impl->load(path); }
	void Module::reset_globals() { _impl->reset_globals(); }

	Module::~Module() {}
}
