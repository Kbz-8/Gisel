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

#ifndef __SINGLETON__
#define __SINGLETON__


template <typename T>
class Singleton
{
	public:
		Singleton(const Singleton&) = delete;
		Singleton(Singleton&&) noexcept = default;
		Singleton &operator=(const Singleton&) = delete;
		Singleton &operator=(Singleton&&) noexcept = default;

		inline static T& get()
		{
			static T instance;
			return instance;
		}

	protected:
		Singleton() = default;
		virtual ~Singleton() = default;
};

#endif // __SINGLETON__
