#pragma once

#include "ServerSocket.hpp"

class MymlObject
{
	public :
		MymlObject();
		~MymlObject();

		int	getAsInt();
		std::string getAsString();

		std::vector<MymlObject> getAsList();
		std::map<MymlObject> getAsDictionary();
		
		const std::string getKey();
	private :
		std::string	value;
		std::string key;
		
		// Error class
		class WrongType : public std::runtime_error {
			public :
				WrongType() : std::runtime_error() {}
		};
}

class MymlList : public MymlObject
{
	public :
		std::vector<MymlObject>	getElements();

	private :
		std::vector<MymlObject>	list;
}

class MymlDictionary : public MymlObject
{
	public
		MymlObject getMymlObject();
	
	private :
		std::map<std::string, MymlObject>	dictionary;
}

class MymlParser
{
	public :
		MymlParser(char *path); /* Calling the constructor, verifying the file, and executing the process */
		~MymlParser(); /* Close _confFilefd */

		std::map<std::string, MymlObject>	getMyml(); // return vector of serverConfiguration */

	private :
		std::ifstream		*_file;
		std::map<std::string, MymlObject>	_myml;

		void	addList();
		void	addDictionary();

		// Error class
		class WrongPerm : public std::runtime_error {
			public :
				WrongPerm(const std::string msg) : std::runtime_error(msg) {}
		};
		class NotAnFile : public std::runtime_error {
			public :
				NotAnFile(const std::string msg) : std::runtime_error(msg) {}
		};
		class BadParsing : public std::runtime_error {
			public :
				BadParsing(const std::string msg) : std::runtime_error(msg) {}
		};
};
