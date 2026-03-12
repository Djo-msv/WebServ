#pragma once

#include <stdexcept>

#include "MymlList.hpp"
#include "MymlPair.hpp"
#include "MymlDictionary.hpp"


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

		bool	isList();
		bool	isDictionnary();

		MymlList 		*getAsList();
		MymlDictionary	*getAsDictionnary();

	private :
		std::string	_value;
		
		// Error class
		class BadCast : public std::bad_cast {
			public :
				BadCast(const std::string msg) : msg(msg) {}
				const char* what() { return (msg.c_str()); }
			private :
				const std::string msg;
		};
};
