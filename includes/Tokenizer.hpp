#pragma once

#include <iostream>
#include <vector>

class Tokenizer
{
	public :
		Tokenizer(std::string &line, const std::string &name);
		~Tokenizer();

		int	getIdentLevel();
		bool	isDefine();
		bool	isMemberOfaList();
		bool	isMemberOfaDictionary();
	
	private :

		int		_indentLevel;
		bool	_define;
		bool	_memberOfaList;
		bool	_memberOfaDictionary;

		std::string	_name;
		
		std::vector<std::string>	_tokens;
};

