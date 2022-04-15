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

#ifndef __RUNTIME_CONTEXT__
#define __RUNTIME_CONTEXT__

#include <variant>
#include <vector>
#include <deque>
#include <stack>
#include <string>
#include "variable.h"
#include "expression.h"

class runtime_context
{
    class scope
    {
        public:
            scope(runtime_context& context);
            ~scope();

        private:
            runtime_context& _context;
            size_t _stack_size;
    };

    public:
        runtime_context(std::vector<expression<lvalue>::ptr> initializers, std::vector<function> functions, std::unordered_map<std::string, size_t> public_functions);
        
        void initialize();
        variable_ptr& global(int idx);
		variable_ptr& retval();
		variable_ptr& local(int idx);

		const function& get_function(int idx) const;
		const function& get_public_function(const char* name) const;

		scope enter_scope();
		void push(variable_ptr v);
		
		variable_ptr call(const function& f, std::vector<variable_ptr> params);

    private:
        std::vector<function> _functions;
		std::unordered_map<std::string, size_t> _public_functions;
		std::vector<expression<lvalue>::ptr> _initializers;
		std::vector<variable_ptr> _globals;
		std::deque<variable_ptr> _stack;
		size_t _retval_idx;
};

#endif // __RUNTIME_CONTEXT__
