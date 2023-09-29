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

#include "expression.h"
#include "expression_tree.h"
#include "parser.h"
#include "utils.h"
#include "errors.h"
#include "tk_iterator.h"
#include "runtime_context.h"
#include "compiler_context.h"
#include <type_traits>

namespace Gisel
{
	template <class V, typename T>
	struct is_boxed { static constexpr const bool value = false; };

	template <typename T>
	struct is_boxed<std::shared_ptr<variable_impl<T>>,T> { static constexpr const bool value = true; };

	template <typename T>
	struct remove_cvref { using type = typename std::remove_cv<typename std::remove_reference<T>::type>::type; };

	template <typename T>
	auto unbox(T&& t) { return t->value; }

	template <typename To, typename From>
	auto convert(From&& from)
	{
		if constexpr(std::is_convertible<From, To>::value)
			return std::forward<From>(from);
		else if constexpr(is_boxed<From, To>::value)
			return unbox(std::forward<From>(from));
		else
			return To();
	}

	template <typename From, typename To>
	struct is_convertible { static const bool value = std::is_convertible<From, To>::value || is_boxed<From, To>::value || (std::is_same<To, string>::value && (std::is_same<From, number>::value || std::is_same<From, lnumber>::value)) || std::is_void<To>::value; };

	number lt(number n1, number n2) { return n1 < n2; }
	number lt(string s1, string s2) { return *s1 < *s2; }

	template <typename R, typename T>
	class global_variable_expression: public expression<R>
	{
		public:
			global_variable_expression(int idx) : _idx(idx) {}              
			R evaluate(runtime_context& context) const override
			{
				if constexpr(std::is_void<R>::value)
					return;
				else
					return convert<R>(context.global(_idx)->template static_pointer_downcast<T>());
			}

		private:
			int _idx;
	};

	template <typename R, typename T>
	class local_variable_expression: public expression<R>
	{
		public:
			local_variable_expression(int idx) : _idx(idx) {}
			R evaluate(runtime_context& context) const override
			{
				if constexpr(std::is_void<R>::value)
					return;
				else
					return convert<R>(context.local(_idx)->template static_pointer_downcast<T>());
			}

		private:
			int _idx;
	};

	template<typename R>
	class function_expression: public expression<R>
	{
		public:
			function_expression(int idx) : _idx(idx) {}
			R evaluate(runtime_context& context) const override { return convert<R>(context.get_function(_idx)); }

		private:
			int _idx;
	};

	template<typename R, typename T>
	class constant_expression: public expression<R>
	{
		public:
			constant_expression(T c) : _c(std::move(c)) {} 
			R evaluate(runtime_context& context) const override { return convert<R>(_c); }

		private:
			T _c;
	};

	template<class O, typename R, typename... Ts>
	class generic_expression: public expression<R>
	{
		public:
			generic_expression(typename expression<Ts>::ptr... exprs) : _exprs(std::move(exprs)...) {}
			R evaluate(runtime_context& context) const override { return std::apply([&](const auto&... exprs){ return this->evaluate_tuple(context, exprs...); }, _exprs);}

		private:
			std::tuple<typename expression<Ts>::ptr...> _exprs;
			
			template<typename... Exprs>
			R evaluate_tuple(runtime_context& context, const Exprs&... exprs) const
			{
				if constexpr(std::is_same<R, void>::value)
					O()(std::move(exprs->evaluate(context))...);
				else
					return convert<R>(O()(std::move(exprs->evaluate(context))...));

			}
	};

#define UNARY_EXPRESSION(name, code)\
			struct name##_op\
			{\
				template <typename T1> \
				auto operator()(T1 t1)\
				{\
					code;\
				}\
			};\
			template<typename R, typename T1>\
			using name##_expression = generic_expression<name##_op, R, T1>;

			UNARY_EXPRESSION(preinc,
				++t1->value;
				return t1;
			);

			UNARY_EXPRESSION(predec,
				--t1->value;
				return t1;
			);

			UNARY_EXPRESSION(postinc, return t1->value++);
			
