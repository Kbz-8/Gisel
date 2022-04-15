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

#ifndef __STATEMENT__
#define __STATEMENT__

#include <memory>
#include <vector>
#include <unordered_map>
#include "expression.h"

enum struct flow_type
{
    f_normal,
    f_break,
    f_continue,
    f_return,
};

class flow
{
    public:
        inline flow_type type() const noexcept { return _type; }
        inline int break_level() const noexcept { return _break_level; }
        static inline flow normal_flow() noexcept { return flow(flow_type::f_normal, 0); }
        static inline flow break_flow(int break_level) noexcept { return flow(flow_type::f_break, break_level); }
        static inline flow continue_flow() noexcept { return flow(flow_type::f_continue, 0); }
        static inline flow return_flow() noexcept { return flow(flow_type::f_return, 0); }
        inline flow consume_break() noexcept { return _break_level == 1 ? flow::normal_flow() : flow::break_flow(_break_level - 1); }
        
    private:
        flow_type _type;
        int _break_level;
        flow(flow_type type, int break_level);
};

class statement
{
    public:
        virtual flow execute(class runtime_context& context) = 0;
        virtual ~statement() = default;
    
    protected:
        statement() = default;

    private:
        statement(const statement&) = delete;
        void operator=(const statement&) = delete;
};

using statement_ptr = std::unique_ptr<statement>;
using shared_statement_ptr = std::shared_ptr<statement>;

statement_ptr create_simple_statement(expression<void>::ptr expr);
statement_ptr create_local_declaration_statement(std::vector<expression<lvalue>::ptr> decls);
statement_ptr create_block_statement(std::vector<statement_ptr> statements);
shared_statement_ptr create_shared_block_statement(std::vector<statement_ptr> statements);
statement_ptr create_break_statement(int break_level);
statement_ptr create_continue_statement();
statement_ptr create_return_statement(expression<lvalue>::ptr expr);
statement_ptr create_return_void_statement();
statement_ptr create_if_statement(std::vector<expression<lvalue>::ptr> decls, std::vector<expression<number>::ptr> exprs, std::vector<statement_ptr> statements);
statement_ptr create_switch_statement(std::vector<expression<lvalue>::ptr> decls, expression<number>::ptr expr, std::vector<statement_ptr> statements, std::unordered_map<number, size_t> cases, size_t dflt);
statement_ptr create_while_statement(expression<number>::ptr expr, statement_ptr statement);
statement_ptr create_do_statement(expression<number>::ptr expr, statement_ptr statement);
statement_ptr create_for_statement(expression<void>::ptr expr1, expression<number>::ptr expr2, expression<void>::ptr expr3, statement_ptr statement);
statement_ptr create_for_statement(std::vector<expression<lvalue>::ptr> decls, expression<number>::ptr expr2, expression<void>::ptr expr3, statement_ptr statement);
statement_ptr create_import_statement(expression<string>::ptr expr);

#endif // __STATEMENT__
