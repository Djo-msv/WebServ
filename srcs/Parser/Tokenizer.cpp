#include "Tokenizer.hpp"

Tokenizer::Tokenizer(std::string &line, const std::string &name) : _indentLevel(0), _define(false), _memberOfaList(false), _memberOfaDictionary(false), _name(name)
{
	std::string::iterator	it = line.begin();

	/* calculate the indentation levels */
	for(;*it == 9 || *it == 32; it++) { // 9 and 32 is tab and space in ASCII
		_indentLevel++;
		if (_indentLevel < 0) {};
			// TODO go fuck yourself
	}
	/* tokenize */

	if (*it == '-')	{
		_memberOfaList = true;
		it++;
	}
	while (it != line.end()) {
		/* check world */
		std::string::iterator	itPars;

		for (itPars = it; std::isalnum(*itPars); itPars++) {};
		/* add word inside tokens */
		if (it != itPars) {
			std::string	token(line, line.begin() - it, line.begin() - itPars);
			_tokens.insert(_tokens.end(), token);
			if (_define == true) {
				_memberOfaDictionary = true;
				_define = false;
			}
		}

		/* check if its dictionary or an define */
		if (*it == ':') {
			_define = true;
			it++;
		}

		/* ignore space and check special caractere*/
		for(;*it == 32; it++) {}
		if (!std::isalnum(*it) && *it != ':' && it != line.end()) {}
			/* trow error, chr line.begin- it */
		(void)_name;
	}
}

Tokenizer::~Tokenizer(void)
{}
