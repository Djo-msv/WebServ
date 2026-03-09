#pragma once

#include <iostream>
#include <list>
#include <map>

class MymlObject // class mère
{
	public :
		MymlObject();
		MymlObject(const std::string value);
		virtual ~MymlObject();

		virtual void insert(MymlObject *value);
		virtual void insert(const std::pair<std::string, MymlObject *> value);

		std::string	getValue();
	private :
		std::string	_value;
		
		// Error class
		class WrongType : public std::runtime_error {
			public :
				WrongType(const std::string msg) : std::runtime_error(msg) {}
		};
};
