#pragma once

#include "ServerSocket.hpp"
#include <vector>
#include <fstream>

class MymlParser
{
	public :
		MymlParser(char *files); /* Calling the constructor, verifying the file, and executing the process */
		~MymlParser(); /* Close _confFilefd */

		std::vector<ServerConfig>	getServerConfiguration(); // return vector of serverConfiguration */
	private :
		ifstream					_file;
		std::vector<ServerConfig>	_servConf;

		int			openFile(const char *path);
		void		addServerConfiguration(std::string serverName);
		void		addSetting(std::string line);
		std::string	readFile();
};