			UNARY_EXPRESSION(postdec, return t1->value--);
			
			UNARY_EXPRESSION(positive, return t1);
			
			UNARY_EXPRESSION(negative, return -t1);
			
			UNARY_EXPRESSION(bnot, return ~int(t1));
			
			UNARY_EXPRESSION(lnot, return !t1);

#undef UNARY_EXPRESSION

#define BINARY_EXPRESSION(name, code)\
			struct name##_op\
			{\
				template <typename T1, typename T2>\
				auto operator()(T1 t1, T2 t2)\
				{\
					code;\
				}\
			};\
			template<typename R, typename T1, typename T2>\
			using name##_expression = generic_expression<name##_op, R, T1, T2>;

			BINARY_EXPRESSION(add, return t1 + t2);
			
			BINARY_EXPRESSION(sub, return t1 - t2);

			BINARY_EXPRESSION(mul, return t1 * t2);

			BINARY_EXPRESSION(div, return t1 / t2);

			BINARY_EXPRESSION(mod, return t1 - t2 * int(t1/t2));

			BINARY_EXPRESSION(add_assign,
				t1->value += t2;
				return t1;
			);
			
			BINARY_EXPRESSION(sub_assign,
				t1->value -= t2;
				return t1;
			);
			
			BINARY_EXPRESSION(mul_assign,
				t1->value *= t2;
				return t1;
			);
			
			BINARY_EXPRESSION(div_assign,
				t1->value /= t2;
				return t1;
			);
			
			BINARY_EXPRESSION(mod_assign,
				t1->value = t1->value - t2 * int(t1->value/t2);
				return t1;
			);
			
			BINARY_EXPRESSION(assign,
				t1->value = std::move(t2);
				return t1;
			);
			
			BINARY_EXPRESSION(eq, return !lt(t1, t2) && !lt(t2, t1));
			
			BINARY_EXPRESSION(ne, return lt(t1, t2) || lt(t2, t1));
			
			BINARY_EXPRESSION(lt, return lt(t1, t2));
			
			BINARY_EXPRESSION(gt, return lt(t2, t1));
			
			BINARY_EXPRESSION(le, return !lt(t2, t1));
			
			BINARY_EXPRESSION(ge, return !lt(t1, t2));

#undef BINARY_EXPRESSION

	template<typename R, typename T1, typename T2>
	class comma_expression: public expression<R>
	{
		public:
			comma_expression(typename expression<T1>::ptr expr1, typename expression<T2>::ptr expr2) : _expr1(std::move(expr1)), _expr2(std::move(expr2)) {}
			
			R evaluate(runtime_context& context) const override
			{
				_expr1->evaluate(context);
				
				if constexpr(std::is_same<R, void>::value)
					_expr2->evaluate(context);
				else
					return convert<R>(_expr2->evaluate(context));
			}

		private:
			typename expression<T1>::ptr _expr1;
			typename expression<T2>::ptr _expr2;
	};

	template<typename R, typename T1, typename T2>
	class land_expression: public expression<R>
	{
		public:
			land_expression(typename expression<T1>::ptr expr1, typename expression<T2>::ptr expr2) : _expr1(std::move(expr1)), _expr2(std::move(expr2)) {}    
			R evaluate(runtime_context& context) const override { return convert<R>(_expr1->evaluate(context) && _expr2->evaluate(context)); }
		
		private:
			typename expression<T1>::ptr _expr1;
			typename expression<T2>::ptr _expr2;
	};

	template<typename R, typename T1, typename T2>
	class lor_expression: public expression<R>
	{
		public:
			lor_expression(typename expression<T1>::ptr expr1, typename expression<T2>::ptr expr2) : _expr1(std::move(expr1)), _expr2(std::move(expr2)) {}
			R evaluate(runtime_context& context) const override { return convert<R>(_expr1->evaluate(context) || _expr2->evaluate(context)); }

		private:
			typename expression<T1>::ptr _expr1;
			typename expression<T2>::ptr _expr2;
	};

