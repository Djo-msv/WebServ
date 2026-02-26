#include "Parser.hpp"

Parser::Parser(const std::string &path)
{
	File files(path);
	std::list<std::string> filesvalue = files.getFile();

	for (std::list<std::string>::iterator it = filesvalue.begin(); it != filesvalue.end(); it++)
		Tokenizer(*it, _tokens);
	TokenTransformer rewrite(_tokens);
	Lexer print(_tokens);
}

Parser::~Parser()
{}
