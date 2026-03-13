#pragma once

#include <list>
#include "Tokenizer.hpp"

class Lexer
{
	public :
		Lexer(std::list<Token> &tokens);
		~Lexer();
};
