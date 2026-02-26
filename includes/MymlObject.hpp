#pragma once

#include <iostream>
#include <vector>
#include <map>

class MymlObject
{
	public :
		MymlObject();
		MymlObject(const std::string &value);
		virtual ~MymlObject();

		std::string getValue();
		
	private :
		std::string	_value;
		
		// Error class
		class WrongType : public std::runtime_error {
			public :
				WrongType(const std::string msg) : std::runtime_error(msg) {}
		};
};

class MymlList : public MymlObject
{
	public :
		MymlList();
		~MymlList();

		std::vector<MymlObject*>	getMymlList();

		MymlObject *insertList();
		MymlObject *insertDictionary();
		void insertValue(std::string &value);

	private :
		std::vector<MymlObject*>	_list;
};

class MymlDictionary : public MymlObject
{
	public :
		MymlDictionary();
		~MymlDictionary();

		std::map<std::string, MymlObject*> getMymlDictionary();

		MymlObject *insertList(std::string &key);
		MymlObject *insertDictionary(std::string &key);
		void insertValue(std::string &key, std::string &value);

	
	private :
		std::map<std::string, MymlObject*>	_dictionary;
};
