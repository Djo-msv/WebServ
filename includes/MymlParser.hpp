#pragma once

#include <vector>
#include <utility>
#include <sys/stat.h>
#include <dirent.h>
#include "ServerSocket.hpp"
#include "Tokenizer.hpp"
#include <map>
#include <fstream>
#include <string>
#include <stack>
#include "MymlObject.hpp"

class MymlParser
{
	public :
		MymlParser(char *path); /* Calling the constructor, verifying the file, and executing the process */
		~MymlParser(); /* Close _confFilefd */

		std::map<std::string, MymlObject>	getMyml(); // return vector of serverConfiguration */

	private :
		std::map<std::string, MymlObject>	_myml;
		std::vector<Tokenizer>				_tokens;

		void	addList();
		void	addDictionary();
		void	readFile(std::map<std::string, std::ifstream *>::iterator file);
		void	openFile(std::string path, std::map<std::string, std::ifstream *> &file);
		int		isDirectory(std::string path);

		void	buildTree();

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
