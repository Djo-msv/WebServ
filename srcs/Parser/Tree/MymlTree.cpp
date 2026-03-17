#include "Parser/Tree/MymlTree.hpp"

MymlTree::MymlTree(std::list<Token> &tokens)
{
	std::string key;
	std::string value;

	_root =  new MymlList("root");
	for (std::list<Token>::iterator it = tokens.begin(); it != tokens.end(); it++) {
		if (it->_type == STRING) {
			key = it->_token;
			it++;
			if (it->_type == COLON) {
				it++;
				if (isValue(it)) { // is pair
					value = it->_token;
					for(;it->_type == END_OF_LINE; it++){};
					_root->insert(new MymlPair(key, value));	
				}
				else if (it->_type == END_OF_LINE){ // is list or dictionary
					for(;it->_type == END_OF_LINE; it++){};
					_root->insert(define(it, key, 0));
				}
				else if (it->_type == OPEN_BRACKET || it->_type == OPEN_BRACE) {
					_root->insert(inlineDefine(it, key));
				}
			}
		}
	}
}

MymlTree::~MymlTree(void)
{
	delete _root;
}

MymlObject *MymlTree::listDefine(std::list<Token>::iterator &it, const std::string &key)
{
	std::string	elemKey;
	MymlObject *list = new MymlList(key);

	while (it->_type != CLOSE_BRACKET) {
		if (isValue(it)){
			elemKey = it->_token;
			it++;
			if (it->_type == COLON) {
				it++;
				list->insert(inlineDefine(it, elemKey));
			}
			else 
				list->insert(new MymlObject(elemKey));
		}
		if (it->_type != COMMA && it->_type != CLOSE_BRACKET)
			throw (BadParsingError(it->_token));
		if (it->_type == COMMA)
			it++;
	}
	it++;
	return (list);
}

MymlObject *MymlTree::dictionaryDefine(std::list<Token>::iterator &it, const std::string &key)
{
	std::string	elemKey;
	MymlObject *dct = new MymlDictionary(key);

	while (it->_type != CLOSE_BRACE) {
		if (isValue(it)){
			elemKey = it->_token;
			it++;
			if ((it++)->_type != COLON) 
				throw (BadParsingError(it->_token));
			if (isValue(it))
				dct->insert(std::pair<std::string, MymlObject*>(elemKey, new MymlObject((it++)->_token)));
			else if (it->_type == OPEN_BRACKET || it->_type == OPEN_BRACE) {
				dct->insert(std::pair<std::string, MymlObject*>(elemKey, inlineDefine(it, elemKey)));
				it++;
			}
		}
		if (it->_type != COMMA && it->_type != CLOSE_BRACE)
			throw (BadParsingError(it->_token));
		if (it->_type == COMMA)
			it++;
	}
	it++;
	return (dct);
}

MymlObject *MymlTree::inlineDefine(std::list<Token>::iterator &begin, std::string &key)
{
	if (begin->_type == OPEN_BRACKET)
		return (listDefine(++begin, key));
	else if (begin->_type == OPEN_BRACE)
		return (dictionaryDefine(++begin, key));
	else
		return (new MymlPair(key, (begin++)->_token));
}

/*
 * *Takes an line of token and check if it's an define or a value
 * return an Object of the define or value
**/
MymlObject *MymlTree::parseListArg(std::list<Token>::iterator &begin, std::list<Token>::iterator &it, size_t level)
{
	(void)begin;
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
			it++;
			for(;it->_type != INDENTATION && it->_type == END_OF_LINE; it++){};
			return (new MymlPair(key, value));	
		}
		else { // is list or dictionary
			for(;it->_type != INDENTATION && it->_type == END_OF_LINE; it++){};
			return (define(it, key, level));
		}
	}
	// is value
	for(;it->_type != INDENTATION && it->_type == END_OF_LINE; it++){};
	return (new MymlObject(key)); // !
}

/**
 * *Takes an line of token and check if it's an define or a dictionary value
 * return an pair of value for the dictionary
**/
std::pair<std::string, MymlObject*> MymlTree::parseDictionaryArg(std::list<Token>::iterator &begin, std::list<Token>::iterator &it, size_t level)
{
	(void)begin;
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
			it++;
			for(;it->_type != INDENTATION && it->_type == END_OF_LINE; it++){};
			obj = std::pair<std::string, MymlObject*>(key, new MymlObject(value));
			return (obj);
		}
		else if (it->_type == END_OF_LINE) { // is list or dictionary
			for(;it->_type != INDENTATION && it->_type == END_OF_LINE; it++){};
			obj = std::pair<std::string, MymlObject*>(key, define(it, key, level));
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
	MymlObject *dictionary = new MymlDictionary(key);
	std::list<Token>::iterator	it = begin;

	while (level == nbSpace(it)) {
		if (!isDictionary(it)) {
			delete dictionary;
			throw BadParsingError(it->_token);
		}
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

	if (prev_level >= level) // checks that current the level is higher than the previous level and throw exeption if not
		throw BadParsingError(it->_token);
	if (it->_type == DASH)
		return (parseList(key, begin, level));
	else if (isDictionary(it))
		return (parseDictionary(key, begin, level));
	return (NULL); // if an error
}

MymlList	*MymlTree::getRoot(void)
{
	return (_root);
}
