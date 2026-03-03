#prama once

#include <list>
#include <pair>

#include "MymlObject.hpp"
#include "Token.hpp"

class MymlTree
{
	public :
		MymlTree(std::list<Token> tokens);
		~MymlTree();

	private :
		MymlList	_root;
		std::list<MymlObject *> _anchor;

		MymlObject *MymlTree::define(std::list<Token>::iterator &begin, const std::string &key, size_t prev_level);
		MymlObject *MymlTree::parseDictionary(const std::string &key, std::list<Token>::iterator &begin, size_t level);
		MymlObject *MymlTree::parseList(const std::string &key, std::list<Token>::iterator &begin, size_t level);
		std::pair<std::string, MymlObject*> MymlTree::parseDictionaryArg(std::list<Token>::iterator &begin, std::list<Token>::iterator &it, size_t level);
		MymlObject *MymlTree::parseListArg(std::list<Token>::iterator &begin, std::list<Token>::iterator &it, size_t level);

		class BadParsingError : public std::runtime_error {
			public :
				BadParsingError(const std::string msg) : std::runtime_error(msg) {}
		};
};
