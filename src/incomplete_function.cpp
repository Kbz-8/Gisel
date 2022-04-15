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

#include "incomplete_function.h"
#include "compiler.h"
#include "compiler_context.h"
#include "errors.h"
#include "lexer.h"
#include "tk_iterator.h"
#include "runtime_context.h"

function_declaration parse_function_declaration(compiler_context& ctx, tk_iterator& it)
{
    function_declaration ret;
    
    parse_token_value(ctx, it, Tokens::kw_fn);
    
    function_type ft;
    ret.name = parse_declaration_name(ctx, it);
    
    {
        auto _ = ctx.function();
        
        parse_token_value(ctx, it, Tokens::bracket_b);
        
        while(!it->has_value(Tokens::bracket_e))
        {
            if(!ret.params.empty())
                parse_token_value(ctx, it, Tokens::comma);
            
            parse_token_value(ctx, it, Tokens::kw_var);
            bool byref = false;
            if(it->has_value(Tokens::bitwise_and))
            {
                byref = true;
                ++it;
            }
            
            ret.params.push_back(parse_declaration_name(ctx, it));

            parse_token_value(ctx, it, Tokens::type_specifier);

            type_handle t = parse_type(ctx, it);
            ft.param_type_id.push_back({t, byref});

            if(!it->has_value(Tokens::bracket_e) && !it->has_value(Tokens::comma))
                ret.params.push_back("@" + std::to_string(ret.params.size()));
        }
        ++it;
    }

    if(it->has_value(Tokens::sub))
    {
        parse_token_value(ctx, it, Tokens::sub);
        parse_token_value(ctx, it, Tokens::gt);
        ft.return_type_id = parse_type(ctx, it);
    }
    else
        ft.return_type_id = ctx.get_handle(simple_type::nothing);
    
    ret.type_id = ctx.get_handle(ft);
    
    return ret;
}

incomplete_function::incomplete_function(compiler_context& ctx, tk_iterator& it)
{
    _decl = parse_function_declaration(ctx, it);
    
    _tokens.push_back(*it);
    
    parse_token_value(ctx, it, Tokens::embrace_b);
    
    int nesting = 1;
    
    while(nesting && !it->is_eof())
    {
        if(it->has_value(Tokens::embrace_b))
            ++nesting;
        
        if(it->has_value(Tokens::embrace_e))
            --nesting;
        
        _tokens.push_back(*it);
        ++it;
    }
    
    if(nesting)
        unexpected_syntax_error("end of file", it->get_line_number()).expose();
    
    ctx.create_function(_decl.name, _decl.type_id);
}

incomplete_function::incomplete_function(incomplete_function&& orig) noexcept : _tokens(std::move(orig._tokens)), _decl(std::move(orig._decl)) {}

function incomplete_function::compile(compiler_context& ctx)
{
    auto _ = ctx.function();
    const function_type* ft = std::get_if<function_type>(_decl.type_id);
    for(int i = 0; i < int(_decl.params.size()); ++i)
        ctx.create_param(std::move(_decl.params[i]), ft->param_type_id[i].type_id);
    tk_iterator it(_tokens);
    shared_statement_ptr stmt = compile_function_block(ctx, it, ft->return_type_id);
    return [stmt=std::move(stmt)] (runtime_context& ctx) { stmt->execute(ctx); };
}
