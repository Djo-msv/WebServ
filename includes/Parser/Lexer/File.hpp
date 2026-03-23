#pragma once

#include <iostream>
#include <list>
#include <sys/stat.h>
#include <dirent.h>
#include <fstream>

/* This class aims to recursively open a folder
 * specified in the path and return the contents
 * of each file. */

class File
{
	public :
		File(const std::string &path);
		~File();

		std::list<std::string> getFile();
	
	private :
		std::list<std::string> _files;

		int		isDirectory(const std::string &path);
		void	openFiles(const std::string &path, std::list<std::ifstream *> &file);
		void	readFile(std::ifstream *fileFd);
};
