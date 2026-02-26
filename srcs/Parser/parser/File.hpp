#pragma once

#include <iostream>
#include <vector>
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

		std::vector<std::string> getFile();
	
	private :
		std::vector<std::string> _files;

		int		isDirectory(const std::string &path);
		void	openFiles(const std::string &path, std::vector<std::ifstream *> &file);
		void	readFile(std::ifstream *fileFd);
};
