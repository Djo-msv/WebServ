#include "Parser.hpp"
#include "../Tree/MymlTree.hpp"

Parser::Parser(const std::string &path)
{
	File files(path);
	std::list<std::string> filesvalue = files.getFile();
	
	if (filesvalue.empty()) {
		std::cout << "no files found at :" << path << std::endl;
		return ;
	}
	for (std::list<std::string>::iterator it = filesvalue.begin(); it != filesvalue.end(); it++)
		Tokenizer(*it, _tokens);
	try {
		TokenTransformer rewrite(_tokens);
	}
	catch (const std::runtime_error &e) {
		std::cout << e.what() << std::endl;
		_tokens.clear();
		return ;
	}
	Lexer print(_tokens);
	try {
		MymlTree	tree(_tokens);
	}
	catch (const std::runtime_error &e) {
		std::cout << "error :" << e.what() << std::endl;
		_tokens.clear();
		return ;
	}
}

Parser::~Parser()
{}
