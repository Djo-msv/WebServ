#pragma once

#include <list>

#include "Token.hpp"

#define NULL_STATUE 0
#define SINGLE_QUOTE_STATUE 1
#define DOUBLE_QUOTE_STATUE 2
#define NEW_LINE 3
#define STOP_LINE 4


class TokenTransformer
{
	public :
		TokenTransformer(std::list<Token> &tokens);
		~TokenTransformer();

	private :
		uint8_t	_status;
		
		void	remover(std::list<Token> &tokens, std::list<Token>::iterator &it);
		void	indentationTransformer(std::list<Token>::iterator &it);
		void	quoteTransformer(std::list<Token> &tokens, std::list<Token>::iterator &it);
		void	castTransformer(std::list<Token>::iterator &it);

		class BadParsing : public std::runtime_error {
			public :
				BadParsing(const std::string msg) : std::runtime_error(msg) {}
		};
};
