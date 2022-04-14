#ifndef __MACROS__
#define __MACROS__

#include <vector>
#include <unordered_map>
#include <string>

#include "singleton.h"

class Macros : public Singleton<Macros>
{
    public:
        inline void new_set(const std::string& source, const std::string& dest) { _sets[source] = dest; }
        inline void remove_set(const std::string& set) { _sets.erase(set); }
        inline std::unordered_map<std::string, std::string>& get_sets() { return _sets; }

    private:
        std::unordered_map<std::string, std::string> _sets;
};

#endif // __MACROS__
