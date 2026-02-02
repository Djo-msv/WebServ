#pragma once

class Tokenizer
{
	public :
		Tokenizer(const string &line);
		~Tokenizer();
	
	private :

		int		_indentLevel;
		bool	_define;
		bool	_membersOfaList;
		bool	_membersOfaDictionary;
		
		std::vector<std::string>	_tokens;
};

