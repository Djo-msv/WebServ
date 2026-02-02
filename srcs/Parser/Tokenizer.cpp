#include "Tokenizer.hpp"

Tokenizer::Tokenizer(const string &line) : _indentLevel(0), _define(false), _membersOfaList(false), _membersOfaDictionary(false)
{
	std::string::iterrator	it = line.begin();

	/* calculate the indentation levels */
	for(;*it == 9 || *it == 32; it++) { // 9 and 32 is tab and space in ASCII
		_identLevel++;
		if (_indentLevel < 0) {};
			// go fuck yourself
	}
	/* tokenize */

	if (*it == '-')	{
		MembersOfaList = true;
		it++;
	}
	while (it != line.end()) {
		/* check world */
		std::string::iterrator	itPars = it;

		for (;std::isalnum(*itPars); itPars++) {};
		/* add word inside tokens */
		if (it != itPars) {
			std::string	token(line, line.begin() - it, line.begin() - itPars);
			_tokens.insert(token);
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
		if (!std::isalnum(*it) && *it != ':') {}
			/* trow error, chr line.begin() - it */
	}
}
