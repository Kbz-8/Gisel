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

#ifndef __FILE_TYPE__
#define __FILE_TYPE__

#include <string>
#include <cstdio>

class File
{
    public:
        explicit File(const char* path);
        File(const File&) = delete;

        void operator=(const File&) = delete;
        inline int operator()() const { return fgetc(_fp); }
        inline bool is_eof() const { return feof(_fp); }
        inline std::string& get_path() { return _path; }
        
        ~File();

    private:
        FILE* _fp;
        std::string _path;
};

#endif // __FILE_TYPE__
