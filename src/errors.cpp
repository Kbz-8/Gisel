#include "errors.h"
#include <cstdlib>
#include <string.h>

Error::Error(const char* message, size_t line)
{
    _message = message;
    _line = line + 1;
}
Error::Error(std::string message, size_t line)
{
    _message = std::move(message);
    _line = line + 1;
}

void Error::expose() const
{
    if(_line == -1)
        std::cout << OUT_RED << "Nir error : " << OUT_DEF  << _message << std::endl;
    else
        std::cout << OUT_RED << "Nir error : " << OUT_DEF  << _message << ", line : " << _line << std::endl;
    std::exit(EXIT_FAILURE);
}

Error parsing_error(const char* message, size_t line)
{
    std::string dest(strlen(message) + 32, '\0');
    sprintf(&dest[0], "parsing error : '%s'", message);
    return Error(std::move(dest), line);
}
Error syntax_error(const char* message, size_t line)
{
    std::string dest(strlen(message) + 31, '\0');
    sprintf(&dest[0], "syntax error : '%s'", message);
    return Error(std::move(dest), line);
}
Error semantic_error(const char* message, size_t line)
{
    std::string dest(strlen(message) + 33, '\0');
    sprintf(&dest[0], "semantic error : %s", message);
    return Error(std::move(dest), line);
}
Error compiler_error(const char* message, size_t line)
{
    std::string dest(strlen(message) + 33, '\0');
    sprintf(&dest[0], "compiler error : %s", message);
    return Error(std::move(dest), line);
}

Error unexpected_error(const char* message, size_t line)
{
    std::string dest(strlen(message) + 43, '\0');
    sprintf(&dest[0], "parsing error : unexpected '%s'", message);
    return Error(std::move(dest), line);
}
Error unexpected_syntax_error(const char* message, size_t line)
{
    std::string dest(strlen(message) + 42, '\0');
    sprintf(&dest[0], "unexpected syntax error : '%s'", message);
    return Error(std::move(dest), line);
}
Error expected_syntax_error(const char* message, size_t line)
{
    std::string dest(strlen(message) + 40, '\0');
    sprintf(&dest[0], "expected syntax error : '%s'", message);
    return Error(std::move(dest), line);
}
Error undeclared_error(const char* message, size_t line)
{
    std::string dest(strlen(message) + 35, '\0');
    sprintf(&dest[0], "undeclared error : '%s'", message);
    return Error(std::move(dest), line);
}
Error wrong_type_error(const char* message, const char* destination, bool lvalue, size_t line)
{
    std::string dest = "wrong type error : ";
    if(lvalue)
    {
        dest += "'";
        dest += message;
        dest += "' is not a lvalue";
    }
    else
    {
        dest += "cannot convert '";
        dest +=  message;
        dest += "' to '";
        dest +=  destination;
        dest += "'";
    }
    return Error(std::move(dest), line);
}
Error already_declared_error(const char* message, size_t line)
{
    std::string dest(strlen(message) + 41, '\0');
    sprintf(&dest[0], "already declared error : '%s'", message);
    return Error(std::move(dest), line);
}

Error file_not_found(const char* file)
{
    std::string msg = "file not found : ";
    msg += file;
    return Error(std::move(msg), -2);
}
