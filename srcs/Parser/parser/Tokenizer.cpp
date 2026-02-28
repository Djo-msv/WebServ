#include "Tokenizer.hpp"

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

		while (file.size() == checkToken && i != 9)
			(this->*functions[i++])(file, tokens);
		if (file.size() == checkToken)
			throw BadParsing(file);
	}
	tokens.insert(tokens.end(), Token{"", END_OF_FILE, 0});
}

Tokenizer::~Tokenizer()
{}

void	Tokenizer::coment(std::string &file, std::list<Token> &tokens)
{
	if (file[0] == '#') {
		std::string::iterator it;

		for (it = file.begin(); it != file.end() && (*it) != '\n'; it++) {}
		tokens.insert(tokens.end(), Token{std::string(file, 0, it - file.begin()), HASH});
		file.erase(file.begin(), it);
	}
}

void	Tokenizer::complexToken(std::string &file, std::list<Token> &tokens)
{
	std::string::iterator it = file.begin();
	
	switch (file[0]) {
		case '\t':
			for (; *it == '\t'; it++) {};
			tokens.insert(tokens.end(), Token{std::string(file, 0, it - file.begin()), SPACE, 0});
			break;
		case 32:
			for (; *it == 32; it++) {};
			tokens.insert(tokens.end(), Token{std::string(file, 0, it - file.begin()), SPACE, 0});
			break;
		case '-':
			it++;
			tokens.insert(tokens.end(), Token{std::string(file, 0, it - file.begin()), DASH, 0});
			break;
	}
	if (it != file.begin())
		file.erase(file.begin(), it);
}

void	Tokenizer::basicToken(std::string &file, std::list<Token> &tokens)
{
	std::string::iterator it = file.begin();

	switch (file[0]) {
		case ':':
			it++;
			tokens.insert(tokens.end(), Token{std::string(file, 0, it - file.begin()), COLON, 0});
			break;
		case ',':
			it++;
			tokens.insert(tokens.end(), Token{std::string(file, 0, it - file.begin()), COMMA, 0});
			break;
		case '"':
			it++;
			tokens.insert(tokens.end(), Token{std::string(file, 0, it - file.begin()), DOUBLE_QUOTE, 0});
			break;
		case '\'':
			it++;
			tokens.insert(tokens.end(), Token{std::string(file, 0, it - file.begin()), QUOTE, 0});
			break;
		case '{':
			it++;
			tokens.insert(tokens.end(), Token{std::string(file, 0, it - file.begin()), OPEN_BRACE, 0});
			break;
		case '}':
			it++;
			tokens.insert(tokens.end(), Token{std::string(file, 0, it - file.begin()), CLOSE_BRACE, 0});
			break;
		case '[':
			it++;
			tokens.insert(tokens.end(), Token{std::string(file, 0, it - file.begin()), OPEN_BRACKET, 0});
			break;
		case ']':
			it++;
			tokens.insert(tokens.end(), Token{std::string(file, 0, it - file.begin()), CLOSE_BRACKET, 0});
			break;
	}
	if (it != file.begin())
		file.erase(file.begin(), it);
}

void	Tokenizer::typeOfValue(std::string &file, std::list<Token> &tokens)
{
	std::string::iterator it = file.begin();

	// check int
	if (isdigit(*it)) {
		for (; isdigit(*it); it++){};
		if (*it == 32 || it == file.end() || *it == '\n')
			tokens.insert(tokens.end(), Token{std::string(file, 0, it - file.begin()), INTEGER, 0});
		else
			it = file.begin();
	}
	// check float
	if (isdigit(*it)) {
		for (; isdigit(*it); it++){};
		if (*it == '.')
			it++;
		for (; isdigit(*it); it++){};
		if (*it == 32 || it == file.end() || *it == '\n')
			tokens.insert(tokens.end(), Token{std::string(file, 0, it - file.begin()), FLOAT, 0});
		else
			it = file.begin();
	}
	// ckeck bool
	if (file.rfind("true", 0) == 0) {
		it += 5;
		if (*it == 32 || it == file.end() || *it == '\n')
			tokens.insert(tokens.end(), Token{std::string(file, 0, it - file.begin()), BOOL, 0});
		else
			it = file.begin();
	}
	else if (file.rfind("false", 0) == 0) {
		it += 6;
		if (*it == 32 || it == file.end() || *it == '\n')
			tokens.insert(tokens.end(), Token{std::string(file, 0, it - file.begin()), BOOL, 0});
		else
			it = file.begin();
	}
	if (isalnum(*it)) {
		for(; isalnum(*it); it++) {};
		tokens.insert(tokens.end(), Token{std::string(file, 0, it - file.begin()), STRING, 0});
	}
	if (it != file.begin())
		file.erase(file.begin(), it);
}

void	Tokenizer::castOfValue(std::string &file, std::list<Token> &tokens)
{
	std::string::iterator it = file.begin();
	
	if (file.rfind("!!str", 0) == 0) {
		it += 5;
		tokens.insert(tokens.end(), Token{std::string(file, 0, it - file.begin()), STRING_CAST, 0});
	}
	else if (file.rfind("!!int", 0) == 0) {
		it += 5;
		tokens.insert(tokens.end(), Token{std::string(file, 0, it - file.begin()), INTEGER_CAST, 0});
	}
	else if (file.rfind("!!float", 0) == 0) {
		it += 7;
		tokens.insert(tokens.end(), Token{std::string(file, 0, it - file.begin()), FLOAT_CAST, 0});
	}
	else if (file.rfind("!!double", 0) == 0) {
		it += 8;
		tokens.insert(tokens.end(), Token{std::string(file, 0, it - file.begin()), DOUBLE_CAST, 0});
	}
	else if (file.rfind("!!bool", 0) == 0) {
		it += 6;
		tokens.insert(tokens.end(), Token{std::string(file, 0, it - file.begin()), BOOL_CAST, 0});
	}
	if (it != file.begin())
		file.erase(file.begin(), it);
}

void	Tokenizer::alias(std::string &file, std::list<Token> &tokens)
{
	std::string::iterator it = file.begin();
	
	switch (file[0]) {
		case '&':	
			it += 1;
			for(; *it != 32 && it != file.end() && *it != '\n' && *it != '\\'; it++) {};
			tokens.insert(tokens.end(), Token{std::string(file, 0, it - file.begin()), AMPERSTAND, 0});
			break;
		case '*':	
			it += 1;
			for(; *it != 32 && it != file.end() && *it != '\n' && *it != '\\'; it++) {};
			tokens.insert(tokens.end(), Token{std::string(file, 0, it - file.begin()), STAR, 0});
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
		tokens.insert(tokens.end(), Token{std::string(file, 0, it - file.begin()), BACK_SLASH, 0});
	}
	if (it != file.begin())
		file.erase(file.begin(), it);
}

void	Tokenizer::specialValue(std::string &file, std::list<Token> &tokens)
{
	std::string::iterator it = file.begin();
	
	if (*it == '\n') {
		it++;
		tokens.insert(tokens.end(), Token{std::string(file, 0, it - file.begin()), END_OF_LINE, 0});
	}
	if (it != file.begin())
		file.erase(file.begin(), it);
}
