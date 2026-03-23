#include "Parser/Tree/MymlList.hpp"

MymlList::MymlList(const std::string key) : _key(key)
{
	_list = new std::list<MymlObject*>();
}

MymlList::~MymlList(void)
{
	for (std::list<MymlObject*>::iterator it = _list->begin(); it != _list->end(); it++)
		delete (*it);
	delete _list;
}

void	MymlList::insert(MymlObject *value)
{
	_list->insert(_list->end(), value);
}

std::string	MymlList::getKey(void)
{
	return (_key);
}

std::list<MymlObject*>	*MymlList::getList(void)
{
	return (_list);
}

std::list<MymlObject *>::iterator MymlList::begin()
{
	return (_list->begin());
}

std::list<MymlObject *>::iterator MymlList::end()
{
	return (_list->end());
}
