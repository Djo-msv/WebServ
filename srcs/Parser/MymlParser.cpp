//#include "MymlParser.hpp"
//
//MymlParser::MymlParser(char *path)
//{
//	std::map<std::string, ifstream>	file;

//	openFile(std::string(path), file)
//	for (std::map::iterator it = file.begin(); it != file.end(); it++) /* iterate on every file */
//		readFile(it->second);
//	clarityCheck(); /* Checks the parsing in its entirety; if an error is found, displays an error message explaining the nature of the error. */
//	buildTree(_myml); /* create a tree resulting from the parsing */
//}

//MymlParser::~MymlParser(void)
//{}

#include <iostream>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>

int	isDirectory(std::string path)
{
   struct stat statbuf;

   if (stat(path.c_str(), &statbuf) != 0)
       return 0;
   return S_ISDIR(statbuf.st_mode);
}

void openFile(std::string path)
{
	if (isDirectory(path)) {
		DIR	*dir;
		struct dirent *ent;

		if ((dir = opendir(path.c_str())) != NULL) {
			while ((ent = readdir(dir)) != NULL)
				if (*(ent->d_name) != '.')
					openFile(path + "/" + ent->d_name);	
		}
	}
	else {
		std::cout << path << " :" << std::string(path, path.rfind('/') + 1, path.size()) << std::endl;
	}
}

//void	MymlParser::readFile(ifstream *file)
//{
//	std::string	line;
//
//	while (std::getline(file, line)) /* read each line inside file */
//		_tokens.insert(Tokenizer(line)); /* cuts the line into tokens and defines its depth level */
//	close(file);
//}

int	main(int argc, char **argv)
{
	openFile(argv[1]);
}
