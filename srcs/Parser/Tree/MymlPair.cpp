#include "MymlPair.hpp"

MymlPair::MymlPair(std::string &key, std::string &value)
{
	_pair.first = key;
	_pair.second = new MymlObject(value);
}

MymlPair::MymlPair(std::string &key, MymlObject *value)
{
	_pair.first = key;
	_pair.second = value;
}

MymlPair::~MymlPair(void)
{
	delete _pair.second;
}

std::string	MymlPair::getKey(void)
{
	return (_pair.first);
}
