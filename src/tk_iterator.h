#ifndef __TOKEN_ITERATOR__
#define __TOKEN_ITERATOR__

#include "tokens.h"
#include "streamstack.h"
#include <deque>

class tk_iterator
{
    public:
        tk_iterator(std::deque<Token>& tokens);
        tk_iterator(StreamStack& tokens);

        inline const Token& operator*() const noexcept { return _current; }
        inline const Token* operator->() const noexcept { return &_current; }
        inline tk_iterator& operator++() { _current = _get_next_token(); return *this; }
        inline tk_iterator operator++(int)
        {
            tk_iterator old = *this;
            operator++();
            return old;
        }
        inline bool operator()() const noexcept { return !_current.is_eof(); }

    private:
        Token _current;
        func::function<Token()> _get_next_token;
};

#endif // __TOKEN_ITERATOR__
