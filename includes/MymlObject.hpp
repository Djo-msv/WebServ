#pragma once

#include <iostream>
#include <vector>
#include <map>

class MymlObject
{
	public :
		MymlObject();
		~MymlObject();

		int	getAsInt();
		std::string getAsString();

		std::vector<MymlObject> getAsList();
		std::map<std::string, MymlObject> getAsDictionary();
		
		const std::string getKey();
	private :
		std::string	value;
		std::string key;
		
		// Error class
		class WrongType : public std::runtime_error {
			public :
				WrongType(const std::string msg) : std::runtime_error(msg) {}
		};
};

class MymlList : public MymlObject
{
	public :
		std::vector<MymlObject>	getElements();

	private :
		std::vector<MymlObject>	list;
};

class MymlDictionary : public MymlObject
{
	public :
		MymlObject getMymlObject();
	
	private :
		std::map<std::string, MymlObject>	dictionary;
};


