#include "MymlObject.hpp"

MymlObject::MymlObject(void)
{}

// for value string only, canot stock any list or dictionary
MymlObject::MymlObject(const std::string &value) : _value(value)
{}

MymlObject::~MymlObject(void)
{}

std::string	MymlObject::getValue(void)
{
	return (_value);
}

MymlList::MymlList(void)
{}

MymlList::~MymlList(void)
{
	for (std::vector<MymlObject*>::iterator it = _list.begin(); it != _list.end(); it++)
		delete (*it);
}

MymlDictionary::MymlDictionary(void)
{}

MymlDictionary::~MymlDictionary(void)
{
	for (std::map<std::string, MymlObject*>::iterator it = _dictionary.begin(); it != _dictionary.end(); it++)
		delete (*it).second;
}

std::vector<MymlObject*>	MymlList::getMymlList(void)
{
	return (_list);
}

std::map<std::string, MymlObject*> MymlDictionary::getMymlDictionary(void)
{
	return (_dictionary);
}

MymlObject	*MymlList::insertList()
{
	MymlList *obj = new MymlList();
	_list.push_back(obj);
	return (obj);
}

MymlObject	*MymlList::insertDictionary()
{
	MymlDictionary *obj = new MymlDictionary();
	_list.push_back(obj);
	return (obj);
}

void	MymlList::insertValue(std::string &value)
{
	MymlObject *obj = new MymlObject(value);
	_list.push_back(obj);
}

MymlObject	*MymlDictionary::insertList(std::string &key)
{
	MymlList *obj = new MymlList();
	_dictionary.insert(std::pair<std::string, MymlObject*>(key, obj));
	return (obj);
}

MymlObject	*MymlDictionary::insertDictionary(std::string &key)
{
	MymlDictionary *obj = new MymlDictionary();
	_dictionary.insert(std::pair<std::string, MymlObject*>(key, obj));
	return (obj);
}

void	MymlDictionary::insertValue(std::string &key, std::string &value)
{
	MymlObject *obj = new MymlObject(value);
	_dictionary.insert(std::pair<std::string, MymlObject*>(key, obj));
}
