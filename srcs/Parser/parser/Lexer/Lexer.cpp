#include "Lexer.hpp"

Lexer::Lexer(std::list<Token> &tokens)
{
	std::cout << std::endl;

	for (std::list<Token>::iterator it = tokens.begin(); it != tokens.end(); it++) {
		if ((*it)._type == END_OF_FILE)
			std::cout << "=====================================================" << std::endl;
		else
			std::cout << (int)(*it)._type << (*it)._token;
	}
}

Lexer::~Lexer(void)
{}
