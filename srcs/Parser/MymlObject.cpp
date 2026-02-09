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

MymlList::MymlList(MymlObject object) : MymlObject()
{}

MymlList::~MymlList(void)
{}

MymlDictionary::MymlDictionary(MymlObject &object) : MymlObject()
{}

MymlDictionary::~MymlDictionary(void)
{}

std::vector	MymlList::getList(void)
{
	return (_list);
}

std::map MymlDictionary::getDictionary(void)
{
	return (_dictionary);
}

MymlObject	*MymlList::insertList()
{
	MymlList *obj = new MymlList()
	_list.push_back(obj);
	return (obj);
}

MymlObject	*MymlList::insertDictionary()
{
	MymlDictionary *obj = new MymlDirectory();
	_list.push_back(obj);
	return (obj);
}

MymlObject	*MymlDictionary::insertList(std::string &value)
{
	MymlList *obj = new MymlList();
	_dictionary.insert(std::pair<std::string, MymlObject*>(value, obj));
	return (obj);
}

MymlObject	*MymlDictionary::insertDictionary(std::string &value)
{
	MymlDictionary *obj - new MymlDictionary();
	_dictionary.insert(std::pair<std::string, MymlObject*>(value, obj));
	return (obj);
}
