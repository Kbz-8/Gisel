#include "file.h"
#include "errors.h"

File::File(const char* path)
{
    _fp = fopen(path, "rt");
    if(!_fp)
        file_not_found(path).expose();
    _path = path;
}

File::~File()
{
    if(_fp)
        fclose(_fp);
}    
