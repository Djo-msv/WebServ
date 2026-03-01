#include "MymlTree.hpp"

	//	compter indentation
	//	si token est un dash/ l'item apartien a une list
	// 		si token++ comptien string + : + \n / define
	// 		si token++ comptien string + : + string + \n / pair // non recursif
	// 	sinon l'item apartien a un dictionaire
	// 		add dictionary result of define

MymlTree::MymlTree(std::list<Token> &tokens)
{
	for (std::list<Token>::iterator it = tokens.begine(); it != tokens.end(); it++)
		_root.insert(define(it));
}

MymlTree::MymlTree(void)
{}

MymlObject *MymlTree::parsList(const std::string &key, std::list<Token>::iterator &begin, size_t level)
{
	MymlObject *list = new MymlList(key);
	std::list<Token>::iterator	it = begin;

	while (level == nbSpace(it)) {
		if (isDictionary(it)) {}; // throw error
		list->insert(parsListArg(it));
	}
	return (list);
}

MymlObject *MymlTree::parsDictionary(const std::string &key, std::list<Token>::iterator &begin, size_t level)
{
	MymlObject *list = new MymlDictionary(key);
	std::list<Token>::iterator	it = begin;

	while (level == nbSpace(it)) {
		if (!isDictionary(it)) {}; // throw error
		list->insert(parsDictionaryArg(it));
	}
	return (list);
}

// a la suite d'une definition, retourn un dictionaire ou une liste
MymlObject *MymlTree::define(std::list<Token>::iterator &begin, const std::string &key, size_t prev_level)
{
	bool	isList = 0;
	boll	isDictionary = 0;
	std::list<Token>::iterator	it = begin;
	size_t	level = nbSpace(it); // calcule l'indentation

	if (prev_level <= level) {}; // throw bad parsing
	if ((*it)._type == DASH)
		return (parsList(key, begin));
	if (isDictionaryArg(it))
		return (parsDictionary(key, begin));
	return (NULL);
}
