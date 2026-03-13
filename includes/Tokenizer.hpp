#pragma once

#include <iostream>
#include <vector>

class Tokenizer
		int		getIndent();
		bool	isDefine();
		bool	isMemberOfaList();
		bool	isMemberOfaDictionary();
		bool	isString(char c);

		std::vector<std::string>	_tokens;
		
};

