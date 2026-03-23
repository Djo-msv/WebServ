#include "Parser/Lexer/TokenTransformer.hpp"

TokenTransformer::TokenTransformer(std::list<Token> &tokens) : _status(STOP_LINE)
{
	for (std::list<Token>::iterator it = tokens.begin(); it != tokens.end();) {
		std::list<Token>::iterator tmp = it;

		indentationTransformer(it);
		quoteTransformer(tokens, it);
		castTransformer(tokens, it);
		remover(tokens, it);
		if (tmp == it)
			it++;
	}
}

TokenTransformer::~TokenTransformer(void)
{}

void	TokenTransformer::remover(std::list<Token> &tokens, std::list<Token>::iterator &it)
{
	// remove backspace
	if (_status == NEW_LINE || _status == STOP_LINE) {
		if ((*it)._type == END_OF_LINE) {
			it = tokens.erase(it);
			_status = STOP_LINE;
		}
		else
			_status = NULL_STATUE;
	}
	if (((*it)._type == END_OF_LINE || ((*it)._type == END_OF_FILE)) && _status != STOP_LINE)
		_status = NEW_LINE;

	// remove commant
	if ((*it)._type == HASH)
		it = tokens.erase(it);

	// remove unused space
	if ((*it)._type == SPACE && _status == NULL_STATUE) {
		it = tokens.erase(it); 
	}
}

void	TokenTransformer::indentationTransformer(std::list<Token>::iterator &it)
{
	if ((*it)._type == SPACE && _status == STOP_LINE)
		throw BadParsing((*it)._token);
	else if ((*it)._type == SPACE && _status == NEW_LINE) {
		(*it)._type = INDENTATION;
		(*it)._level = strlen(((*it)._token).c_str());
	}
}

void	TokenTransformer::quoteTransformer(std::list<Token> &tokens, std::list<Token>::iterator &it)
{
	static std::string	value("");

	//single quote
	if ((*it)._type == QUOTE && _status != SINGLE_QUOTE_STATUE && _status != DOUBLE_QUOTE_STATUE) {
		_status = SINGLE_QUOTE_STATUE;
		it = tokens.erase(it);
		return ;
	}
	else if ((*it)._type == QUOTE && _status == SINGLE_QUOTE_STATUE) {
		_status = NULL_STATUE;
		(*it)._type = STRING;
		(*it)._token = value;
		value.clear();
		return ;
	}
	else if ((*it)._type == BACK_SLASH && _status == SINGLE_QUOTE_STATUE) {
		if ((*it)._token == "\\'") {
			_status = NULL_STATUE;
			(*it)._type = STRING;
			value += '\\';
			(*it)._token = value;
			value.clear();
			return ;
		}
	}
	//double quote
	if ((*it)._type == DOUBLE_QUOTE && _status != DOUBLE_QUOTE_STATUE && _status != SINGLE_QUOTE_STATUE) {
		_status = DOUBLE_QUOTE_STATUE;
		it = tokens.erase(it);
		return ;
	}
	else if ((*it)._type == DOUBLE_QUOTE && _status == DOUBLE_QUOTE_STATUE) {
		_status = NULL_STATUE;
		(*it)._type = STRING;
		(*it)._token = value;
		value.clear();
		return ;
	}
	if (_status == SINGLE_QUOTE_STATUE) {
		if ((*it)._type == END_OF_LINE || (*it)._type == END_OF_FILE)
			throw BadParsing(value);
		value += (*it)._token;
		it = tokens.erase(it);
	}
	if (_status == DOUBLE_QUOTE_STATUE) {
		if ((*it)._type == END_OF_LINE || (*it)._type == END_OF_FILE)
		{
			throw BadParsing(value);
		}
		if ((*it)._type == BACK_SLASH) {
			if ((*it)._token == "\\n")
				value += '\n';
			else if ((*it)._token == "\\t")
				value += '\t';
			else if ((*it)._token == "\\\\")
				value += '\\';
			else if ((*it)._token == "\\\"")
				value += '"';
			else
				throw BadParsing((*it)._token);
		}
		else
			value += (*it)._token;
		it = tokens.erase(it);
	}
}

void	TokenTransformer::castTransformer(std::list<Token> &tokens, std::list<Token>::iterator &it)
{
	if ((*it)._type == STRING_CAST) {
		it = tokens.erase(it);
		if ((*it)._type != END_OF_LINE && (*it)._type != END_OF_FILE)
			(*it)._type = STRING;
		else
			throw BadParsing((*it)._token);
	}
	if ((*it)._type == INTEGER_CAST) {
		it = tokens.erase(it);
		if ((*it)._type != END_OF_LINE && (*it)._type != END_OF_FILE) {
			(*it)._token.erase((*it)._token.find((*it)._token));
			(*it)._type = INTEGER;
		}
		else
			throw BadParsing((*it)._token);
	}
	if ((*it)._type == FLOAT_CAST) {
		it = tokens.erase(it);
		if ((*it)._type != END_OF_LINE && (*it)._type != END_OF_FILE)
			(*it)._type = FLOAT;
		else
			throw BadParsing((*it)._token);
	}
}
