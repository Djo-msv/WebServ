#pragma once

#include <cstdlib>
#include <iostream>
#include <list>
#include <map>

#include "MymlObject.hpp"

class MymlList : public MymlObject
{
	public :
		MymlList(const ::std::string key);
		~MymlList();

		virtual void insert(MymlObject *value);

		std::string getKey();
		std::list<MymlObject*> *getList();

	private :
		std::string				_key;
		std::list<MymlObject*>	*_list;
};
