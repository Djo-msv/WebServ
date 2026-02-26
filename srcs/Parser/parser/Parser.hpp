#pragma once

#include <vector>

#include "Tokenizer.hpp"
#include "File.hpp"
#include "Lexer.hpp"

class Parser
{
	public :
		Parser(const std::string &path);
		~Parser();

	private :
		std::vector<Token> _tokens;
};
