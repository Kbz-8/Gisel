#ifndef __ERRORS__
#define __ERRORS__

#include <string>
#include <iostream>
#include "utils.h"

class Error
{
    public:
        Error(const char* message, size_t line);
        Error(std::string message, size_t line);

        inline const char* what() const noexcept { return _message.c_str(); }
        void expose() const;
        inline int line() const noexcept { return _line; }
    
    private:
        std::string _message;
        int _line = 0;
};

Error parsing_error(const char* message, size_t line);
Error syntax_error(const char* message, size_t line);
Error semantic_error(const char* message, size_t line);
Error compiler_error(const char* message, size_t line);

Error unexpected_error(const char* unexpected, size_t line);
Error unexpected_syntax_error(const char* unexpected, size_t line);
Error expected_syntax_error(const char* expected, size_t line);
Error undeclared_error(const char* undeclared, size_t line);
Error wrong_type_error(const char* source, const char* destination, bool lvalue, size_t line);
Error already_declared_error(const char* name, size_t line);

Error file_not_found(const char* file);

class runtime_error
{
    public:
        runtime_error(std::string message) noexcept { _message = std::move(message); }
        inline const char* what() const noexcept { return _message.c_str(); }
        inline void expose() const
        {
            std::cout << OUT_RED << "Gisel runtime error : " << OUT_DEF  << _message << std::endl;
            std::exit(EXIT_FAILURE);
        }
    
    private:
        std::string _message;
};

inline void runtime_assertion(bool b, std::string message)
{
    if(!b)
		runtime_error(std::move(message)).expose();
}

#endif // __EROROR__
