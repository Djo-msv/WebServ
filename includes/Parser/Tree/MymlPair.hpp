#pragma once

#include <cstdlib>
#include <iostream>
#include <list>
#include <map>

#include "MymlObject.hpp"

class MymlPair :public MymlObject
{
	public :
		MymlPair(std::string &key, std::string &value);
		MymlPair(std::string &key, MymlObject *value);
		~MymlPair();

		std::string getKey();
		MymlObject	*getValue();
		int 		getKeyAsInt();

	private :
		std::pair<std::string, MymlObject*>	_pair;
};
