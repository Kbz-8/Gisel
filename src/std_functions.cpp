#include "std_functions.h"
#include "errors.h"
#include "module.h"

#include <iostream>
#include <string>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <cstdarg>

void add_math_functions(module& m)
{
    m.add_external_function("sin", func::function<number(number)>(
        [](number x)
        {
            return std::sin(x);
        }
    ));
    
    m.add_external_function("cos", func::function<number(number)>(
        [](number x)
        {
            return std::cos(x);
        }
    ));
    
    m.add_external_function("tan", func::function<number(number)>(
        [](number x)
        {
            return std::tan(x);
        }
    ));
    
    m.add_external_function("log", func::function<number(number)>(
        [](number x)
        {
            return std::log(x);
        }
    ));
    
    m.add_external_function("exp", func::function<number(number)>(
        [](number x)
        {
            return std::exp(x);
        }
    ));
    
    m.add_external_function("pow", func::function<number(number, number)>(
        [](number x, number y)
        {
            return std::pow(x, y);
        }
    ));

    m.add_external_function("abs", func::function<number(number)>(
        [](number x)
        {
            float a = x;
            long i = *(long*)&a; // tricky convertion
            i &= ~(1u << 31); // set sign to 0 makes it positive
            return *(float*)&i; // another tricky convertion
        }
    ));
    
    srand((unsigned int)time(0));
    
    m.add_external_function("random", func::function<number(number)>(
        [](number x)
        {
            return rand() % int(x);
        }
    ));
}

void add_string_functions(module& m)
{
    m.add_external_function("strlen", func::function<number(const std::string&)>(
        [](const std::string& str)
        {
            return str.size();
        }
    ));
    
    m.add_external_function("substr", func::function<std::string(const std::string&, number, number)>(
        [](const std::string& str, number from, number count)
        {
            return str.substr(size_t(from), size_t(count));
        }
    ));

    m.add_external_function("to_str", func::function<std::string(number)>(
        [](number num)
        {
            return num == (int)num ? std::to_string((int)num) : std::to_string(num);
        }
    ));
    m.add_external_function("to_num", func::function<number(const std::string&)>(
        [](const std::string& str)
        {
            for(char const& c : str)
                runtime_assertion(std::isdigit(c), "trying to convert non digit string to number");
            return std::stod(str);
        }
    ));
}

void add_io_functions(module& m, uint8_t opt)
{
    if(opt == 0)
    {
        m.add_external_function("print", func::function<void(const std::string&)>(
            [](const std::string& val)
            {
                std::cout << val << std::endl;
            }
        ));
    }
    else if(opt == 1)
    {
        m.add_external_function("print", func::function<void(const std::string&)>(
            [](const std::string& val)
            {
                puts(val.c_str());
                fflush(stdout);
            }
        ));
    }

    m.add_external_function("entry", func::function<std::string()>(
        []()
        {
            std::string str;
            std::getline(std::cin, str);
            return str;
        }
    ));
}

void add_standard_functions(module& m, uint8_t opt)
{
    add_math_functions(m);
    add_string_functions(m);
    add_io_functions(m, opt);
}
