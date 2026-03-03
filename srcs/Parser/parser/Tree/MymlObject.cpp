#include "MymlObject.hpp"

MymlObject::MymlObject(void)
{}

// for value string only, canot stock any list or dictionary
MymlObject::MymlObject(const std::string &value) : _value(value)
{}

MymlObject::~MymlObject(void)
{}

MymlObject::~MymlObject(const std::string &value) : _value(value)
{}

MymlList::MymlList(const std::string &key) : _key(key)
{}

MymlList::~MymlList(void)
{
	for (std::list<MymlObject*>::iterator it = _list.begin(); it != _list.end(); it++)
		delete (*it);
}

MymlPair::MymlPair(std::string &key, std::string &value) :: _pair(std::pair<std::string, std::string>(key, value))
{}

MymlPair::~MymlPair(void)
{}

MymlDictionary::MymlDictionary(const std::string &key) : _key(key)
{}

MymlDictionary::~MymlDictionary(void)
{
	for (std::map<std::string, MymlObject*>::iterator it = _dictionary.begin(); it != _dictionary.end(); it++)
		delete it->second;
}

void	MymlList::insert(const MymlObject *value)
{
	_list.insert(_list.end(), value);
}


void	MymlDictionary::insert(const std::pair<std::string, MymlObject *> value)
{
	_dictionary.insert(_dictonary.end(), value);
}
