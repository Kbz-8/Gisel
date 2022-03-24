#ifndef __MESSAGES__
#define __MESSAGES__

#include <iostream>

class Message
{
    public:
        Message(const char* message) : _message(message) {}
        Message(std::string message) : _message(message) {}

        inline const char* what() const noexcept { return _message.c_str(); }
        void expose() const { std::cout << OUT_GREEN << "[Nir compiler] " << OUT_DEF  << _message << std::endl; }
    
    private:
        std::string _message;
};

#endif // __MESSAGES__
