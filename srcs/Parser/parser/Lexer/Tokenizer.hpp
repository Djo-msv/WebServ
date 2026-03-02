#pragma once

#include <iostream>
#include <stdint.h>
#include <list>

#include "Token.hpp"

/* The class aims to take any string of characters and
 * extract its first token. It then stores this information
 * in `_type` and the token itself in `_token`. */

class Tokenizer
{
	public :
		Tokenizer();
		Tokenizer(std::string &file, std::list<Token> &tokens);
		~Tokenizer();

	private :
		void	coment(std::string &file, std::list<Token> &tokens);
		void	basicToken(std::string &file, std::list<Token> &tokens);
		void	complexToken(std::string &file, std::list<Token> &tokens);
		void	typeOfValue(std::string &file, std::list<Token> &tokens);
		void	castOfValue(std::string &file, std::list<Token> &tokens);
		void	alias(std::string &file, std::list<Token> &tokens);
		void	escapeValue(std::string &file, std::list<Token> &tokens);
		void	specialValue(std::string &file, std::list<Token> &tokens);

		class BadParsing : public std::runtime_error {
			public :
				BadParsing(const std::string msg) : std::runtime_error(msg) {}
		};
};
