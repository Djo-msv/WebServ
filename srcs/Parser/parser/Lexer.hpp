#pragma once

#include <vector>
#include "Tokenizer.hpp"

class Lexer
{
	public :
		Lexer(std::vector<Token> &tokens);
		~Lexer();
};
