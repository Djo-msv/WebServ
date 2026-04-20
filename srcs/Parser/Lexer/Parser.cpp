#include "Parser/Lexer/Parser.hpp"
#include "Parser/Tree/MymlTree.hpp"

Parser::Parser(const std::string path) : _tree(NULL)
{
	File files(path);
	std::list<std::string> filesvalue = files.getFile();
	
	if (filesvalue.empty())
		throw std::invalid_argument("no files found at :" + path);
	try {
		for (std::list<std::string>::iterator it = filesvalue.begin(); it != filesvalue.end(); it++)
			Tokenizer(*it, _tokens);
		TokenTransformer rewrite(_tokens);
	}
	catch (const std::runtime_error &e) {
		std::cout << "\e[1;31m" << "error :" << "\e[0m" << std::endl;
		std::cout << e.what() << std::endl;
		_tokens.clear();
		throw e;
	}
	try {
		_tree = new MymlTree(_tokens);

		_root = ((_tree->getRoot())->getList());
	}
	catch (const std::runtime_error &e) {
		_tokens.clear();
		throw std::invalid_argument(e.what());
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
