#pragma once

#include <cstdlib>
#include <iostream>
#include <list>
#include <map>

#include "MymlObject.hpp"
#include "MymlList.hpp"

class MymlDictionary : public MymlObject
{
	public :
		MymlDictionary(const std::string key);
		~MymlDictionary();

		virtual void insert(const std::pair<std::string, MymlObject *> value);

		std::string getKey();
		std::map<std::string, MymlObject*> *getDictionary();
	
		std::string getValueAsString(std::string key);
		int getValueAsInt(std::string key);
		ssize_t getValueAsLong(std::string key);
		double getValueAsFloat(std::string key);

		MymlObject *getValue(std::string key);
		MymlList *getValueAsList(std::string key);
		MymlDictionary *getValueAsDictionary(std::string key);

		bool	has(std::string key);
		

	private :
		std::string							_key;
		std::map<std::string, MymlObject*>	*_dictionary;
};
