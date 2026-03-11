#pragma once

#include <cstdlib>
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

		std::string	getAsString();
		int	getAsInt();
		double getAsFloat();
		bool getAsBool();

	private :
		std::string	_value;
		
		// Error class
		class BadCast : public std::runtime_error {
			public :
				BadCast(const std::string msg) : std::runtime_error(msg) {}
		};
};
