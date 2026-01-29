#include "MymlParser.hpp"

MymlParser::MymlParser(char *path)
{
	ifstream	*file;
	std::string	name;

	while ((file = openFile(path, name))) /* if directory open each fd of .myml file, else return fd */
	{
		std::string	line;

//		_myml.insert(name, MymlList); // if file already have list
		while (std::getline(*file, line)) /* read each line inside file */
		{
			std::string::iterator it = line.begin();
			while (it != line.end()) /* tocken */
			{
				if (*it == '#')
					break ;
			}
		}
	}
}

MymlParser::~MymlParser(void)
{
	_file->close();
}

std::string	MymlParser::readFile(void)
{
	std::string line;
	
	std::getline(*_file, line);
	return (line);
}

void	MymlParser::openFile(const char *path)
{
	struct stat s;

	if (stat(path, &s) == 0)
	{
		if (!(s.st_mode & S_IFREG))
			throw NotAnFile("cannot read anything other than a file");
	}
	else
		throw NotAnFile("can't open file");
	std::ifstream file(path, std::ios::in);
	_file = &file;
}

}
