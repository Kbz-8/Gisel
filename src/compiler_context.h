#ifndef __COMPILER_CONTEXT__
#define __COMPILER_CONTEXT__

#include <unordered_map>
#include <memory>
#include <string>

#include "type.h"

enum struct identifier_scope
{
    global_variable,
    local_variable,
    function,
};

class identifier_info
{
    public:
        identifier_info(type_handle type_id, size_t index, identifier_scope scope);
        
        inline type_handle type_id() const noexcept { return _type_id; }
        inline size_t index() const noexcept { return _index; }
        inline identifier_scope get_scope() const { return _scope; }
        
    private:
        type_handle _type_id;
        size_t _index;
        identifier_scope _scope;
};

class identifier_lookup
{
    public:
        inline virtual const identifier_info* find(const std::string& name) const
        {
            if(auto it = _identifiers.find(name); it != _identifiers.end())
                return &it->second;
            return nullptr;
        }
        virtual const identifier_info* create_identifier(std::string name, type_handle type_id) = 0;
        inline bool can_declare(const std::string& name) const { return _identifiers.find(name) == _identifiers.end(); }

        virtual ~identifier_lookup() = default;
    
    protected:
        inline const identifier_info* insert_identifier(std::string name, type_handle type_id, size_t index, identifier_scope scope) { return &_identifiers.emplace(std::move(name), identifier_info(type_id, index, scope)).first->second; }
        inline size_t identifiers_size() const noexcept { return _identifiers.size(); }

    private:
        std::unordered_map<std::string, identifier_info> _identifiers;
};

class global_variable_lookup: public identifier_lookup
{
    public:
        inline const identifier_info* create_identifier(std::string name, type_handle type_id) override { return insert_identifier(std::move(name), type_id, identifiers_size(), identifier_scope::global_variable); }
};

class local_variable_lookup: public identifier_lookup
{
    public:
        local_variable_lookup(std::unique_ptr<local_variable_lookup> parent_lookup);
        const identifier_info* find(const std::string& name) const override;
        const identifier_info* create_identifier(std::string name, type_handle type_id) override;
        std::unique_ptr<local_variable_lookup> detach_parent();

    private:
        std::unique_ptr<local_variable_lookup> _parent;
        int _next_identifier_index;
};

class param_lookup: public local_variable_lookup
{
    public:
        param_lookup();
        const identifier_info* create_param(std::string name, type_handle type_id);

    private:
        int _next_param_index;
};

class function_lookup: public identifier_lookup
{
    public:
        const identifier_info* create_identifier(std::string name, type_handle type_id) override;
};

class compiler_context
{
    class scope_raii
    {
        public:
            scope_raii(compiler_context& context);
            ~scope_raii();
        private:
            compiler_context& _context;
    };
    
    class function_raii
    {
        public:
            function_raii(compiler_context& context);
            ~function_raii();
        private:
            compiler_context& _context;
    };
    
    public:
        compiler_context();
        type_handle get_handle(const type& t);
        const identifier_info* find(const std::string& name) const;
        const identifier_info* create_identifier(std::string name, type_handle type_id);
        const identifier_info* create_param(std::string name, type_handle type_id);
        const identifier_info* create_function(std::string name, type_handle type_id);
        bool can_declare(const std::string& name) const;
        scope_raii scope();
        function_raii function();

    private:
        function_lookup _functions;
        global_variable_lookup _globals;
        param_lookup* _params;
        std::unique_ptr<local_variable_lookup> _locals;
        type_registry _types;
        
        
        void enter_function();
        void enter_scope();
        void leave_scope();
};

#endif // __COMPILER_CONTEXT__
