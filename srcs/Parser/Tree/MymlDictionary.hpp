#pragma once

#include <cstdlib>
#include <iostream>
#include <list>
#include <map>

class MymlObject;

class MymlDictionary : public MymlObject
{
	public :
		MymlDictionary(const std::string key);
		~MymlDictionary();

		std::map<std::string, MymlObject*> getMymlDictionary();

		virtual void insert(const std::pair<std::string, MymlObject *> value);

		std::string getKey();
		std::map<std::string, MymlObject*> getDictionary();
	
		MymlObject *getValue(std::string key);
		std::string getValueAsString(std::string key);
		int getValueAsInt(std::string key);
		double getValueAsFloat(std::string key);

	private :
		std::string							_key;
		std::map<std::string, MymlObject*>	_dictionary;
};
