#include "Parser/Tree/MymlPair.hpp"


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

MymlObject *MymlPair::getValue(void)
{
	return (_pair.second);
}
int	MymlPair::getKeyAsInt(void)
{
	std::string::iterator it = _pair.first.begin();

	if (*it == '-')
		it++;
	for (; it != getKey().end(); it++) {
		if (!*it) { break; }
		if (!isdigit(*it))
			throw BadCast("the value [" + getKey() + "] is not an integer !");
	}
	return (std::atoi(getKey().c_str()));
}
