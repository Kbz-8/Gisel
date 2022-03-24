#include "nir.h"

namespace
{
    struct possible_flow
    {
        size_t break_level;
        bool can_continue;
        type_handle return_type_id;
        
        inline possible_flow add_loop() { return possible_flow{break_level+1, true, return_type_id}; }
        inline static possible_flow in_function(type_handle return_type_id) { return possible_flow{0, false, return_type_id}; }
    };

    bool is_typename(const compiler_context&, const tk_iterator& it)
    {
        return std::visit(overloaded
        {
            [](Tokens t)
            {
                switch(t)
                {
                    case Tokens::type_number:
                    case Tokens::type_string:
                    case Tokens::type_void:
                    default: return false;
                }
            },
            [](const token_value&) { return false; }
        }, it->get_value());
    }

    inline Error unexpected_syntax(const tk_iterator& it) { return unexpected_syntax_error(std::to_string(it->get_value()).c_str(), it->get_line_number()); }
    
    std::vector<expression<lvalue>::ptr> compile_variable_declaration(compiler_context& ctx, tk_iterator& it)
    {
        parse_token_value(ctx, it, Tokens::kw_var);
        std::string name = parse_declaration_name(ctx, it);

        type_handle type_id = nullptr;
        if(it->has_value(Tokens::type_specifier))
        {
            parse_token_value(ctx, it, Tokens::type_specifier);
            type_id = parse_type(ctx, it);

            if(type_id == type_registry::get_void_handle())
                syntax_error("cannot declare void variable", it->get_line_number()).expose();
        }
    
        std::vector<expression<lvalue>::ptr> ret;
        
        if(it->has_value(Tokens::assign))
        {
            ++it;
            ret.emplace_back(build_initialization_expression(ctx, it, type_id, false));
        }
        else
            ret.emplace_back(build_default_initialization(type_id));
        
        ctx.create_identifier(std::move(name), type_id);
        
        return ret;
    }
    
    statement_ptr compile_simple_statement(compiler_context& ctx, tk_iterator& it);
    statement_ptr compile_block_statement(compiler_context& ctx, tk_iterator& it, possible_flow pf);
    statement_ptr compile_for_statement(compiler_context& ctx, tk_iterator& it, possible_flow pf);
    statement_ptr compile_while_statement(compiler_context& ctx, tk_iterator& it, possible_flow pf);
    statement_ptr compile_do_statement(compiler_context& ctx, tk_iterator& it, possible_flow pf);
    statement_ptr compile_if_statement(compiler_context& ctx, tk_iterator& it, possible_flow pf);
    statement_ptr compile_switch_statement(compiler_context& ctx, tk_iterator& it, possible_flow pf);
    statement_ptr compile_var_statement(compiler_context& ctx, tk_iterator& it);
    statement_ptr compile_break_statement(compiler_context& ctx, tk_iterator& it, possible_flow pf);
    statement_ptr compile_continue_statement(compiler_context& ctx, tk_iterator& it, possible_flow pf);
    statement_ptr compile_return_statement(compiler_context& ctx, tk_iterator& it, possible_flow pf);
    
    statement_ptr compile_statement(compiler_context& ctx, tk_iterator& it, possible_flow pf, bool in_switch)
    {
        if(it->is_keyword())
        {
            switch(it->get_token())
            {
                case Tokens::kw_var:       return compile_var_statement(ctx, it);
                case Tokens::kw_for:       return compile_for_statement(ctx, it, pf.add_loop());
                case Tokens::kw_while:     return compile_while_statement(ctx, it, pf.add_loop());
                case Tokens::kw_do:        return compile_do_statement(ctx, it, pf.add_loop());
                case Tokens::statement_if: return compile_if_statement(ctx, it, pf);
                case Tokens::kw_break:     return compile_break_statement(ctx, it, pf);
                case Tokens::kw_continue:  return compile_continue_statement(ctx, it, pf);
                case Tokens::kw_return:    return compile_return_statement(ctx, it, pf);
                
                default: break;
            }
        }
        if(it->has_value(Tokens::embrace_b))
            return compile_block_statement(ctx, it, pf);
        return compile_simple_statement(ctx, it);
    }
    
