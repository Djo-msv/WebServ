#include <fstream>
#include "MymlParser.hpp"

MymlParser::MymlParser(char *path)
{
	std::map<std::string, std::ifstream *>	file;

	openFile(std::string(path), file);
	for (std::map<std::string, std::ifstream *>::iterator it = file.begin(); it != file.end(); it++) /* iterate on every file */
		readFile(it);
	buildTree(); /* create a tree resulting from the parsing */
}

MymlParser::~MymlParser(void)
{}

void	MymlParser::buildTree(void)
{
	Tokenizer	prevToken = *(_tokens.begin());

	for (std::vector<Tokenizer>::iterator it = _tokens.begin(); it != _tokens.end(); it++) {
		Tokenizer token = (*it);
		if (token.getIndent() < prevToken.getIndent()) {
//			searchLastEqualIndent(token);	
		}
		if ((it == _tokens.begin() && token.isDefine()) || (token.isDefine() &&
				((token.getIndent() == prevToken.getIndent() && !prevToken.isDefine()) || 
				(token.getIndent() > prevToken.getIndent() && prevToken.isDefine())))) {};
			// new branch here
		if (token.isMemberOfaList() && token.getIndent() > prevToken.getIndent() && prevToken.isDefine()) {
			// new vector()
		}
		else if (token.isMemberOfaList() && (token.getIndent() == prevToken.getIndent() && !prevToken.isDefine())) {};
			// add info to the last vector
		if (token.isMemberOfaDictionary() && token.getIndent() > prevToken.getIndent() && prevToken.isDefine()) {
			// new dictionary()
		}
		else if (token.isMemberOfaDictionary() && (token.getIndent() == prevToken.getIndent() && !prevToken.isDefine())) {};
			// add info to the last dictionary
		prevToken = token;
	}
}

int	MymlParser::isDirectory(std::string path)
{
   struct stat statbuf;

   if (stat(path.c_str(), &statbuf) != 0)
       return 0;
   return S_ISDIR(statbuf.st_mode);
}

void MymlParser::openFile(std::string path, std::map<std::string, std::ifstream *> file)
{
	if (isDirectory(path)) {
		DIR	*dir;
		struct dirent *ent;

		if ((dir = opendir(path.c_str())) != NULL) {
			while ((ent = readdir(dir)) != NULL)
				if (*(ent->d_name) != '.')
					openFile(path + "/" + ent->d_name, file); // protect this
		}
	}
	else {
		std::string name(path, path.rfind('/') + 1, path.size());
	
		if (name.find(".myml") == name.size() - 5 && name.size() >= 5) {
			std::ifstream f;

			f.open(path.c_str());
			if (f.is_open()) {
				file.insert(file.end(), std::pair<std::string, std::ifstream *>(name, &f));
			}
		}
		else
			std::cout << path << std::endl;
	}
}

void	MymlParser::readFile(std::map<std::string, std::ifstream *>::iterator file)
{
	std::string	line;

	while (std::getline(*(file->second), line)) /* read each line inside file */
		_tokens.insert(_tokens.end(), Tokenizer(line, file->first)); /* cuts the line into tokens and defines its depth level */
	file->second->close();
}

