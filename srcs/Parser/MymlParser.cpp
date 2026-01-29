#include "MymlParser.hpp"

MymlParser::MymlParser(char *file)
{
	std::string	line;
	std::string::iterator it;

	openFile(file); /* open file, if error, throw an excetpion */
	while ((line = readFile()) != "")
	{
		for (it = line.begin(); isspace(static_cast<int>(*it)) && it != line.end(); it++) {}; /* found the first char */
		if (*it == '#') // comments
			continue ;
		if (*it == '-' && *(line.end()) == ':')
			addServerConfiguration(line); /* add serverConfiguration to the vector, and set the name */
		if (line.find(':', 0))
			addSetting(line); /* add line to the last confServer vector, if none, throw error */
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

void	MymlParser::addServerConfiguration(std::string serverName)
{
	ServerConfig	server = {0,0,0,0,0};

	std::string::iterator it = serverName.find(':', 0) + serverName.begin();
	if (it == serverName.begin())
		return ;
	serverName.erase(it, serverName.end());
	for (it = serverName.begin(); (isspace(static_cast<int>(*it)) || *it == '-' ) && it != serverName.end(); it++) {}; /* found the first char */
	serverName.erase(serverName.begin(), it);
	server.name = serverName;
	_servConf.insert(_servConf.end(), server);	
}

void	MymlParser::addSetting(std::string line)
{
	int	i = 0;
	std::string setting[] = {"sin_port", "sin_family", "cgi_path", "index_file"};

	if (_servConf.empty() == 1)
		throw BadParsing("at line : " + line + "no Server define before");
	for (i = 0; i <= 3 && line.find(setting[i], 0) == 0; i++) {};
	std::string::iterator it = line.find(':', 0) + line.begin() + 1;
	for (; it != line.end() && isspace(static_cast<int>(*it)); it++) {};
	line.erase(line.begin(), it);
	switch (i)
	{
		case 0:
			(*_servConf.end()).sin_port = std::atoi(line.c_str());
			break ;
		case 1:	
			(*_servConf.end()).sin_family = std::atoi(line.c_str());
			break ;
		case 2:
			(*_servConf.end()).cgi_path = line;
			break ;
		case 3:
			(*_servConf.end()).index_file = line;
			break ;
	}
}