    statement_ptr compile_simple_statement(compiler_context& ctx, tk_iterator& it)
    {
        statement_ptr ret = create_simple_statement(build_void_expression(ctx, it));
        parse_token_value(ctx, it, Tokens::semicolon);
        return ret;
    }
    
    statement_ptr compile_for_statement(compiler_context& ctx, tk_iterator& it, possible_flow pf)
    {
        auto _ = ctx.scope();
    
        parse_token_value(ctx, it, Tokens::kw_for);
        parse_token_value(ctx, it, Tokens::bracket_b);
        
        std::vector<expression<lvalue>::ptr> decls;
        expression<void>::ptr expr1;
        
        if(it->has_value(Tokens::kw_var))
            decls = compile_variable_declaration(ctx, it);
        else
            expr1 = build_void_expression(ctx, it);
    
        parse_token_value(ctx, it, Tokens::semicolon);
        
        expression<number>::ptr expr2 = build_number_expression(ctx, it);

        parse_token_value(ctx, it, Tokens::semicolon);
        
        expression<void>::ptr expr3 = build_void_expression(ctx, it);

        parse_token_value(ctx, it, Tokens::bracket_e);
        
        statement_ptr block = compile_block_statement(ctx, it, pf);
        
        if(!decls.empty())
            return create_for_statement(std::move(decls), std::move(expr2), std::move(expr3), std::move(block));
        return create_for_statement(std::move(expr1), std::move(expr2), std::move(expr3), std::move(block));
    }
    
    statement_ptr compile_while_statement(compiler_context& ctx, tk_iterator& it, possible_flow pf)
    {
        parse_token_value(ctx, it, Tokens::kw_while);

        parse_token_value(ctx, it, Tokens::bracket_b);
        expression<number>::ptr expr = build_number_expression(ctx, it);
        parse_token_value(ctx, it, Tokens::bracket_e);
        
        statement_ptr block = compile_block_statement(ctx, it, pf);
        
        return create_while_statement(std::move(expr), std::move(block));
    }
    
    statement_ptr compile_do_statement(compiler_context& ctx, tk_iterator& it, possible_flow pf)
    {
        parse_token_value(ctx, it, Tokens::kw_do);
        
        statement_ptr block = compile_block_statement(ctx, it, pf);
        
        parse_token_value(ctx, it, Tokens::kw_while);
        
        parse_token_value(ctx, it, Tokens::bracket_b);
        expression<number>::ptr expr = build_number_expression(ctx, it);
        parse_token_value(ctx, it, Tokens::bracket_e);
        
        return create_do_statement(std::move(expr), std::move(block));
    }
    
    statement_ptr compile_if_statement(compiler_context& ctx, tk_iterator& it, possible_flow pf)
    {
        auto _ = ctx.scope();
        parse_token_value(ctx, it, Tokens::statement_if);
        
        parse_token_value(ctx, it, Tokens::bracket_b);
        
        std::vector<expression<lvalue>::ptr> decls;
        
        if(is_typename(ctx, it))
        {
            decls = compile_variable_declaration(ctx, it);
            parse_token_value(ctx, it, Tokens::semicolon);
        }
        
        std::vector<expression<number>::ptr> exprs;
        std::vector<statement_ptr> stmts;
        
        exprs.emplace_back(build_number_expression(ctx, it));
        parse_token_value(ctx, it, Tokens::bracket_e);
        stmts.emplace_back(compile_block_statement(ctx, it, pf));
        
        while(it->has_value(Tokens::statement_elif))
        {
            ++it;
            parse_token_value(ctx, it, Tokens::bracket_b);
            exprs.emplace_back(build_number_expression(ctx, it));
            parse_token_value(ctx, it, Tokens::bracket_e);
            stmts.emplace_back(compile_block_statement(ctx, it, pf));
        }
        
        if(it->has_value(Tokens::statement_else))
        {
            ++it;
            stmts.emplace_back(compile_block_statement(ctx, it, pf));
        } 
        else
            stmts.emplace_back(create_block_statement({}));
        
        return create_if_statement(std::move(decls), std::move(exprs), std::move(stmts));
    }

