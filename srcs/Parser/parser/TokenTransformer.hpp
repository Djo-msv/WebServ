#pragma once

#include <vector>

#include "Token.hpp"

#define NULL_STATUS 0
#define SINGLE_QUOTE_STATUE 1
#define DOUBLE_QUOTE_STATUE 2


class TokenTransformer
{
	public :
		TokenTransformer(std::vector<Token> &tokens);
		~TokenTransformer();

	private :
		uint8_t	_status;
		
		void	commentRemover(std::vector<Token> &it);
		void	indentationTransformer(std::vector<Token> &it);
		void	quoteTransformer(std::vector<Token> &it);
		void	castTransformer(std::vector<Token> &it);
		void	stringTransformer(std::vector<Token> &it);

		class BadParsing : public std::runtime_error {
			public :
				BadParsing(const std::string msg) : std::runtime_error(msg) {}
		};
};
