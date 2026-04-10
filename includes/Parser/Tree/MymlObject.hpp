#pragma once

#include <stdexcept>

class MymlList;
class MymlPair;
class MymlDictionary;


class MymlObject // class mère
{
	public :
		MymlObject();
		MymlObject(const std::string value);
		virtual ~MymlObject();

		virtual void insert(MymlObject *value);
		virtual void insert(const std::pair<std::string, MymlObject *> value);

		std::string	getAsString();
		int		getAsInt();
		ssize_t getAsLong();
		double 	getAsFloat();
		bool 	getAsBool();

		bool	isList();
		bool	isDictionnary();
		bool	isPair();

		MymlList 		*getAsList();
		MymlDictionary	*getAsDictionnary();
		MymlPair		*getAsPair();

		// Error class
		class BadCast : public std::runtime_error {
			public :
				BadCast(const std::string msg) : runtime_error(msg) {}
		};
	private :
		std::string	_value;
};
