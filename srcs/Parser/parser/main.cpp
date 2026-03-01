#include <iostream>
#include <cstring>

#include "Lexer/File.hpp"
#include "Lexer/Tokenizer.hpp"
#include "Lexer/Parser.hpp"

int	main(int argc, char **argv)
{
	if (argc != 2) {
		int	msg_len = 0;

		if (argc > 2)
			std::cout << "too much argument" << std::endl;
		else
			std::cout << "not enought argument" << std::endl;
		for (int i = 0; i != argc; i++) {
			std::cout << argv[i] << ' ';
			if (i < 2)
				msg_len += strlen(argv[i]) + 1;
		}
		std::cout << std::endl;
		for (int i = 0; i != msg_len; i++) {
			std::cout << '~';
		}
		std::cout << '^' << std::endl;
		return (1);
	}
	Parser parser(argv[1]);
	return (0);
}
