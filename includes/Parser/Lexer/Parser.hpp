#pragma once

#include <vector>

#include "Tokenizer.hpp"
#include "File.hpp"
#include "Lexer.hpp"
#include "TokenTransformer.hpp"
#include "Parser/Tree/MymlObject.hpp"

class Parser
{
	public :
		Parser(const std::string path);
		~Parser();

		std::list<MymlObject*> getRoot();

	private :
		std::list<Token> _tokens;
		std::list<MymlObject*> _root;
};
