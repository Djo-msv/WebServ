#include "Parser/Tree/MymlDictionary.hpp"
#include "Parser/Tree/MymlObject.hpp"

MymlDictionary::MymlDictionary(const std::string key) : _key(key)
{
	_dictionary = new std::map<std::string, MymlObject*>();
}

MymlDictionary::~MymlDictionary(void)
{
	for (std::map<std::string, MymlObject*>::iterator it = _dictionary->begin(); it != _dictionary->end(); it++)
		delete it->second;
	delete _dictionary;
}

void	MymlDictionary::insert(const std::pair<std::string, MymlObject *> value)
{
	if (_dictionary->count(value.first)) {
		delete _dictionary->at(value.first);
		_dictionary->erase(value.first);
	}
	_dictionary->insert(_dictionary->end(), value);
}

std::map<std::string, MymlObject*>	*MymlDictionary::getDictionary(void)
{
	return (_dictionary);
}

std::string	MymlDictionary::getKey(void)
{
	return (_key);
}
MymlObject *MymlDictionary::getValue(std::string key)
{
	std::map<std::string, MymlObject*>::iterator map = _dictionary->find(key);
	if (map == _dictionary->end())
		throw std::invalid_argument("Cannot find " + key + " inside map of the dictionary " + _key + " !");
	return (map->second);
}

std::string MymlDictionary::getValueAsString(std::string key)
{
	std::map<std::string, MymlObject*>::iterator map = _dictionary->find(key);
	if (map == _dictionary->end())
		throw std::invalid_argument("Cannot find " + key + " inside map of the dictionary " + _key + " !");
	return (map->second->getAsString());
}

int MymlDictionary::getValueAsInt(std::string key)
{
	std::map<std::string, MymlObject*>::iterator map = _dictionary->find(key);
	if (map == _dictionary->end())
		throw std::invalid_argument("Cannot find " + key + " inside map of the dictionary " + _key + " !");
	return (map->second->getAsInt());
}

double MymlDictionary::getValueAsFloat(std::string key)
{
	std::map<std::string, MymlObject*>::iterator map = _dictionary->find(key);
	if (map == _dictionary->end())
		throw std::invalid_argument("Cannot find " + key + " inside map of the dictionary " + _key + " !");
	return (map->second->getAsFloat());
}
