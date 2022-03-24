#ifndef __VARIABLE__
#define __VARIABLE__

#include <memory>
#include <deque>
#include "function.h"
#include <string>

class variable;

using variable_ptr = std::shared_ptr<variable>;

template <typename T>
class variable_impl;

class runtime_context;

using number = double;
using string = std::shared_ptr<std::string>;
using function = func::function<void(runtime_context&)>;

using lvalue = variable_ptr;
using lnumber = std::shared_ptr<variable_impl<number>>;
using lstring = std::shared_ptr<variable_impl<string>>;
using lfunction = std::shared_ptr<variable_impl<function>>;

inline number clone_variable_value(number value) { return value; }
inline string clone_variable_value(const string& value) { return value; }
inline function clone_variable_value(const function& value) { return value; }

class variable: public std::enable_shared_from_this<variable>
{
    public:
        template <typename T>
        T static_pointer_downcast() { return std::static_pointer_cast<variable_impl<typename T::element_type::value_type>>(shared_from_this()); }
        virtual variable_ptr clone() const = 0;
        virtual string to_string() const = 0;
        virtual ~variable() = default;

    protected:
        variable() = default;

    private:
        variable(const variable&) = delete;
        void operator=(const variable&) = delete;
};

template<typename T>
class variable_impl: public variable
{
    public:
        using value_type = T;
        
        value_type value;
        variable_impl(value_type value);
        inline variable_ptr clone() const override { return std::make_shared<variable_impl<T>>(clone_variable_value(value)); }
        string to_string() const override;
};

template <class T>
inline T clone_variable_value(const std::shared_ptr<variable_impl<T>>& v) { return clone_variable_value(v->value); }

inline string from_std_string(std::string str) { return std::make_shared<std::string>(std::move(str)); }

inline string convert_to_string(number value) { return value == int(value) ? from_std_string(std::to_string(int(value))) : from_std_string(std::to_string(value)); }
inline string convert_to_string(const string& value) { return value; }
inline string convert_to_string(const function& value) { return from_std_string("FUNCTION"); }	
inline string convert_to_string(const lvalue& var) { return var->to_string(); }

#endif // __VARIABLE__
