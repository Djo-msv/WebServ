#pragma once

#include "MymlObject.hpp"

class MymlPair :public MymlObject
{
	public :
		MymlPair(std::string &key, std::string &value);
		MymlPair(std::string &key, MymlObject *value);
		~MymlPair();

		std::pair<std::string, MymlObject>	getMymlPair();

		std::string getKey();

	private :
		std::pair<std::string, MymlObject*>	_pair;
};
