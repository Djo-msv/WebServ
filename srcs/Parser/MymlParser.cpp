#include "MymlParser.hpp"

MymlParser::MymlParser(char *path)
{
	ifstream	*file;
	std::string	name;

	while ((file = openFile(path, name))) /* if directory open each fd of .myml file, else return fd */
	{
		readFile(file);
//		_myml.insert(name, MymlList); // if file already have list
		// read file
		// tokenizer
	}
}

MymlParser::~MymlParser(void)
{
	_file->close();
}

void	MymlParser::readFile(ifstream *file)
{
	std::string	line;

	while (std::getline(*file, line)) /* read each line inside file */
	{
		int						indentLevel = 0;
		std::string::iterator	it = line.begin();
		while (it != line.end()) /* tocken */
		{
			if (*it == '-')
				_elementOfList = true;
			if (*it == ':' && it == line.end()) {};
				_elementOfDico = true;
			else if (*it == ':')
				_isDicionary = true;
			if (*it == '\t')
				indentLevel++;
			if (isWorld(it))
			if (*it == '#')
				break ;
		}
	}
}
