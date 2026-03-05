#include "MymlDictionary.hpp"

MymlDictionary::MymlDictionary(const std::string &key) : _key(key)
{}

MymlDictionary::~MymlDictionary(void)
{
	for (std::map<std::string, MymlObject*>::iterator it = _dictionary.begin(); it != _dictionary.end(); it++)
		delete it->second;
}

void	MymlDictionary::insert(const std::pair<std::string, MymlObject *> value)
{
	_dictionary.insert(_dictionary.end(), value);
}

std::map<std::string, MymlObject*>	MymlDictionary::getDictionary(void)
{
	return (_dictionary);
}
