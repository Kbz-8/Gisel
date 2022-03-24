#include "variable.h"

template<typename T>
variable_impl<T>::variable_impl(T value) : value(std::move(value)) {}

template class variable_impl<number>;
template class variable_impl<string>;
template class variable_impl<function>;

template<typename T>
string variable_impl<T>::to_string() const { return convert_to_string(value); }
