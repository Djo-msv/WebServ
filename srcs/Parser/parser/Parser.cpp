#include "Parser.hpp"

Parser::Parser(const std::string &path)
{
	File files(path);
	std::vector<std::string> filesvalue = files.getFile();

	for (std::vector<std::string>::iterator it = filesvalue.begin(); it != filesvalue.end(); it++)
		Tokenizer(*it, _tokens);
	Lexer print(_tokens);
}

Parser::~Parser()
{}
