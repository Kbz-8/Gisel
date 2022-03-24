#ifndef __TYPE__
#define __TYPE__

#include <variant>
#include <string>
#include <vector>
#include <set>

enum class simple_type
{
    nothing,
    number,
    string
};

struct function_type;

using type = std::variant<simple_type, function_type>;
using type_handle = const type*;

struct function_type
{
    struct param
    {
        type_handle type_id;
        bool by_ref;
    };
    type_handle return_type_id;
    std::vector<param> param_type_id;
};

class type_registry
{
    public:
        type_registry() = default;
        
        type_handle get_handle(const type& t);
        
        inline static type_handle get_void_handle() { return &void_type; }
        inline static type_handle get_number_handle() { return &number_type; }
        inline static type_handle get_string_handle() { return &string_type; }

    private:
        struct types_less { bool operator()(const type& t1, const type& t2) const; };
        std::set<type, types_less> _types;
        
        static type void_type;
        static type number_type;
        static type string_type;
};

namespace std { std::string to_string(type_handle t); }

#endif // __TYPE__