	template<typename R, typename T1, typename T2, typename T3>
	class ternary_expression: public expression<R>
	{
		public:
			ternary_expression(typename expression<T1>::ptr expr1, typename expression<T2>::ptr expr2, typename expression<T2>::ptr expr3) : _expr1(std::move(expr1)), _expr2(std::move(expr2)), _expr3(std::move(expr3)) {}
			
			R evaluate(runtime_context& context) const override
			{
				if constexpr(std::is_same<R, void>::value)
					_expr1->evaluate(context) ? _expr2->evaluate(context) : _expr3->evaluate(context);
				else
					return convert<R>(_expr1->evaluate(context) ? _expr2->evaluate(context) : _expr3->evaluate(context));
			}

		private:
			typename expression<T1>::ptr _expr1;
			typename expression<T2>::ptr _expr2;
			typename expression<T3>::ptr _expr3;
	};

	template<typename R, typename T>
	class call_expression: public expression<R>
	{
		public:
			call_expression(expression<function>::ptr fexpr, std::vector<expression<lvalue>::ptr> exprs) : _fexpr(std::move(fexpr)), _exprs(std::move(exprs)) {}
			
			R evaluate(runtime_context& context) const override
			{
				std::vector<variable_ptr> params;
				params.reserve(_exprs.size());
			
				for(size_t i = 0; i < _exprs.size(); ++i)
					params.push_back(_exprs[i]->evaluate(context));
				
				function f = _fexpr->evaluate(context);
				
				if constexpr(std::is_same<R, void>::value)
					context.call(f, std::move(params));
				else
					return convert<R>(std::move(std::static_pointer_cast<variable_impl<T>>(context.call(f, std::move(params)))->value));
			}
			
		private:
			expression<function>::ptr _fexpr;
			std::vector<expression<lvalue>::ptr> _exprs;
	};

	template <typename T>
	class param_expression: public expression<lvalue>
	{
		public:
			param_expression(typename expression<T>::ptr expr) : _expr(std::move(expr)) {}                
			lvalue evaluate(runtime_context& context) const override { return std::make_shared<variable_impl<T>>(_expr->evaluate(context)); }

		private:
			typename expression<T>::ptr _expr;
	};

	struct expression_builder_error { expression_builder_error(){} };

