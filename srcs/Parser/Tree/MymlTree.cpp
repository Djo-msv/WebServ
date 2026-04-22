#include "Parser/Tree/MymlTree.hpp"

MymlTree::MymlTree(std::list<Token> &tokens)
{
	std::string key;
	std::string value;

	_root =  new MymlList("root");
	_error = new ParserError(tokens);
	try {
		for (std::list<Token>::iterator it = tokens.begin(); it != tokens.end();) {
			for (;it->_type == END_OF_FILE || it->_type == END_OF_LINE; it++){};
			if (it == tokens.end())
				break;
			if (it->_type == STRING) {
				key = it->_token;
				it++;
				if (it->_type == COLON) {
					it++;
					if (isValue(it)) { // is pair
						value = it->_token;
						for(;it->_type == END_OF_LINE; it++){};
						MymlObject *pair = NULL;
						try {pair = new MymlList(key);}
						catch (const std::bad_alloc& e){
							if (pair)
								delete pair;
							throw std::bad_alloc();
						}
					}
					else if (it->_type == END_OF_LINE){ // is list or dictionary
						for(;it->_type == END_OF_LINE; it++){};
						_root->insert(define(it, key, 0));
					}
					else if (it->_type == OPEN_BRACKET || it->_type == OPEN_BRACE)
						_root->insert(inlineDefine(it, key));
					else
						_error->unexpectedToken(it);
				}
			}
			else
				throw (BadParsingError("Do you really want to break everything?"));
		}
	}	
	catch (const std::runtime_error &e) {
		delete _root;
		delete _error;
		throw BadParsingError(e.what());
	}
}
	

MymlTree::~MymlTree(void)
{
	delete _root;
	delete _error;
}

MymlObject *MymlTree::listDefine(std::list<Token>::iterator &it, const std::string &key)
{
	std::string	elemKey;
	MymlObject *list = NULL;
	try {list = new MymlList(key);}
	catch (const std::bad_alloc& e){
		if (list)
			delete list;
		throw std::bad_alloc();
	}


	while (it->_type != CLOSE_BRACKET) {
		if (isValue(it)){
			elemKey = it->_token;
			it++;
			if (it->_type == COLON) {
				it++;
				list->insert(inlineDefine(it, elemKey));
			}
			else 
 			{
				try {list->insert(new MymlObject(elemKey)); }
				catch (const std::bad_alloc& e){
					delete list;
					throw std::bad_alloc();
				}
			}
		}
		if (it->_type != COMMA && it->_type != CLOSE_BRACKET) {
			delete list;
			_error->unexpectedToken(it);
		}
		if (it->_type == COMMA)
			it++;
	}
	it++;
	return (list);
}

