#include "Parser/Lexer/Parser.hpp"
#include "Parser/Tree/MymlTree.hpp"

void	printTree(MymlObject *value, size_t level)
{
	if (!value) {
		std::cout << "null value\n";
		return ;
	}
	if (MymlList *nvalue = dynamic_cast<MymlList *>(value)) {
		std::cout << "list : [" << nvalue->getKey() << "]" << std::endl;
		std::list<MymlObject*> *lst = nvalue->getList();
		level++;
		for(std::list<MymlObject*>::iterator it = lst->begin(); it != lst->end(); it++) {
			std::cout << std::string(level, '\t') << '-';
			printTree(*it, level);
		}
	}
	else if (MymlDictionary *nvalue = dynamic_cast<MymlDictionary *>(value)) {
		std::cout << "dictionary : [" << nvalue->getKey() << "]" << std::endl;
		std::map<std::string, MymlObject*> *dct = nvalue->getDictionary();
		level++;
		for(std::map<std::string, MymlObject*>::iterator it = dct->begin(); it != dct->end(); it++) {
			std::cout << std::string(level, '\t');
			printTree(it->second, level);
		}
	}
	else if (MymlPair *nvalue = dynamic_cast<MymlPair *>(value))
		std::cout << "key : value [" << nvalue->getKey() << "]" << std::endl;
	else
		std::cout << "value [" << value->getAsString() << "]" << std::endl;
}

Parser::Parser(const std::string path) : _tree(NULL)
{
	File files(path);
	std::list<std::string> filesvalue = files.getFile();
	
	if (filesvalue.empty()) {
		std::cout << "no files found at :" << path << std::endl;
		return ;
	}
	try {
		for (std::list<std::string>::iterator it = filesvalue.begin(); it != filesvalue.end(); it++)
			Tokenizer(*it, _tokens);
		TokenTransformer rewrite(_tokens);
	}
	catch (const std::runtime_error &e) {
		std::cout << "\e[1;31m" << "error :" << "\e[0m" << std::endl;
		std::cout << e.what() << std::endl;
		_tokens.clear();
		return ;
	}
//	Lexer print(_tokens);
	try {
		_tree = new MymlTree(_tokens);

		_root = ((_tree->getRoot())->getList());
		for(std::list<MymlObject*>::iterator it = _root->begin(); it != _root->end(); it++)
			printTree(*it, 0);
	}
	catch (const std::runtime_error &e) {
		std::cout << "\e[1;31m" << "error :" << "\e[0m" << std::endl;
		std::cout << e.what() << std::endl;
		_tokens.clear();
		return ;
	}
}


std::list<MymlObject *> *Parser::getRoot()
{
	return (_root);
}

Parser::~Parser()
{
	if (_tree)
		delete _tree;
}
