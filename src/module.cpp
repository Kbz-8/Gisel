#include <vector>
#include <cstdio>
#include "module.h"
#include "errors.h"
#include "streamstack.h"
#include "lexer.h"
#include "compiler.h"
#include "file.h"
#include "tk_iterator.h"

class module_impl
{
    public:
        module_impl() {}
        
        inline runtime_context* get_runtime_context() { return _context.get(); }
        
        inline void add_public_function_declaration(std::string declaration, std::string name, std::shared_ptr<function> fptr)
        {
            _public_declarations.push_back(std::move(declaration));
            _public_functions.emplace(std::move(name), std::move(fptr));
        }
        
        inline void add_external_function_impl(std::string declaration, function f) { _external_functions.emplace_back(std::move(declaration), std::move(f)); }
        
        void load(const char* path)
        {
            File f(path);
            get_character get = [&](){ return f(); };
            StreamStack stream(&get);
            
            tk_iterator it(stream);
            
            _context = std::make_unique<runtime_context>(compile(it, _external_functions, _public_declarations));
            
            for(const auto& p : _public_functions)
                *p.second = _context->get_public_function(p.first.c_str());
        }
        
        inline bool try_load(const char* path, std::ostream* err) noexcept { load(path); return true; }
        
        inline void reset_globals() { if(_context) _context->initialize(); }

    private:
        std::vector<std::pair<std::string, function> > _external_functions;
        std::vector<std::string> _public_declarations;
        std::unordered_map<std::string, std::shared_ptr<function> > _public_functions;
        std::unique_ptr<runtime_context> _context;
};

module::module() : _impl(std::make_unique<module_impl>()) {}

runtime_context* module::get_runtime_context() { return _impl->get_runtime_context(); }
void module::add_external_function_impl(std::string declaration, function f) { _impl->add_external_function_impl(std::move(declaration), std::move(f)); }
void module::add_public_function_declaration(std::string declaration, std::string name, std::shared_ptr<function> fptr) { _impl->add_public_function_declaration(std::move(declaration), std::move(name), std::move(fptr)); }
void module::load(const char* path) { _impl->load(path); }
bool module::try_load(const char* path, std::ostream* err) noexcept { return _impl->try_load(path, err); }
void module::reset_globals() { _impl->reset_globals(); }

module::~module() {}
