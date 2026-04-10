#pragma once

#include <iostream>
#include <list>

class Tokenizer
		int		getIndent();
		bool	isDefine();
		bool	isMemberOfaList();
		bool	isMemberOfaDictionary();
		bool	isString(char c);

		std::list<std::string>	_tokens;
		
};