MymlObject *MymlTree::dictionaryDefine(std::list<Token>::iterator &it, const std::string &key)
{
	std::string	elemKey;
	MymlObject *dct = NULL;
	try {dct = new MymlDictionary(key);}
	catch (const std::bad_alloc& e){
		if (dct)
			delete dct;
		throw std::bad_alloc();
	}
	while (it->_type != CLOSE_BRACE) {
		if (isValue(it)){
			elemKey = it->_token;
			it++;
			if ((it++)->_type != COLON) {
				delete dct;
				_error->unexpectedToken(it);
			}
			if (isValue(it))
			{
				try {dct->insert(std::pair<std::string, MymlObject*>(elemKey, new MymlObject((it++)->_token))); }
				catch (const std::bad_alloc& e){
					delete dct;
					throw std::bad_alloc();
				}
			}
			else if (it->_type == OPEN_BRACKET || it->_type == OPEN_BRACE) {
				dct->insert(std::pair<std::string, MymlObject*>(elemKey, inlineDefine(it, elemKey)));
				it++;
			}
			else {
				delete dct;
				_error->unexpectedToken(it);
			}
		}
		if (it->_type != COMMA && it->_type != CLOSE_BRACE) {
			delete dct;
			_error->unexpectedToken(it);
		}
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
 * Takes an line of token and check if it's an define or a value
 * return an Object of the define or value
 */
MymlObject *MymlTree::parseListArg(std::list<Token>::iterator &it, size_t level)
{
	std::string	key;
	std::string value;

	if (it->_type == DASH)
		it++;
	else 
		_error->unexpectedToken(it);
	if (it->_type != COLON && !isValue(it))
		_error->unexpectedToken(it);
	key = it->_token;
	it++;
	// is define or pair
	if (it->_type == COLON) {
		it++;
		if (isValue(it)) { // is pair
			value = it->_token;
			it++;
			for(;it->_type == END_OF_LINE; it++){};
			return (new MymlPair(key, value));	
		}
		else if (it->_type == END_OF_LINE){ // is list or dictionary
			for(;it->_type == END_OF_LINE; it++){};
			return (define(it, key, level));
		}
	}
	// is value
	for(;it->_type == END_OF_LINE; it++){};
	return (new MymlObject(key)); // !
}

/*
 * Takes a line of token and check if it's an define or a dictionary value
 * return a pair of value for the dictionary
 */
std::pair<std::string, MymlObject*> MymlTree::parseDictionaryArg(std::list<Token>::iterator &it, size_t level)
{
	std::string	key;
	std::string value;
	std::pair<std::string, MymlObject*>	obj;

	if (!isValue(it))
		_error->unexpectedToken(it);
	key = it->_token;
	it++;
	// is define or pair
	if (it->_type == COLON) {
		it++;
		if (isValue(it)) { // is dictionary value
			value = it->_token;
			it++;
			if (it->_type != END_OF_LINE)
				_error->unexpectedToken(it);
			for(;it->_type == END_OF_LINE; it++){};
			obj = std::pair<std::string, MymlObject*>(key, new MymlObject(value));
			return (obj);
		}
		else if (it->_type == END_OF_LINE) { // is list or dictionary
			for(;it->_type == END_OF_LINE; it++){};
			obj = std::pair<std::string, MymlObject*>(key, define(it, key, level));
			return (obj);
		}
	}
	_error->unexpectedToken(it);
	return (std::pair<std::string, MymlObject*>());
}

/**
 * *Takes the key (name of the define) and create an list
 * browses list of token until the indentation level is not the
 * same than the first line and insert the value to the list
**/
MymlObject *MymlTree::parseList(const std::string &key, std::list<Token>::iterator &begin, size_t level)
{
	size_t	current_level;
	MymlObject *list = NULL;
	try {list = new MymlList(key);}
	catch (const std::bad_alloc& e){
		if (list)
			delete list;
		throw std::bad_alloc();
	}
	std::list<Token>::iterator	it = begin;

	while (level == (current_level = nbSpace(it))) {
		if (isDictionary(it)) {
			delete list;
			_error->unexpectedToken(it);
		}; // throw error
		try {
			list->insert(parseListArg(it, level));
		}
		catch (const std::runtime_error &e) {
			delete list;
			throw BadParsingError(e.what());
		}
		begin = it;
	}
	if (level < current_level) {
		delete list;
		_error->unexpectedToken(it);
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
	size_t	current_level;
	MymlObject *dictionary = NULL;
	try {dictionary = new MymlDictionary(key);}
	catch (const std::bad_alloc& e){
		if (dictionary)
			delete dictionary;
		throw std::bad_alloc();
	}
	
	std::list<Token>::iterator	it = begin;

	while (level == (current_level = nbSpace(it))) {
		if (!isDictionary(it)) {
			delete dictionary;
			_error->unexpectedToken(it);
		}
		try {
			dictionary->insert(parseDictionaryArg(it, level));
		}
		catch (const std::runtime_error &e) {
			delete dictionary;
			throw BadParsingError(e.what());
		}
		begin = it;
	}
	if (level < current_level) {
		delete dictionary;
		_error->unexpectedToken(it);
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
		_error->unexpectedToken(it);
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
