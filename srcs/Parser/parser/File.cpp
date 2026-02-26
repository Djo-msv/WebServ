#include "File.hpp"

File::File(const std::string &path)
{
	std::vector<std::ifstream *> fileFd;

	openFiles(path, fileFd);
	for(std::vector<std::ifstream *>::iterator it = fileFd.begin(); it != fileFd.end(); it++)
		readFile(*it);
}

File::~File(void)
{}

int File::isDirectory(const std::string &path)
{
	struct stat statbuf;

	if (stat(path.c_str(), &statbuf) != 0)
		return 0;
	return S_ISDIR(statbuf.st_mode);
}

void File::openFiles(const std::string &path, std::vector<std::ifstream *> &fileFd)
{
	if (isDirectory(path)) {
		DIR	*dir;
		struct dirent *ent;

		if ((dir = opendir(path.c_str())) != NULL) {
			while ((ent = readdir(dir)) != NULL)
				if (*(ent->d_name) != '.')
					openFiles(path + "/" + ent->d_name, fileFd); // protect this
			closedir(dir);
		}
	}
	else {
		std::string name(path, path.rfind('/') + 1, path.size());
	
		if (name.find(".myml") == name.size() - 5 && name.size() >= 5) {
			std::ifstream *f = new std::ifstream();

			std::cout << "open file at " << path << std::endl;
			f->open(path.c_str());
			if (f->is_open()) {
				fileFd.insert(fileFd.end(), f);
			}
		}
	}
}

void	File::readFile(std::ifstream *fileFd)
{
	std::string	line;
	std::string	fileStr;

	while (std::getline(*fileFd, line)) {
		fileStr += line;
		fileStr += '\n';
	}
	_files.insert(_files.end(), fileStr);
	fileFd->close();
	delete fileFd;
}


std::vector<std::string> File::getFile(void)
{
	return (_files);
}
