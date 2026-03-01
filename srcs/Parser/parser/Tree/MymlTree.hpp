#prama once

#include <list>

#include "Token.hpp"

class MymlTree
{
	public :
		MymlTree(std::list<Token> tokens);
		~MymlTree();

	private :
		std::list<MymlObject *>	_root;
		std::list<MymlObject *> _anchor;
};
