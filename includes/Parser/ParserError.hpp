#pragma once

#include <cstdlib>
#include <list>

#include "Parser/Tree/MymlTree.hpp"
#include "Parser/Lexer/Token.hpp"

class ParserError
{
	public :
		ParserError(std::list<Token> &tokens);
		~ParserError();

		void badIndetation(const std::list<Token>::iterator &token);
		void missingClosing(const std::list<Token>::iterator &token);
		void badCast(const std::list<Token>::iterator &token);
		
		void unespectedToken(const std::list<Token>::iterator &token);
	
	private :
		std::list<Token> &_tokens;

		int	CountLine(std::list<Token>::iterator *begin, const std::list<Token>::iterator &token);
		std::string PrintLine(const std::list<Token>::iterator &line, const std::list<Token>::iterator &token);

		class BadParsingError : public std::runtime_error {
			public :
				BadParsingError(const std::string msg) : std::runtime_error(msg) {}
		};
};
