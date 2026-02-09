#pragma once

#include <iostream>
#include <vector>
#include <map>

class MymlObject
{
	public :
		MymlObject();
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
		std::vector<MymlObject*>	getMymlList();

	private :
		std::vector<MymlObject*>	_list;
};

class MymlDictionary : public MymlObject
{
	public :
		MymlObject getMymlDictionary();
	
	private :
		std::map<std::string, MymlObject>	_dictionary;
};
