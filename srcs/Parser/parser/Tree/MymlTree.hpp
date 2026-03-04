#pragma once

#include <list>
#include <utility>

#include "MymlObject.hpp"
#include "../Lexer/Token.hpp"

class MymlTree
{
	public :
		MymlTree(std::list<Token> &tokens);
		~MymlTree();

	private :
		MymlList	_root;
		std::list<MymlObject *> _anchor;

		MymlObject *define(std::list<Token>::iterator &begin, const std::string &key, size_t prev_level);
		MymlObject *parseDictionary(const std::string &key, std::list<Token>::iterator &begin, size_t level);
		MymlObject *parseList(const std::string &key, std::list<Token>::iterator &begin, size_t level);
		std::pair<std::string, MymlObject*> parseDictionaryArg(std::list<Token>::iterator &begin, std::list<Token>::iterator &it, size_t level);
		MymlObject *parseListArg(std::list<Token>::iterator &begin, std::list<Token>::iterator &it, size_t level);

		class BadParsingError : public std::runtime_error {
			public :
				BadParsingError(const std::string msg) : std::runtime_error(msg) {}
		};
};

bool	isValue(std::list<Token>::iterator it);
bool	isDictionary(std::list<Token>::iterator it);
size_t	nbSpace(std::list<Token>::iterator &it);
