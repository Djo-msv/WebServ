#include "MymlPair.hpp"

MymlPair::MymlPair(std::string &key, std::string &value) : _pair(std::pair<std::string, std::string>(key, value))
{}

MymlPair::~MymlPair(void)
{}

std::string	MymlPair::getKey(void)
{
	return (_pair.first);
}
