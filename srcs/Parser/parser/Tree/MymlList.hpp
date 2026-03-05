#pragma once

#include "MymlObject.hpp"

class MymlList : public MymlObject
{
	public :
		MymlList(const ::std::string &key);
		~MymlList();

		virtual void insert(MymlObject *value);

		std::list<MymlObject*> getList();

	private :
		std::string				_key;
		std::list<MymlObject*>	_list;
};
