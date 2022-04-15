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
    #ifdef _WIN32
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
        std::cout << "Nir error";
        SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
        if(_line == -1)
            std::cout << " : " << _message << std::endl;
        else
            std::cout << " : " << _message << ", line : " << _line << std::endl;
    #else
        if(_line == -1)
            std::cout << OUT_RED << "Nir error" << OUT_DEF << " : " << _message << std::endl;
        else
            std::cout << OUT_RED << "Nir error" << OUT_DEF << " : " << _message << ", line : " << _line << std::endl;
    #endif
    std::exit(EXIT_SUCCESS);
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
Error unexpected_macro_error(const char* message, size_t line)
{
    std::string dest(strlen(message) + 38, '\0');
    sprintf(&dest[0], "parsing error : unexpected '%s' as macro", message);
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
