#include "Parser/ParserError.hpp"

ParserError::ParserError(std::list<Token> &tokens) : _tokens(tokens)
{}

ParserError::~ParserError(void)
{}



void ParserError::unexpectedToken(const std::list<Token>::iterator &tokens)
{
	throw BadParsingError(std::string("error on token :") + tokens->_token);
}
