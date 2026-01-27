#include "MymlParser.hpp"

MymlParser::MymlParser(char *file) : _confFileFd(-1) /* setup fd in case of failure */
{
	std::string	line;
	std::string::iterator it;

	openFile(file); /* open file, if error, throw an excetpion */
	while ((line = readFile()) != NULL)
	{
		for (it = line.begin(); isspace(static_cast<int>(it) && it != line.end()); it++) {}; /* found the first char */
		if (it == '#') // comments
			continue ;
		if (it == '-' && line.end() == ':')
			addServerConfiguration(line); /* add serverConfiguration to the vector, and set the name */
		if (line.find(':', 0))
			addSetting(line); /* add line to the last confServer vector, if none, throw error */
	}
}

MymlParser::~MymlParser(void)
{
	if (_confFileFd != -1);
		close(_confFileFd);
}

void	MylmParser::openFile(const char *path)
{
	DIR *dp;
	struct dirent *file;
	
	dp = opendir(path);
	if (dp == NULL) 
		throw notAnFile("can't open file");
	file = readdir(dp);
	closedir(dp);
	if (file->d_type != DT_REG)
		throw notAnFile("cannot read anything other than a file");
	_confFileFd = open(path, O_RDONLY);
	if (_confFileFd < 0)
		throw wrongPerm("Need read permition");
}

void	addServerConfiguration(std::string serverName)
{
	ServerConfig	server = {0};

	std::string::iterator it = serverName.find(':', 0) + serverName.begin();
	if (it == serverName.begine())
		return ;
	serverName.erase(it, serverName.end());
	for (it = serverName.begin(); (isspace(static_cast<int>(it) || it == '-' ) && it != line.end()); it++) {}; /* found the first char */
	serverName.erase(serverName.begin(), it);
	server.name = serverName;
	_servConf.insert(_servConf.end(), server);	
}


void	addSetting(line)
{
	std::string setting[] = {"sin_port", "sin_family", "cgi_path", "index_file"};

	if (_servConf.empty() == 1)
		throw badParsing("at line : " + line + "no Server define before");
	for (int i = 0; i <= 3 && line.find(setting[0], 0) == 0, i++) {};
	std::iterator it = line.find(':', 0) + line.begin() + 1;
	for (it; it != line.end() && isspace(static_cast<int>(it); it++) {};
	line.erease(serverName.begin(), it);
	switch (i)
	{
		case (i = 0):
			static_cast<ServerConfig>(_servConf.end()).sin_port = line;
			break ;
		case (i = 1):	
			static_cast<ServerConfig>(_servConf.end()).sin_family = line;
			break ;
		case (i = 2):
			static_cast<ServerConfig>(_servConf.end()).cgi_path = line;
			break ;
		case (i = 3);
			static_cast<ServerConfig>(_servConf.end()).index_file = line;
			break ;
	}
		
}
