#pragma once

#include <iostream>
#include <list>
#include <map>

class MymlObject
{
	public :
		MymlObject();
		MymlObject(const std::string &value);
		virtual ~MymlObject();

		virtual void insert(MymlObject *value);
		virtual void insert(const std::pair<std::string, MymlObject *> value);
	private :
		std::string	_value;
		
		// Error class
		class WrongType : public std::runtime_error {
			public :
				WrongType(const std::string msg) : std::runtime_error(msg) {}
		};
};

class MymlPair :public MymlObject
{
	public :
		MymlPair(std::string &key, std::string &value);
		~MymlPair();

		std::pair<std::string, std::string>	getMymlPair();

	private :
		std::pair<std::string, std::string>	_pair;
};

class MymlList : public MymlObject
{
	public :
		MymlList(const ::std::string &key);
		~MymlList();

		virtual void insert(MymlObject *value);
	private :
		std::string				_key;
		std::list<MymlObject*>	_list;
};

class MymlDictionary : public MymlObject
{
	public :
		MymlDictionary(const std::string &key);
		~MymlDictionary();

		std::map<std::string, MymlObject*> getMymlDictionary();

		virtual void insert(const std::pair<std::string, MymlObject *> value);

	
	private :
		std::string							_key;
		std::map<std::string, MymlObject*>	_dictionary;
};
