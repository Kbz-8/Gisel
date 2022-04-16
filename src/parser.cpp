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

#include "parser.h"
#include "expression_tree.h"
#include "lexer.h"
#include "errors.h"
#include "tk_iterator.h"
#include <stack>

namespace
{
    enum class operator_precedence
    {
        postfix,
        prefix,
        multiplication,
        addition,
        shift,
        comparison,
        equality,
        bitwise_and,
        logical_and,
        logical_or,
        assignment,
        comma,
        import
    };
    
    enum class operator_associativity
    {
        left_to_right,
        right_to_left
    };
    
    struct operator_info
    {
        node_operation operation;
        operator_precedence precedence;
        operator_associativity associativity;
        int number_of_operands;
        size_t line_number;
        
        operator_info(node_operation operation, size_t line_number) : operation(operation), line_number(line_number)
        {
            switch(operation)
            {
                case node_operation::param: // This will never happen. Used only for the node creation.
                case node_operation::postinc:
                case node_operation::postdec:
                case node_operation::call: precedence = operator_precedence::postfix; break;
                case node_operation::preinc:
                case node_operation::predec:
                case node_operation::positive:
                case node_operation::negative:
                case node_operation::bnot:
                case node_operation::lnot: precedence = operator_precedence::prefix; break;
                case node_operation::mul:
                case node_operation::div:
                case node_operation::mod: precedence = operator_precedence::multiplication; break;
                case node_operation::add:
                case node_operation::sub: precedence = operator_precedence::addition; break;
                case node_operation::lt:
                case node_operation::gt:
                case node_operation::le:
                case node_operation::ge: precedence = operator_precedence::comparison; break;
                case node_operation::eq:
                case node_operation::ne: precedence = operator_precedence::equality; break;
                case node_operation::assign:
                case node_operation::add_assign:
                case node_operation::sub_assign:
                case node_operation::mul_assign:
                case node_operation::div_assign:
                case node_operation::mod_assign:
                case node_operation::ternary: precedence = operator_precedence::assignment; break;
                case node_operation::comma: precedence = operator_precedence::comma; break;
                case node_operation::import: precedence = operator_precedence::import; break;
            }
            
            switch(precedence)
            {
                case operator_precedence::prefix:
                case operator_precedence::assignment: associativity = operator_associativity::right_to_left; break;
                
                default: associativity = operator_associativity::left_to_right; break;
            }
            
            switch(operation)
            {
                case node_operation::postinc:
                case node_operation::postdec:
                case node_operation::preinc:
                case node_operation::predec:
                case node_operation::positive:
                case node_operation::negative:
                case node_operation::bnot:
                case node_operation::lnot:
                case node_operation::import:
                case node_operation::call: number_of_operands = 1; break;
                case node_operation::ternary: number_of_operands = 3; break;
                
                default: number_of_operands = 2; break;
            }
        }
    };

    operator_info get_operator_info(Tokens token, bool prefix, size_t line_number)
    {
        switch(token)
        {
            case Tokens::inc:
                return prefix ? operator_info(node_operation::preinc, line_number) : operator_info(node_operation::postinc, line_number);
            case Tokens::dec:
                return prefix ? operator_info(node_operation::predec, line_number) : operator_info(node_operation::postdec, line_number);
            case Tokens::add:
                return prefix ? operator_info(node_operation::positive, line_number) : operator_info(node_operation::add, line_number);
            case Tokens::sub:
                return prefix ? operator_info(node_operation::negative, line_number) : operator_info(node_operation::sub, line_number);
            case Tokens::mul:
                return operator_info(node_operation::mul, line_number);
            case Tokens::div:
                return operator_info(node_operation::div, line_number);
            case Tokens::mod:
                return operator_info(node_operation::mod, line_number);
            case Tokens::assign:
                return operator_info(node_operation::assign, line_number);
            case Tokens::add_assign:
                return operator_info(node_operation::add_assign, line_number);
            case Tokens::sub_assign:
                return operator_info(node_operation::sub_assign, line_number);
            case Tokens::mul_assign:
                return operator_info(node_operation::mod_assign, line_number);
            case Tokens::div_assign:
                return operator_info(node_operation::div_assign, line_number);
            case Tokens::mod_assign:
                return operator_info(node_operation::mod_assign, line_number);
            case Tokens::bitwise_and:
                return operator_info(node_operation::band, line_number);
            case Tokens::logical_not:
                return operator_info(node_operation::lnot, line_number);
            case Tokens::logical_and:
                return operator_info(node_operation::land, line_number);
            case Tokens::logical_or:
                return operator_info(node_operation::lor, line_number);
            case Tokens::eq:
                return operator_info(node_operation::eq, line_number);
            case Tokens::ne:
                return operator_info(node_operation::ne, line_number);
            case Tokens::lt:
                return operator_info(node_operation::lt, line_number);
            case Tokens::gt:
                return operator_info(node_operation::gt, line_number);
            case Tokens::le:
                return operator_info(node_operation::le, line_number);
            case Tokens::ge:
                return operator_info(node_operation::ge, line_number);
            case Tokens::question:
                return operator_info(node_operation::ternary, line_number);
            case Tokens::comma:
                return operator_info(node_operation::comma, line_number);
            case Tokens::bracket_b:
                return operator_info(node_operation::call, line_number);
            case Tokens::kw_import:
                return operator_info(node_operation::import, line_number);

            default: unexpected_syntax_error(std::to_string(token).c_str(), line_number).expose();
        }
    }

    bool is_end_of_expression(const Token& t, bool allow_comma)
    {
        if(t.is_eof())
            return true;

        if(t.is_keyword())
        {
            switch(t.get_token())
            {
                case Tokens::semicolon:
                case Tokens::bracket_e:
                case Tokens::square_e:
                case Tokens::type_specifier: return true;
                case Tokens::comma: return !allow_comma;
                
                default: return false;
            }
        }
        
        return false;
    }
    
