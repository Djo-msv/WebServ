#include "Parser/Lexer/Tokenizer.hpp"

Tokenizer::Tokenizer(void)
{}

Tokenizer::Tokenizer(std::string &file, std::list<Token> &tokens)
{
	/* list of all function for first layer tokenizer */
	void (Tokenizer::*functions[]) (std::string &, std::list<Token> &) = 
		{
			&Tokenizer::coment,
			&Tokenizer::basicToken,
			&Tokenizer::complexToken,
			&Tokenizer::castOfValue,
			&Tokenizer::alias,
			&Tokenizer::escapeValue,
			&Tokenizer::specialValue,
			&Tokenizer::typeOfValue
		};

	/* aply function until the end of the string */
	while (!file.empty())
	{
		int		i = 0;
		size_t	checkToken = file.size();

		while (file.size() == checkToken && i != 8)
			(this->*functions[i++])(file, tokens);
		if (file.size() == checkToken)
			throw BadParsing(file);
	}
	Token token = {"", END_OF_FILE, 0};
	tokens.insert(tokens.end(), token);
}

Tokenizer::~Tokenizer()
{}

void	Tokenizer::coment(std::string &file, std::list<Token> &tokens)
{
	if (file[0] == '#') {
		std::string::iterator it;

		for (it = file.begin(); it != file.end() && (*it) != '\n'; it++) {}
		Token token = {std::string(file, 0, it - file.begin()), HASH, 0};
		tokens.insert(tokens.end(), token);
		file.erase(file.begin(), it);
	}
}

void	Tokenizer::complexToken(std::string &file, std::list<Token> &tokens)
{
	Token token;
	std::string::iterator it = file.begin();
	
	switch (file[0]) {
		case '\t':
			for (; *it == '\t'; it++) {};
			token = (Token){std::string(file, 0, it - file.begin()), SPACE, 0};
			tokens.insert(tokens.end(), token);
			break;
		case 32:
			for (; *it == 32; it++) {};
			token = (Token){std::string(file, 0, it - file.begin()), SPACE, 0};
			tokens.insert(tokens.end(), token);
			break;
		case '-':
			it++;
			token = (Token){std::string(file, 0, it - file.begin()), DASH, 0};
			tokens.insert(tokens.end(), token);
			break;
	}
	if (it != file.begin())
		file.erase(file.begin(), it);
}

void	Tokenizer::basicToken(std::string &file, std::list<Token> &tokens)
{
	Token token;
	std::string::iterator it = file.begin();

	switch (file[0]) {
		case ':':
			it++;
			token = (Token){std::string(file, 0, it - file.begin()), COLON, 0};
			tokens.insert(tokens.end(), token);
			break;
		case ',':
			it++;
			token = (Token){std::string(file, 0, it - file.begin()), COMMA, 0};
			tokens.insert(tokens.end(), token);
			break;
		case '"':
			it++;
			token = (Token){std::string(file, 0, it - file.begin()), DOUBLE_QUOTE, 0};
			tokens.insert(tokens.end(), token);
			break;
		case '\'':
			it++;
			token = (Token){std::string(file, 0, it - file.begin()), QUOTE, 0};
			tokens.insert(tokens.end(), token);
			break;
		case '{':
			it++;
			token = (Token){std::string(file, 0, it - file.begin()), OPEN_BRACE, 0};
			tokens.insert(tokens.end(), token);
			break;
		case '}':
			it++;
			token = (Token){std::string(file, 0, it - file.begin()), CLOSE_BRACE, 0};
			tokens.insert(tokens.end(), token);
			break;
		case '[':
			it++;
			token = (Token){std::string(file, 0, it - file.begin()), OPEN_BRACKET, 0};
			tokens.insert(tokens.end(), token);
			break;
		case ']':
			it++;
			token = (Token){std::string(file, 0, it - file.begin()), CLOSE_BRACKET, 0};
			tokens.insert(tokens.end(), token);
			break;
	}
	if (it != file.begin())
		file.erase(file.begin(), it);
}

bool	Tokenizer::isString(char c)
{
	char	specialChar[] = " \t\'\"\\()[]{}";
	if (isprint(c)) {
		for (int i = 0; specialChar[i]; i++) {
			if (c == specialChar[i])
				return (0);
		}
		return (1);
	}
	return (0);
}

