#pragma once

#include <map>
#include <string>
#include <netinet/in.h>
#include <unistd.h>
#include <stdexcept>
#include <HttpErrors.hpp>
#include <list>
#include <ctime>

# define value second
# define key first

struct location
{
    int			allowed_methods;
    std::string	path;
    std::string	root;
    std::string	index;
};

class ServerConfig
{
    public:
		ServerConfig(int port, location root_location, std::map<std::string, location *> locations,
			std::map<std::string, std::string> cgi_extensions, std::string exec_folder, std::map<int, std::string> error_files, time_t timeout);
		
			~ServerConfig();

        enum MethodFlag {
            GET = 1 << 0,
            POST = 1 << 1,
            DELETE = 1 << 2
        };
		

        static 		MethodFlag	stringToMethodFlag(const std::string &method);
        bool		isMethodAllowed(std::list<std::string> &full, int method) const; // Method Flag conseillé mais pas forcé
        std::string	getCgi(const std::string &extension) const;
        std::string	getFullPath(std::list<std::string> &full) const;
		std::string	getRootFolder() const;
		bool		isExecFolder(std::list<std::string> &full) const;
		std::string getIndex(std::list<std::string> &full) const;
        std::string getErrorFile(int errorCode) const;
		time_t		getTimeout(void) const;

        struct sockaddr_in					sin;
        socklen_t							sin_len;

		sa_family_t                         sin_family;
        int                          		sin_port;

    private:
		location							root_location;
		std::map<std::string, location *>	locations;
        std::map<std::string, std::string>	cgi_extensions;
        std::map<int, std::string>			error_files;
		std::string							exec_folder;
		time_t								timeout;
};
