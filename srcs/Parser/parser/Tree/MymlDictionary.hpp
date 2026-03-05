#pragma once

#include "MymlObject.hpp"

class MymlDictionary : public MymlObject
{
	public :
		MymlDictionary(const std::string &key);
		~MymlDictionary();

		std::map<std::string, MymlObject*> getMymlDictionary();

		virtual void insert(const std::pair<std::string, MymlObject *> value);

		std::map<std::string, MymlObject*> getDictionary();
	
	private :
		std::string							_key;
		std::map<std::string, MymlObject*>	_dictionary;
};
