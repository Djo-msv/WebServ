#include "Parser/ParserError.hpp"

ParserError::ParserError(std::list<Token> &tokens) : _tokens(tokens)
{}

ParserError::~ParserError(void)
{}

std::string ParserError::PrintLine(const std::list<Token>::iterator &line, const std::list<Token>::iterator &token, std::string &msg)
{
	std::string buffer(msg);

	for (std::list<Token>::iterator it = line; it->_type != END_OF_LINE && it->_type != END_OF_FILE; it++) {
		if (it == token)
			buffer += "\e[1;31m";
		else if (it->_type >= 8 && it->_type <= 11)
			buffer += "\e[1;35m";
		else if (it->_type == BACK_SLASH)
			buffer += "\e[0;33m";
		else if (it->_type == STRING)
			buffer += "\e[1;37m";
		else
			buffer += "\e[0m";
		buffer += it->_token;
	}
	return (buffer);
}

void ParserError::unexpectedToken(const std::list<Token>::iterator &token)
{
	std::ostringstream str;
	std::list<Token>::iterator	begin;

//	CountLine(&begin, token);
	std::string msg = str.str();
	(void)token;
	throw BadParsingError("error with token :" + token->_token);
}
