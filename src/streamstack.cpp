#include "streamstack.h"

StreamStack::StreamStack(const func::function<int()>* input) : _input(*input), _line(0) {}
	
int StreamStack::operator()()
{
	int ret = -1;

	if(_stack.empty())
		ret = _input();
    else
    {
		ret = _stack.top();
		_stack.pop();
	}

	if(ret == '\n')
		_line++;
	
	return ret;
}

void StreamStack::push_back(int c) 
{
	_stack.push(c);
	
	if(c == '\n')
		_line--;
}
