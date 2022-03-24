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
