#ifndef __WARNINGS__
#define __WARNINGS__

#include <string>
#include <iostream>
#include "utils.h"

class Warning
{
    public:
        Warning(const char* message, size_t line);
        Warning(std::string message, size_t line);

        inline const char* what() const noexcept { return _message.c_str(); }
        inline void expose() const { std::cout << OUT_MAGENTA << "Nir warning : " << OUT_DEF << _message << ", line : " << _line << std::endl; }
        inline int line() const noexcept { return _line; }
    
    private:
        std::string _message;
        int _line = 0;
};

Warning no_end(const char* message, size_t line);

#endif // __WARNINGS__
