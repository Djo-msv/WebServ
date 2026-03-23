#pragma once

#include <map>
#include <string>
#include <netinet/in.h>
#include <unistd.h>
#include <stdexcept>
#include <HttpErrors.hpp>
#include <list> 

# define value second
# define key first

class ServerConfig
{
    public:
        ServerConfig(int port, std::map<std::string, std::string> cgiHandlers,
			std::map<std::string, int> requestsFlag, std::string index_file,
			std::string rootFolder, std::string execFolder, std::map<int, std::string> errorFiles, time_t timeout);
	~ServerConfig();


        enum MethodFlag {
            GET = 1 << 0,
            POST = 1 << 1,
            DELETE = 1 << 2
        };

		struct location
		{
			std::string					path;
			std::string					rootFolder;
			std::string					index_file;
		};
		

        static 		MethodFlag	stringToMethodFlag(const std::string &method);
        bool		isMethodAllowed(const std::string &location, int method) const; // Method Flag conseillé mais pas forcé
        std::string	getCgi(const std::string &extension) const;
		std::string	getRootFolder() const;
		bool		isExecFolder(std::string location) const;
		std::string getIndex() const;
        std::string getErrorFile(int errorCode) const;
		time_t		getTimeout(void) const;

        struct sockaddr_in					sin;
        socklen_t							sin_len;

		sa_family_t                         sin_family;
        int                          		sin_port;

    private:
		std::map<std::string, int>			allowed_methods;
        std::map<std::string, std::string>	cgi_extensions;
		std::map<std::string, location *>	locations;
        std::map<int, std::string>			error_files;
        location							root_location;
		time_t								timeout;
};
