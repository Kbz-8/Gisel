/**
 * This file is a part of the Nir Interpreter
 *
 * Copyright (C) 2022 @kbz_8
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

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
