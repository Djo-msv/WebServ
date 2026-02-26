#pragma once

#include <vector>

#include "Tokenizer.hpp"
#include "File.hpp"
#include "Lexer.hpp"
#include "TokenTransformer.hpp"

class Parser
{
	public :
		Parser(const std::string &path);
		~Parser();

	private :
		std::list<Token> _tokens;
};
