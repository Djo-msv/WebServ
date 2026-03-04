#include "MymlTree.hpp"

MymlTree::MymlTree(std::list<Token> &tokens) : _root("root")
{
	std::string key;
	std::string value;

	for (std::list<Token>::iterator it = tokens.begin(); it != tokens.end(); it++) {
		if (it->_type == STRING) {
			it++;
			key = it->_token;
			if (it->_type == COLON) {
				it++;
				if (isValue(it)) { // is pair
					value = it->_token;
					for(;it->_type != INDENTATION && it->_type == END_OF_LINE; it++){};
					std::cout << "pair" << std::endl;
					_root.insert(new MymlPair(key, value));	
				}
				else { // is list or dictionary
					for(;it->_type != INDENTATION && it->_type == END_OF_LINE; it++){};
					_root.insert(define(it, key, -1));
				}
			}
		}
	}
}

MymlTree::~MymlTree(void){}

/**
 * *Takes an line of token and check if it's an define or a value
 * return an Object of the define or value
**/
MymlObject *MymlTree::parseListArg(std::list<Token>::iterator &begin, std::list<Token>::iterator &it, size_t level)
{
	std::string	key;
	std::string value;

	if (it->_type == DASH)
		it++;
	else 
		throw BadParsingError(it->_token);
	key = it->_token;
	it++;
	// is define or pair
	if (it->_type == COLON) {
		it++;
		if (isValue(it)) { // is pair
			value = it->_token;
			for(;it->_type != INDENTATION && it->_type == END_OF_LINE; it++){};
			std::cout << std::string(level, ' ') << "pair" << std::endl;
			return (new MymlPair(key, value));	
		}
		else { // is list or dictionary
			for(;begin->_type != INDENTATION && begin->_type == END_OF_LINE; begin++){};
			return (define(begin, key, level));
		}
	}
	// is value
	for(;begin->_type != INDENTATION && begin->_type == END_OF_LINE; begin++){};
	std::cout << std::string(level, ' ') << "value" << std::endl;
	return (new MymlObject(key)); // !
}

/**
 * *Takes an line of token and check if it's an define or a dictionary value
 * return an pair of value for the dictionary
**/
std::pair<std::string, MymlObject*> MymlTree::parseDictionaryArg(std::list<Token>::iterator &begin, std::list<Token>::iterator &it, size_t level)
{
	std::string	key;
	std::string value;
	std::pair<std::string, MymlObject*>	obj;

	if (!isValue(it))
		throw BadParsingError(it->_token);
	key = it->_token;
	it++;
	// is define or pair
	if (it->_type == COLON) {
		it++;
		if (isValue(it)) { // is dictionary value
			value = it->_token;
			for(;it->_type != INDENTATION && it->_type == END_OF_LINE; it++){};
			std::cout << std::string(level, ' ') << "value" << std::endl;
			obj = std::pair<std::string, MymlObject*>(key, new MymlObject(value));
			return (obj);
		}
		else if (it->_type == END_OF_LINE) { // is list or dictionary
			for(;begin->_type != INDENTATION && begin->_type == END_OF_LINE; begin++){};
			std::cout << std::string(level, ' ') << "list" << std::endl;
			obj = std::pair<std::string, MymlObject*>(key, define(begin, key, level));
			return (obj);
		}
	}
	throw BadParsingError(it->_token);
	return (std::pair<std::string, MymlObject*>());
}

/**
 * *Takes the key (name of the define) and create an list
 * browses list of token until the indentation level is not the
 * same than the first line and insert the value to the list
**/
MymlObject *MymlTree::parseList(const std::string &key, std::list<Token>::iterator &begin, size_t level)
{
	std::cout << std::string(level, ' ') << "list" << std::endl;
	MymlObject *list = new MymlList(key);
	std::list<Token>::iterator	it = begin;

	while (level == nbSpace(it)) {
		if (isDictionary(it)) {}; // throw error
		list->insert(parseListArg(begin, it, level));
		begin = it;
	}
	return (list);
}

/**
 * *Takes the key (name of the define) and create an dictionary
 * browses list of token until the indentation level is not the
 * same than the first line and insert the value to the dictionary
**/
MymlObject *MymlTree::parseDictionary(const std::string &key, std::list<Token>::iterator &begin, size_t level)
{
	std::cout << std::string(level, ' ') << "dico" << std::endl;
	MymlObject *dictionary = new MymlDictionary(key);
	std::list<Token>::iterator	it = begin;

	while (level == nbSpace(it)) {
		if (!isDictionary(it))
			throw BadParsingError(it->_token);
		dictionary->insert(parseDictionaryArg(begin, it, level));
		begin = it;
	}
	return (dictionary);
}

/**
  * * Takes the first token of the line after a define token,
  * * returns an tree of MymlObject and can be use recursively 
**/
MymlObject *MymlTree::define(std::list<Token>::iterator &begin, const std::string &key, size_t prev_level)
{
	std::list<Token>::iterator	it = begin;
	size_t	level = nbSpace(it); // checks indentation and set level to the current indentation

	if (prev_level <= level) // checks that current the level is higher than the previous level and throw exeption if not
		throw BadParsingError(it->_token);
	if (it->_type == DASH)
		return (parseList(key, begin, level));
	else if (isDictionary(it))
		return (parseDictionary(key, begin, level));
	return (NULL); // if an error
}
