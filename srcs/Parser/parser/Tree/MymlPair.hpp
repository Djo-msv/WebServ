#pragma once

#include "MymlObject.hpp"

class MymlPair :public MymlObject
{
	public :
		MymlPair(std::string &key, std::string &value);
		~MymlPair();

		std::pair<std::string, std::string>	getMymlPair();

		std::string getKey();

	private :
		std::pair<std::string, std::string>	_pair;
};
