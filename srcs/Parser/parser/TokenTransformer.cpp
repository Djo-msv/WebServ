#include "TokenTransformer.hpp"

TokenTransformer::TokenTransformer(std::list<Token> &tokens) : _status(0)
{
	for (std::list<Token>::iterator it = tokens.begin(); it != tokens.end(); it++) {
		remover(tokens, it);
		indentationTransformer(it);
		quoteTransformer(it);
		castTransformer(it);
		stringTransformer(it);
	}
}

TokenTransformer::~TokenTransformer(void)
{}

void	TokenTransformer::remover(std::list<Token> &tokens, std::list<Token>::iterator &it)
{
	// remove commant
	if ((*it)._type == HASH)
		it = tokens.erase(it);

	// remove backspace
	if ((*it)._type == END_OF_LINE) {
		if (it == tokens.begin())
				it = tokens.erase(it);
		std::list<Token>::iterator end = it;
		it++;
		if ((*it)._type == END_OF_LINE)
			it = tokens.erase(it);
		it = end;
	}
}

void	TokenTransformer::indentationTransformer(std::list<Token>::iterator &it)
{}

void	TokenTransformer::quoteTransformer(std::list<Token>::iterator &it)
{}

void	TokenTransformer::castTransformer(std::list<Token>::iterator &it)
{}

void	TokenTransformer::stringTransformer(std::list<Token>::iterator &it)
{}