	expression<lvalue>::ptr build_lvalue_expression(type_handle type_id, const node_ptr& np, compiler_context& context);

#define RETURN_EXPRESSION_OF_TYPE(T)\
		if constexpr(is_convertible<T, R>::value)\
			return build_##T##_expression(np, context);\
		else\
		{\
			throw expression_builder_error();\
			return expression_ptr();\
		}

#define CHECK_IDENTIFIER(T1)\
		if(std::holds_alternative<identifier>(np->get_value()))\
		{\
			const identifier& id = std::get<identifier>(np->get_value());\
			const identifier_info* info = context.find(id.name);\
			switch(info->get_scope())\
			{\
				case identifier_scope::global_variable: return std::make_unique<global_variable_expression<R, T1>>(info->index());\
				case identifier_scope::local_variable: return std::make_unique<local_variable_expression<R, T1>>(info->index());\
				case identifier_scope::function: break;\
			}\
		}

#define CHECK_FUNCTION()\
		if(std::holds_alternative<identifier>(np->get_value()))\
		{\
			const identifier& id = std::get<identifier>(np->get_value());\
			const identifier_info* info = context.find(id.name);\
			switch(info->get_scope())\
			{\
				case identifier_scope::global_variable:\
				case identifier_scope::local_variable: break;\
				case identifier_scope::function: return std::make_unique<function_expression<R> >(info->index());\
			}\
		}

#define CHECK_UNARY_OPERATION(name, T1)\
		case node_operation::name: return expression_ptr(std::make_unique<name##_expression<R, T1>>(expression_builder<T1>::build_expression(np->get_children()[0], context)));

#define CHECK_BINARY_OPERATION(name, T1, T2)\
		case node_operation::name:\
			return expression_ptr(std::make_unique<name##_expression<R, T1, T2>>(expression_builder<T1>::build_expression(np->get_children()[0], context), expression_builder<T2>::build_expression(np->get_children()[1], context)));

#define CHECK_TERNARY_OPERATION(name, T1, T2, T3)\
		case node_operation::name:\
			return expression_ptr(std::make_unique<name##_expression<R, T1, T2, T3>>(expression_builder<T1>::build_expression(np->get_children()[0], context), expression_builder<T2>::build_expression(np->get_children()[1], context), expression_builder<T3>::build_expression(np->get_children()[2], context)));

#define CHECK_COMPARISON_OPERATION(name)\
			case node_operation::name:\
				if(np->get_children()[0]->get_type_id() == type_registry::get_number_handle() && np->get_children()[1]->get_type_id() == type_registry::get_number_handle())\
					return expression_ptr(std::make_unique<name##_expression<R, number, number>>(expression_builder<number>::build_expression(np->get_children()[0], context), expression_builder<number>::build_expression(np->get_children()[1], context)));\
				else\
					return expression_ptr(std::make_unique<name##_expression<R, string, string>>(expression_builder<string>::build_expression(np->get_children()[0], context), expression_builder<string>::build_expression(np->get_children()[1], context)));

#define CHECK_CALL_OPERATION(T)\
		case node_operation::call:\
		{\
			std::vector<expression<lvalue>::ptr> arguments;\
			const function_type* ft = std::get_if<function_type>(np->get_children()[0]->get_type_id());\
			for(size_t i = 1; i < np->get_children().size(); ++i)\
			{\
				const node_ptr& child = np->get_children()[i];\
				if(child->is_node_operation() && std::get<node_operation>(child->get_value()) == node_operation::param)\
					arguments.push_back(build_lvalue_expression(ft->param_type_id[i-1].type_id, child->get_children()[0], context));\
				else\
					arguments.push_back(expression_builder<lvalue>::build_expression(child, context));\
			}\
			return expression_ptr(std::make_unique<call_expression<R, T>>(expression_builder<function>::build_expression(np->get_children()[0], context), std::move(arguments)));\
		}

	template<typename R>
	class expression_builder
	{
		using expression_ptr = typename expression<R>::ptr;

		public:
			static expression_ptr build_expression(const node_ptr& np, compiler_context& context)
			{
				return std::visit(overloaded
				{
					[&](simple_type st)
					{
						switch(st)
						{
							case simple_type::number:
								if(np->is_lvalue())
								{
									RETURN_EXPRESSION_OF_TYPE(lnumber);
								}
								else
								{
									RETURN_EXPRESSION_OF_TYPE(number);
								}
							case simple_type::string:
								if(np->is_lvalue())
								{
									RETURN_EXPRESSION_OF_TYPE(lstring);
								}
								else
								{
									RETURN_EXPRESSION_OF_TYPE(string);
								}
							case simple_type::nothing: RETURN_EXPRESSION_OF_TYPE(void);
						}
					},
					[&](const function_type& ft)
					{
						if(np->is_lvalue())
						{
							RETURN_EXPRESSION_OF_TYPE(lfunction);
						}
						else
						{
							RETURN_EXPRESSION_OF_TYPE(function);
						}
					},
				}, *np->get_type_id());
			}
			
			static expression<lvalue>::ptr build_param_expression(const node_ptr& np, compiler_context& context) { return std::make_unique<param_expression<R>>(expression_builder<R>::build_expression(np, context)); }
		
		private:
			static expression_ptr build_void_expression(const node_ptr& np, compiler_context& context)
			{
				switch(std::get<node_operation>(np->get_value()))
				{
					CHECK_BINARY_OPERATION(comma, void, void);
					CHECK_TERNARY_OPERATION(ternary, number, void, void);
					CHECK_CALL_OPERATION(void);
					default: throw expression_builder_error();
				}
			}
			
			static expression_ptr build_number_expression(const node_ptr& np, compiler_context& context)
			{
				if(std::holds_alternative<double>(np->get_value()))
					return std::make_unique<constant_expression<R, number>>(std::get<double>(np->get_value()));
				
				CHECK_IDENTIFIER(lnumber);
				
				switch(std::get<node_operation>(np->get_value()))
				{
					CHECK_UNARY_OPERATION(postinc, lnumber);
					CHECK_UNARY_OPERATION(postdec, lnumber);
					CHECK_UNARY_OPERATION(positive, number);
					CHECK_UNARY_OPERATION(negative, number);
					CHECK_UNARY_OPERATION(bnot, number);
					CHECK_UNARY_OPERATION(lnot, number);
					CHECK_BINARY_OPERATION(add, number, number);
					CHECK_BINARY_OPERATION(sub, number, number);
					CHECK_BINARY_OPERATION(mul, number, number);
					CHECK_BINARY_OPERATION(div, number, number);
					CHECK_BINARY_OPERATION(mod, number, number);
					CHECK_COMPARISON_OPERATION(eq);
					CHECK_COMPARISON_OPERATION(ne);
					CHECK_COMPARISON_OPERATION(lt);
					CHECK_COMPARISON_OPERATION(gt);
					CHECK_COMPARISON_OPERATION(le);
					CHECK_COMPARISON_OPERATION(ge);
					CHECK_BINARY_OPERATION(comma, void, number);
					CHECK_BINARY_OPERATION(land, number, number);
					CHECK_BINARY_OPERATION(lor, number, number);
					CHECK_TERNARY_OPERATION(ternary, number, number, number);
					CHECK_CALL_OPERATION(number);
					
					default: throw expression_builder_error();
				}
			}
			
			static expression_ptr build_lnumber_expression(const node_ptr& np, compiler_context& context)
			{
				CHECK_IDENTIFIER(lnumber);

				switch(std::get<node_operation>(np->get_value()))
				{
					CHECK_UNARY_OPERATION(preinc, lnumber);
					CHECK_UNARY_OPERATION(predec, lnumber);
					CHECK_BINARY_OPERATION(assign, lnumber, number);
					CHECK_BINARY_OPERATION(add_assign, lnumber, number);
					CHECK_BINARY_OPERATION(sub_assign, lnumber, number);
					CHECK_BINARY_OPERATION(mul_assign, lnumber, number);
					CHECK_BINARY_OPERATION(div_assign, lnumber, number);
					CHECK_BINARY_OPERATION(mod_assign, lnumber, number);
					CHECK_BINARY_OPERATION(comma, void, lnumber);
					CHECK_TERNARY_OPERATION(ternary, number, lnumber, lnumber);
					
					default: throw expression_builder_error();
				}
			}
			
			static expression_ptr build_string_expression(const node_ptr& np, compiler_context& context)
			{
				if(std::holds_alternative<std::string>(np->get_value()))
					return std::make_unique<constant_expression<R, string>>(std::make_shared<std::string>(std::get<std::string>(np->get_value())));
				
				CHECK_IDENTIFIER(lstring);
				
				switch(std::get<node_operation>(np->get_value()))
				{
					CHECK_BINARY_OPERATION(comma, void, string);
					CHECK_TERNARY_OPERATION(ternary, number, string, string);
					CHECK_CALL_OPERATION(string);
					
					default: throw expression_builder_error();
				}
			}
			
			static expression_ptr build_lstring_expression(const node_ptr& np, compiler_context& context)
			{
				CHECK_IDENTIFIER(lstring);
				
				switch(std::get<node_operation>(np->get_value()))
				{
					CHECK_BINARY_OPERATION(assign, lstring, string);
					CHECK_BINARY_OPERATION(comma, void, lstring);
					CHECK_TERNARY_OPERATION(ternary, number, lstring, lstring);
					
					default: throw expression_builder_error();
				}
			}
			
			static expression_ptr build_function_expression(const node_ptr& np, compiler_context& context)
			{
				CHECK_IDENTIFIER(lfunction);
				CHECK_FUNCTION();
				
				switch(std::get<node_operation>(np->get_value()))
				{
					CHECK_BINARY_OPERATION(comma, void, function);
					CHECK_TERNARY_OPERATION(ternary, number, function, function);
					CHECK_CALL_OPERATION(function);
					
					default: throw expression_builder_error();
				}
			}
			
			static expression_ptr build_lfunction_expression(const node_ptr& np, compiler_context& context)
			{
				CHECK_IDENTIFIER(lfunction);
				
				switch(std::get<node_operation>(np->get_value()))
				{
					CHECK_BINARY_OPERATION(assign, lfunction, function);
					CHECK_BINARY_OPERATION(comma, void, lfunction);
					CHECK_TERNARY_OPERATION(ternary, number, lfunction, lfunction);
					
					default: throw expression_builder_error();
				}
			}
			
	};

#undef CHECK_CALL_OPERATION
#undef CHECK_COMPARISON_OPERATION
#undef CHECK_TERNARY_OPERATION
#undef CHECK_BINARY_OPERATION
#undef CHECK_UNARY_OPERATION
#undef CHECK_FUNCTION
#undef CHECK_IDENTIFIER
#undef RETURN_EXPRESSION_OF_TYPE

	expression<lvalue>::ptr build_lvalue_expression(type_handle type_id, const node_ptr& np, compiler_context& context)
	{
		return std::visit(overloaded
		{
			[&](simple_type st)
			{
				switch(st)
				{
					case simple_type::number: return expression_builder<number>::build_param_expression(np, context);
					case simple_type::string: return expression_builder<string>::build_param_expression(np, context);
					case simple_type::nothing:
						throw expression_builder_error();
						return expression<lvalue>::ptr();
				}
			},
			[&](const function_type&) { return expression_builder<function>::build_param_expression(np, context); },
		}, *type_id);
	}

	class empty_expression: public expression<void> { void evaluate(runtime_context&) const override {} };

	template<typename R>
	typename expression<R>::ptr build_expression(type_handle type_id, compiler_context& context, tk_iterator& it, bool allow_comma)
	{
		size_t line_number = it->get_line_number();
		
		try
		{
			node_ptr np = parse_expression_tree(context, it, type_id, allow_comma);
			
			if constexpr(std::is_void<R>::value)
			{
				if(!np)
					return std::make_unique<empty_expression>();
			}

			if constexpr(std::is_same<R, lvalue>::value)
				return build_lvalue_expression(type_id, np, context);
			else
				return expression_builder<R>::build_expression(np, context);
		}
		catch(const expression_builder_error&)
		{
			compiler_error("expression building failed", line_number).expose();
		}
	}

	template <typename T>
	class default_initialization_expression: public expression<lvalue>
	{
		public:
			lvalue evaluate(runtime_context &context) const override { return std::make_shared<variable_impl<T>>(T{}); }
	};

	expression<void>::ptr build_void_expression(compiler_context& context, tk_iterator& it) { return build_expression<void>(type_registry::get_void_handle(), context, it, true); }
	expression<number>::ptr build_number_expression(compiler_context& context, tk_iterator& it) { return build_expression<number>(type_registry::get_number_handle(), context, it, true); }
	expression<string>::ptr build_string_expression(compiler_context& context, tk_iterator& it) { return build_expression<string>(type_registry::get_number_handle(), context, it, true); }
	expression<lvalue>::ptr build_initialization_expression(compiler_context& context, tk_iterator& it, type_handle type_id, bool allow_comma) { return build_expression<lvalue>(type_id, context, it, allow_comma); }

	expression<lvalue>::ptr build_default_initialization(type_handle type_id)
	{
		return std::visit(overloaded
		{
			[&](simple_type st)
			{
				switch(st)
				{
					case simple_type::number:  return expression<lvalue>::ptr(std::make_unique<default_initialization_expression<number>>());
					case simple_type::string:  return expression<lvalue>::ptr(std::make_unique<default_initialization_expression<string>>());
					case simple_type::nothing: return expression<lvalue>::ptr(nullptr); // cannot happen
				}
			},
			[&](const function_type& ft) { return expression<lvalue>::ptr(std::make_unique<default_initialization_expression<function>>()); },
		}, *type_id);
	}
}
