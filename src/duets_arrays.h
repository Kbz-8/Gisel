/**
 * This file is a part of the Gisel Interpreter
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

#ifndef __DUETS_ARRAY__
#define __DUETS_ARRAY__

#include <vector>
#include <iostream>
#include <map>
#include <utility>
#include <cassert>

namespace Gisel
{
	template <class T>
	struct default_t // default type class
	{
		static T get() { return T(); }
	};

	template <typename __first, typename __second>
	class duets_array
	{
		using __type = std::pair<__first, __second>;

		public:
			using iterator = typename std::vector<__type>::const_iterator;
			using const_iterator = iterator;

			duets_array(std::initializer_list<__type> duets)
			{
				assert(duets.size() <= _array.max_size());
				_array = std::move(duets);
			}

			duets_array& operator= (const duets_array&& array) noexcept
			{
				_array = std::move(array);
				return *this;
			}

			duets_array& set_duet(const __first&& first, const __second&& second) noexcept
			{
				_array.push_back(__type(std::move(first), std::move(second)));
				return *this;
			}

			constexpr const __second& operator[](__first duet) noexcept
			{
				for(int i = 0; i < _array.size(); i++)
				{
					if(_array[i].first == duet)
						return _array[i].second;
				}
				return _sDefault;
			}
			
			constexpr const __first& operator[](__second duet) noexcept
			{
				for(int i = 0; i < _array.size(); i++)
				{
					if(_array[i].second == duet)
						return _array[i].first;
				}
				return _fDefault;
			}
			constexpr const __type& operator[](int index) noexcept { return index < _array.size() ? _array[index] : _dDefault; }
			constexpr const __second& at(__first duet) noexcept { return this[duet]; }
			constexpr const __first& at(__second duet) noexcept { return this[duet]; }
			constexpr std::size_t size() noexcept { return _array.size(); }
			constexpr bool empty() noexcept { return size() == 0; }
			constexpr std::size_t max_size() noexcept { return _array.max_size(); }

			bool erase(__first& duet) noexcept // if object cannot erase its duet, function return false
			{
				for(int i = 0; i < _array.size(); i++)
				{
					if(_array[i].first == duet)
					{
						_array.erase(_array.begin() + i);
						return true;
					}
				}
				return false;
			}
			bool erase(__second& duet) noexcept // if object cannot erase its duet, function return false
			{
				for(int i = 0; i < _array.size(); i++)
				{
					if(_array[i].second == duet)
					{
						_array.erase(_array.begin() + i);
						return true;
					}
				}
				return false;
			}
			inline void clear() noexcept { _array.clear(); }
			bool have(const __first& duet)
			{
				for(int i = 0; i < _array.size(); i++)
				{
					if(_array[i].first == duet)
						return true;
				}
				return false;
			}
			bool have(const __second& duet)
			{
				for(int i = 0; i < _array.size(); i++)
				{
					if(_array[i].second == duet)
						return true;
				}
				return false;
			}

			inline iterator begin() { return _array.begin(); }
			inline iterator end() { return _array.end(); }
			inline const_iterator cbegin() const { return _array.cbegin(); }
			inline const_iterator cend() const { return _array.cend(); }

			friend std::ostream& operator<<(std::ostream& target, const duets_array& source)
			{
				for(int i = 0; i < source.size(); i++)
					target << source[i].first << "	" << source[i].second << std::endl;
				return target;
			}

		private:
			std::vector<__type> _array;
			const __first _fDefault = default_t<__first>::get();
			const __second _sDefault = default_t<__second>::get();
			const __type _dDefault = default_t<__type>::get();
	};
}

#endif // __DUETS_ARRAY__