    statement_ptr compile_var_statement(compiler_context& ctx, tk_iterator& it)
    {
        std::vector<expression<lvalue>::ptr> decls = compile_variable_declaration(ctx, it);
        parse_token_value(ctx, it, Tokens::semicolon);
        return create_local_declaration_statement(std::move(decls));
    }
    
    statement_ptr compile_break_statement(compiler_context& ctx, tk_iterator& it, possible_flow pf)
    {
        if(pf.break_level == 0)
            unexpected_syntax(it).expose();
        
        parse_token_value(ctx, it, Tokens::kw_break);
        
        double break_level;
        
        if(it->is_number())
        {
            break_level = it->get_number();
            if(break_level < 1 || break_level != int(break_level) || break_level > pf.break_level)
                syntax_error("invalid break value", it->get_line_number()).expose();
            ++it;
        }
        else
            break_level = 1;
        
        parse_token_value(ctx, it, Tokens::semicolon);
        
        return create_break_statement(int(break_level));
    }
    
    statement_ptr compile_continue_statement(compiler_context& ctx, tk_iterator& it, possible_flow pf)
    {
        if(!pf.can_continue)
            unexpected_syntax(it).expose();
        parse_token_value(ctx, it, Tokens::kw_continue);
        parse_token_value(ctx, it, Tokens::semicolon);
        return create_continue_statement();
    }
    
    statement_ptr compile_return_statement(compiler_context& ctx, tk_iterator& it, possible_flow pf)
    {
        parse_token_value(ctx, it, Tokens::kw_return);
        
        if(pf.return_type_id == type_registry::get_void_handle())
        {
            parse_token_value(ctx, it, Tokens::semicolon);
            return create_return_void_statement();
        }
        expression<lvalue>::ptr expr = build_initialization_expression(ctx, it, pf.return_type_id, true);
        parse_token_value(ctx, it, Tokens::semicolon);
        return create_return_statement(std::move(expr));
    }
    
    
    std::vector<statement_ptr> compile_block_contents(compiler_context& ctx, tk_iterator& it, possible_flow pf)
    {
        std::vector<statement_ptr> ret;
        
        if(it->has_value(Tokens::embrace_b))
        {
            parse_token_value(ctx, it, Tokens::embrace_b);
            
            while(!it->has_value(Tokens::embrace_e))
                ret.push_back(compile_statement(ctx, it, pf, false));
            
            parse_token_value(ctx, it, Tokens::embrace_e);
        }
        else
            ret.push_back(compile_statement(ctx, it, pf, false));
        
        return ret;
    }
    
    statement_ptr compile_block_statement(compiler_context& ctx, tk_iterator& it, possible_flow pf)
    {
        auto _ = ctx.scope();
        std::vector<statement_ptr> block = compile_block_contents(ctx, it, pf);
        return create_block_statement(std::move(block));
    }
}

void parse_token_value(compiler_context&, tk_iterator& it, const token_value& value)
{
    if(it->has_value(value))
    {
        ++it;
        return;
    }
    expected_syntax_error(std::to_string(value).c_str(), it->get_line_number()).expose();
}

std::string parse_declaration_name(compiler_context& ctx, tk_iterator& it)
{
    if(!it->is_identifier())
        unexpected_syntax(it).expose();

    std::string ret = it->get_identifier().name;
    
    if(!ctx.can_declare(ret))
        already_declared_error(ret.c_str(), it->get_line_number()).expose();

    ++it;
    
    return ret;
}

