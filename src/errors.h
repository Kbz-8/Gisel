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

#ifndef __ERRORS__
#define __ERRORS__

#include <string>
#include <iostream>
#include "utils.h"

#ifdef _WIN32
#include <windows.h>
#endif 

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
Error unexpected_macro_error(const char* unexpected, size_t line);
Error expected_syntax_error(const char* expected, size_t line);
Error undeclared_error(const char* undeclared, size_t line);
Error wrong_type_error(const char* source, const char* destination, bool lvalue, size_t line);
Error already_declared_error(const char* name, size_t line);

Error file_not_found(const char* file);
Error file_not_found(const char* file, size_t line);

class runtime_error
{
    public:
        runtime_error(std::string message) noexcept { _message = std::move(message); }
        inline const char* what() const noexcept { return _message.c_str(); }
        inline void expose() const
        {
            #ifdef _WIN32
                HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
                SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
                std::cout << "Nir runtime error";
                SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
                std::cout << " : " << _message << std::endl;
            #else
                std::cout << OUT_RED << "Nir runtime error" << OUT_DEF << " : " << _message << std::endl;
            #endif
            std::exit(EXIT_SUCCESS);
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