void	Tokenizer::typeOfValue(std::string &file, std::list<Token> &tokens)
{
	Token token;
	std::string::iterator it = file.begin();

	// check int
	if (isdigit(*it)) {
		for (; isdigit(*it); it++){};
		if (*it == 32 || it == file.end() || *it == '\n') {
			token = (Token){std::string(file, 0, it - file.begin()), INTEGER, 0};
			tokens.insert(tokens.end(), token);
		}
		else
			it = file.begin();
	}
	// check float
	if (isdigit(*it)) {
		for (; isdigit(*it); it++){};
		if (*it == '.')
			it++;
		for (; isdigit(*it); it++){};
		if (*it == 32 || it == file.end() || *it == '\n') {
			token = (Token){std::string(file, 0, it - file.begin()), FLOAT, 0};
			tokens.insert(tokens.end(), token);
		}
		else
			it = file.begin();
	}
	// ckeck bool
	if (file.rfind("true", 0) == 0) {
		it += 5;
		if (*it == 32 || it == file.end() || *it == '\n') {
			token = (Token){std::string(file, 0, it - file.begin()), BOOL, 0};
			tokens.insert(tokens.end(), token);
		}
		else
			it = file.begin();
	}
	else if (file.rfind("false", 0) == 0) {
		it += 6;
		if (*it == 32 || it == file.end() || *it == '\n') {
			token = (Token){std::string(file, 0, it - file.begin()), BOOL, 0};
			tokens.insert(tokens.end(), token);
		}
		else
			it = file.begin();
	}
	if (isString(*it)) {
		for(; isString(*it); it++) {};
		token = (Token){std::string(file, 0, it - file.begin()), STRING, 0};
		tokens.insert(tokens.end(), token);
	}
	if (it != file.begin())
		file.erase(file.begin(), it);
}

void	Tokenizer::castOfValue(std::string &file, std::list<Token> &tokens)
{
	Token token;
	std::string::iterator it = file.begin();
	
	if (file.rfind("!!str", 0) == 0) {
		it += 5;
		token = (Token){std::string(file, 0, it - file.begin()), STRING_CAST, 0};
		tokens.insert(tokens.end(), token);
	}
	else if (file.rfind("!!int", 0) == 0) {
		it += 5;
		token = (Token){std::string(file, 0, it - file.begin()), INTEGER_CAST, 0};
		tokens.insert(tokens.end(), token);
	}
	else if (file.rfind("!!float", 0) == 0) {
		it += 7;
		token = (Token){std::string(file, 0, it - file.begin()), FLOAT_CAST, 0};
		tokens.insert(tokens.end(), token);
	}
	if (it != file.begin())
		file.erase(file.begin(), it);
}

void	Tokenizer::alias(std::string &file, std::list<Token> &tokens)
{
	Token token;
	std::string::iterator it = file.begin();
	
	switch (file[0]) {
		case '&':	
			it += 1;
			for(; *it != 32 && it != file.end() && *it != '\n' && *it != '\\'; it++) {};
			token = (Token){std::string(file, 0, it - file.begin()), AMPERSTAND, 0};
			tokens.insert(tokens.end(), token);
			break;
		case '*':	
			it += 1;
			for(; *it != 32 && it != file.end() && *it != '\n' && *it != '\\'; it++) {};
			token = (Token){std::string(file, 0, it - file.begin()), STAR, 0};
			tokens.insert(tokens.end(), token);
			break;
	}
	if (it != file.begin())
		file.erase(file.begin(), it);
}

void	Tokenizer::escapeValue(std::string &file, std::list<Token> &tokens)
{
	std::string::iterator it = file.begin();
	
	if (*it == '\\') {
		it++;	
		if (*it == 32 || it == file.end() || *it == '\n')
			return ;
		it++;
		Token token = {std::string(file, 0, it - file.begin()), BACK_SLASH, 0};
		tokens.insert(tokens.end(), token);
	}
	if (it != file.begin())
		file.erase(file.begin(), it);
}

void	Tokenizer::specialValue(std::string &file, std::list<Token> &tokens)
{
	std::string::iterator it = file.begin();
	
	if (*it == '\n') {
		it++;
		Token token = {std::string(file, 0, it - file.begin()), END_OF_LINE, 0};
		tokens.insert(tokens.end(), token);
	}
	if (it != file.begin())
		file.erase(file.begin(), it);
}
