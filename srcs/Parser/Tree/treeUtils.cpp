#include "MymlTree.hpp"

bool	isValue(std::list<Token>::iterator it)
{
	if (it->_type >= 12 && it->_type <= 16)
		return (true);
	return (false);
}

bool	isDictionary(std::list<Token>::iterator it)
{
	if (it->_type == STRING) {
		it++;
		if (it->_type == COLON)
			return (true);
	}
	return (false);
}

size_t	nbSpace(std::list<Token>::iterator &it)
{
	size_t	level;

	if (it->_type == INDENTATION) {
		level = it->_level;
		it++;
		return (level);
	}
	return (0);
}
