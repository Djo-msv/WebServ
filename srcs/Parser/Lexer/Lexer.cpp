#include "Parser/Lexer/Lexer.hpp"
#include <map>

Lexer::Lexer(std::list<Token> &tokens)
{
	std::cout << std::endl;
	std::map<int, std::string> types;
	types.insert(std::pair<int, std::string>(0, "HASH"));
	types.insert(std::pair<int, std::string>(1, "SPACE"));
	types.insert(std::pair<int, std::string>(2, "INDENTATION"));
	types.insert(std::pair<int, std::string>(3, "DASH"));
	types.insert(std::pair<int, std::string>(4, "DOUBLE_QUOTE"));
	types.insert(std::pair<int, std::string>(5, "QUOTE"));
	types.insert(std::pair<int, std::string>(6, "COLON"));
	types.insert(std::pair<int, std::string>(7, "COMMA"));
	types.insert(std::pair<int, std::string>(8, "OPEN_BRACE"));
	types.insert(std::pair<int, std::string>(9, "CLOSE_BRACE"));
	types.insert(std::pair<int, std::string>(10, "OPEN_BRACKET"));
	types.insert(std::pair<int, std::string>(11, "CLOSE_BRACKET"));
	types.insert(std::pair<int, std::string>(12, "STRING"));
	types.insert(std::pair<int, std::string>(13, "INTEGER"));
	types.insert(std::pair<int, std::string>(14, "FLOAT"));
	types.insert(std::pair<int, std::string>(15, "BOOL"));
	types.insert(std::pair<int, std::string>(16, "NULL_VALUE"));
	types.insert(std::pair<int, std::string>(17, "STRING_CAST"));
	types.insert(std::pair<int, std::string>(18, "INTEGER_CAST"));
	types.insert(std::pair<int, std::string>(19, "FLOAT_CAST"));
	types.insert(std::pair<int, std::string>(22, "AMPERSTAND"));
	types.insert(std::pair<int, std::string>(23, "STAR"));
	types.insert(std::pair<int, std::string>(24, "BACK_SLASH"));
	types.insert(std::pair<int, std::string>(25, "END_OF_LINE"));
	

	for (std::list<Token>::iterator it = tokens.begin(); it != tokens.end(); it++) {
		if ((*it)._type == END_OF_FILE)
			std::cout << "=====================================================" << std::endl;
		else
			std::cout << (int)(*it)._type << "[" << (*it)._token << "]";
	}
}

Lexer::~Lexer(void)
{}
