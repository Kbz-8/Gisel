#ifndef __STD_FUNCTIONS__
#define __STD_FUNCTIONS__

#include <cstdint>

class module;
	
void add_math_functions(module& m);
void add_string_functions(module& m);
void add_trace_functions(module& m, uint8_t opt);

void add_standard_functions(module& m, uint8_t opt);

#endif // __STD_FUNCTIONS__
