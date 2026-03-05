#include "MymlList.hpp"

MymlList::MymlList(const std::string &key) : _key(key)
{}

MymlList::~MymlList(void)
{
	for (std::list<MymlObject*>::iterator it = _list.begin(); it != _list.end(); it++)
		delete (*it);
}

void	MymlList::insert(MymlObject *value)
{
	_list.insert(_list.end(), value);
}

std::list<MymlObject*>	MymlList::getList(void)
{
	return (_list);
}
