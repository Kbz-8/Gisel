#include "warnings.h"
#include <string.h>

Warning::Warning(const char* message, size_t line)
{
    _message = message;
    _line = line + 1;
}
Warning::Warning(std::string message, size_t line)
{
    _message = std::move(message);
    _line = line + 1;
}

Warning no_end(const char* message, size_t line)
{
    std::string dest(strlen(message) + 55, '\0');
    sprintf(&dest[0], "Nir warning : expected closing %s, but none was found", message);
    return Warning(std::move(dest), line);
}
