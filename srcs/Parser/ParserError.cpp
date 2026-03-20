#include "Parser/ParserError.hpp"

ParserError::ParserError(std::list<Token> &tokens) : _tokens(tokens)
{}

ParserError::~ParserError(void)
{}

int	ParserError::CountLine(std::list<Token>::iterator *begin, const std::list<Token>::iterator &token)
{
	int	count_line = 1;
	
	*begin = _tokens.begin();
	for (std::list<Token>::iterator it = _tokens.begin(); it != token; it++) {
		if (it->_type == END_OF_LINE) {
			count_line++;
			*begin = ++it;
		}
		else if (it->_type == END_OF_FILE)
			count_line = 0;
	}
	return (count_line);
}

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

void ParserError::badIndetation(const std::list<Token>::iterator &token)
{
	(void)token;
}

void ParserError::missingClosing(const std::list<Token>::iterator &token)
{
	(void)token;
}

void ParserError::badCast(const std::list<Token>::iterator &token)
{
	(void)token;
}

void ParserError::unespectedToken(const std::list<Token>::iterator &token)
{
	int	count;
	std::ostringstream str;
	std::list<Token>::iterator	begin;

	count = CountLine(&begin, token);
	str << "\e[1;31m" << "error on line " << count << ":" << "\e[0m" << std::endl;
	std::string msg = str.str();
	throw BadParsingError(PrintLine(begin, token, msg));
}
