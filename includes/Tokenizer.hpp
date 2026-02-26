#pragma once

#include <iostream>
#include <vector>

class Tokenizer
{
	public :
		Tokenizer(std::string &line, const std::string &name);
		~Tokenizer();

		int		getIndent();
		bool	isDefine();
		bool	isMemberOfaList();
		bool	isMemberOfaDictionary();
		bool	isString();

		std::vector<std::string>	getTokens();

	
	private :

		int		_indentLevel;
		bool	_define;
		bool	_memberOfaList;
		bool	_memberOfaDictionary;
		bool	_string;

		std::string	_name;
		
		std::vector<std::string>	_tokens;
};