type_handle parse_type(compiler_context& ctx, tk_iterator& it)
{
    if(!it->is_keyword())
        unexpected_syntax(it).expose();
    
    type_handle t = nullptr;
    
    switch(it->get_token())
    {
        case Tokens::type_void:
            t = ctx.get_handle(simple_type::nothing);
            ++it;
            break;
        case Tokens::type_number:
            t = ctx.get_handle(simple_type::number);
            ++it;
            break;
        case Tokens::type_string:
            t = ctx.get_handle(simple_type::string);
            ++it;
            break;
        default: unexpected_syntax(it).expose();
    }
    
    while(it->is_keyword())
    {
        switch(it->get_token())
        {
            case Tokens::bracket_b:
            {
                function_type ft;
                ft.return_type_id = t;
                ++it;
                while(!it->has_value(Tokens::bracket_e))
                {
                    if(!ft.param_type_id.empty())
                        parse_token_value(ctx, it, Tokens::comma);
                    type_handle param_type = parse_type(ctx, it);
                    if(it->has_value(Tokens::bitwise_and))
                    {
                        ft.param_type_id.push_back({param_type, true});
                        ++it;
                    }
                    else
                        ft.param_type_id.push_back({param_type, false});
                }
                ++it;
                t = ctx.get_handle(ft);
                break;
            }
            default: return t;
        }
    }
    return t;
}

shared_statement_ptr compile_function_block(compiler_context& ctx, tk_iterator& it, type_handle return_type_id)
{
    std::vector<statement_ptr> block = compile_block_contents(ctx, it, possible_flow::in_function(return_type_id));
    if(return_type_id != type_registry::get_void_handle())
        block.emplace_back(create_return_statement(build_default_initialization(return_type_id)));
    return create_shared_block_statement(std::move(block));
}

runtime_context compile(tk_iterator& it, const std::vector<std::pair<std::string, function>>& external_functions, std::vector<std::string> public_declarations)
{
    compiler_context ctx;
    
    for(const std::pair<std::string, function>& p : external_functions)
    {
        get_character get = [i = 0, &p]() mutable { return i < p.first.size() ? int(p.first[i++]) : -1; };
        StreamStack stream(&get);
        tk_iterator function_it(stream);
        function_declaration decl = parse_function_declaration(ctx, function_it);
        ctx.create_function(decl.name, decl.type_id);
    }
    
    std::unordered_map<std::string, type_handle> public_function_types;
    
    for(const std::string& f : public_declarations)
    {
        get_character get = [i = 0, &f]() mutable { return i < f.size() ? int(f[i++]) : -1; };
        StreamStack stream(&get);
        tk_iterator function_it(stream);
        function_declaration decl = parse_function_declaration(ctx, function_it);
        public_function_types.emplace(decl.name, decl.type_id);
    }

    std::vector<expression<lvalue>::ptr> initializers;
    
    std::vector<incomplete_function> incomplete_functions;
    std::unordered_map<std::string, size_t> public_functions;
    
    while(it())
    {
        if(!std::holds_alternative<Tokens>(it->get_value()))
            unexpected_syntax(it).expose();
    
        bool public_function = false;
        
        switch(it->get_token())
        {
            case Tokens::kw_public:
                public_function = true;
                if(!(++it)->has_value(Tokens::kw_fn))
                    unexpected_syntax(it).expose();
            case Tokens::kw_fn:
            {
                size_t line_number = it->get_line_number();
                const incomplete_function& f = incomplete_functions.emplace_back(ctx, it);

                if(public_function)
                {
                    auto it = public_function_types.find(f.get_decl().name);
                
                    if(it != public_function_types.end() && it->second != f.get_decl().type_id)
                        semantic_error(std::string("function doesn't match it's declaration " + std::to_string(it->second)).c_str(), line_number).expose();
                    else
                        public_function_types.erase(it);
                
                    public_functions.emplace(f.get_decl().name, external_functions.size() + incomplete_functions.size() - 1);
                }

                break;
            }
            default:
                for(expression<lvalue>::ptr& expr : compile_variable_declaration(ctx, it))
                    initializers.push_back(std::move(expr));
                parse_token_value(ctx, it, Tokens::semicolon);
            break;
        }
    }
    
    if(!public_function_types.empty())
        semantic_error(std::string("public function '" + public_function_types.begin()->first + "' is not defined.").c_str(), it->get_line_number()).expose();
    
    std::vector<function> functions;
    
    functions.reserve(external_functions.size() + incomplete_functions.size());
    
    for(const std::pair<std::string, function>& p : external_functions)
        functions.emplace_back(p.second);
    
    for(incomplete_function& f : incomplete_functions)
        functions.emplace_back(f.compile(ctx));
    
    return runtime_context(std::move(initializers), std::move(functions), std::move(public_functions));
}
