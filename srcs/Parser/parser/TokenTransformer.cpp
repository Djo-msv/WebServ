#include "TokenTransformer.hpp"

void	TokenTransformer::TokenTransformer(std::vector<Token> &tokens) _status(0)
{
	for (std::vector<Token>::iterator it = tokens.begin(); it != tokens.end(); it++) {
		commentRemover(it);
		indentationTransformer(it);
		quoteTransformer(it);
		castTransformer(it);
		stringTransformer(it);
	}
}

void	TokenTransformer::TokenTransformer::~TokenTransformer(void)
{}

void	TokenTransformer::commentRemover(std::vector<Token> &it)
{}

void	TokenTransformer::indentationTransformer(std::vector<Token> &it)
{}

void	TokenTransformer::quoteTransformer(std::vector<Token> &it)
{}

void	TokenTransformer::castTransformer(std::vector<Token> &it)
{}

void	TokenTransformer::stringTransformer(std::vector<Token> &it)
{}
