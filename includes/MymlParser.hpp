#pragma once

#include "ServerSocket.hpp"
#include <vector>
#include <fstream>
#include <cstdlib>
#include <sys/stat.h>
#include <string>

class MymlParser
{
	public :
		MymlParser(char *files); /* Calling the constructor, verifying the file, and executing the process */
		~MymlParser(); /* Close _confFilefd */

		std::vector<ServerConfig>	getServerConfiguration(); // return vector of serverConfiguration */
	private :
		std::ifstream				*_file;
		std::vector<ServerConfig>	_servConf;

		void		openFile(const char *path);
		void		addServerConfiguration(std::string serverName);
		void		addSetting(std::string line);
		std::string	readFile();

		// Error class
		class WrongPerm : public std::runtime_error {
			public :
				WrongPerm(const std::string msg) : std::runtime_error(msg) {}
		};
		class NotAnFile : public std::runtime_error {
			public :
				NotAnFile(const std::string msg) : std::runtime_error(msg) {}
		};
		class BadParsing : public std::runtime_error {
			public :
				BadParsing(const std::string msg) : std::runtime_error(msg) {}
		};
};
