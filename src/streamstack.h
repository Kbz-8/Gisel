#ifndef __STREAM_STACK__
#define __STREAM_STACK__

#include <vector>
#include <string>
#include <stack>
#include "function.h"

using get_character = func::function<int()>;

class StreamStack
{
    public:
        explicit StreamStack(const func::function<int()>* input);
        
        int operator()();
        
        void push_back(int c);
        
        inline size_t getline() const noexcept { return _line; }

        ~StreamStack() = default;

    private:
        const func::function<int()>& _input;
        std::stack<int> _stack;
        size_t _line;
};

#endif // __STREAM_STACK__