    inline bool is_evaluated_before(const operator_info& l, const operator_info& r) { return l.associativity == operator_associativity::left_to_right ? l.precedence <= r.precedence : l.precedence < r.precedence; }
    
    void pop_one_operator(std::stack<operator_info>& operator_stack, std::stack<node_ptr>& operand_stack, compiler_context& context, size_t line_number)
    {
        if(operand_stack.size() < operator_stack.top().number_of_operands)
            compiler_error("failed to parse an expression", line_number).expose();
        
        std::vector<node_ptr> operands;
        operands.resize(operator_stack.top().number_of_operands);
        
        if(operator_stack.top().precedence != operator_precedence::prefix)
            operator_stack.top().line_number = operand_stack.top()->get_line_number();
        
        for(int i = operator_stack.top().number_of_operands - 1; i >= 0; --i)
        {
            operands[i] = std::move(operand_stack.top());
            operand_stack.pop();
        }
        
        operand_stack.push(std::make_unique<node>(context, operator_stack.top().operation, std::move(operands), operator_stack.top().line_number));
        operator_stack.pop();
    }
    
    node_ptr parse_expression_tree_impl(compiler_context& context, tk_iterator& it, bool allow_comma, bool allow_empty)
    {
        std::stack<node_ptr> operand_stack;
        std::stack<operator_info> operator_stack;
        
        bool expected_operand = true;
        
        for(; !is_end_of_expression(*it, allow_comma); ++it)
        {
            if(it->is_keyword())
            {
                operator_info oi = get_operator_info(it->get_token(), expected_operand, it->get_line_number());
                
                if(oi.operation == node_operation::call && expected_operand)
                {
                    //open round bracket is misinterpreted as a function call
                    ++it;
                    operand_stack.push(parse_expression_tree_impl(context, it, true, false));
                    if(it->has_value(Tokens::bracket_e))
                    {
                        expected_operand = false;
                        continue;
                    }
                    else
                        syntax_error("expected closing ')'", it->get_line_number()).expose();
                }
                
                if((oi.precedence == operator_precedence::prefix) != expected_operand)
                    unexpected_syntax_error(std::to_string(it->get_value()).c_str(), it->get_line_number()).expose();
                
                if(!operator_stack.empty() && is_evaluated_before(operator_stack.top(), oi))
                    pop_one_operator(operator_stack, operand_stack, context, it->get_line_number());

                switch(oi.operation)
                {
                    case node_operation::call:
                        ++it;
                        if(!it->has_value(Tokens::bracket_e))
                        {
                            while(true)
                            {
                                bool remove_lvalue = !it->has_value(Tokens::type_specifier); // is passed by reference
                                if(!remove_lvalue)
                                    ++it;
                                node_ptr argument = parse_expression_tree_impl(context, it, false, false);
                                if(remove_lvalue)
                                {
                                    size_t line_number = argument->get_line_number();
                                    std::vector<node_ptr> argument_vector;
                                    argument_vector.push_back(std::move(argument));
                                    argument = std::make_unique<node>(context, node_operation::param, std::move(argument_vector), line_number);
                                }
                                else if(!argument->is_lvalue())
                                    wrong_type_error(std::to_string(argument->get_type_id()).c_str(), std::to_string(argument->get_type_id()).c_str(), true, argument->get_line_number());
                                 
                                operand_stack.push(std::move(argument));

                                ++oi.number_of_operands;
                                
                                if(it->has_value(Tokens::bracket_e))
                                    break;
                                else if(it->has_value(Tokens::comma))
                                    ++it;
                                else
                                    syntax_error("expected ',' or closing ')'", it->get_line_number()).expose();
                            }
                        }
                    break;
                    case node_operation::ternary:
                        ++it;
                        operand_stack.push(parse_expression_tree_impl(context, it, true, false));
                        if(!it->has_value(Tokens::type_specifier))
                            syntax_error("expected ':'", it->get_line_number()).expose();
                    break;
                    
                    default: break;
                }
                
                operator_stack.push(oi);
                expected_operand = (oi.precedence != operator_precedence::postfix);
            }
            else
            {
                if(!expected_operand)
                    unexpected_syntax_error(std::to_string(it->get_value()).c_str(), it->get_line_number()).expose();
                if(it->is_number())
                    operand_stack.push(std::make_unique<node>(context, it->get_number(), std::vector<node_ptr>(), it->get_line_number()));
                else if(it->is_string()) 
                    operand_stack.push(std::make_unique<node>(context, it->get_string(), std::vector<node_ptr>(), it->get_line_number()));
                else
                    operand_stack.push(std::make_unique<node>(context, it->get_identifier(), std::vector<node_ptr>(), it->get_line_number()));
                expected_operand = false;
            }
        }
        
        if(expected_operand)
        {
            if(allow_empty && operand_stack.empty() && operator_stack.empty())
                return node_ptr();
            syntax_error("operand expected", it->get_line_number()).expose();
        }
        
        while(!operator_stack.empty())
            pop_one_operator(operator_stack, operand_stack, context, it->get_line_number());
        
        if(operand_stack.size() != 1 || !operator_stack.empty())
            compiler_error("failed to parse an expression", it->get_line_number()).expose();
        
        return std::move(operand_stack.top());
    }
}

node_ptr parse_expression_tree(compiler_context& context, tk_iterator& it, type_handle type_id, bool allow_comma)
{
    node_ptr ret = parse_expression_tree_impl(context, it, allow_comma, type_id == type_registry::get_void_handle());
    if(ret)
		ret->check_conversion(type_id, false);
    return ret;
}
