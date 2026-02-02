#pragma once

#include <iostream>
#include <vector>

class Tokenizer
{
	public :
		Tokenizer(std::string &line, const std::string &name);
		~Tokenizer();
	
	private :

		int		_indentLevel;
		bool	_define;
		bool	_membersOfaList;
		bool	_membersOfaDictionary;

		std::string	_name;
		
		std::vector<std::string>	_tokens;
};

