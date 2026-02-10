#pragma once

#include <map>
#include <string>
#include <netinet/in.h>
#include <unistd.h>
#include <stdexcept>

# define value second
# define key first

class ServerConfig
{
    public:
        ServerConfig(std::map<std::string, std::string> cgiHandlers, std::map<std::string, int> requestsFlag, std::string index_file, std::string rootFolder, sa_family_t sin_family, int sin_port);
        ~ServerConfig();

        enum RequestFlag {
            GET = 1 << 0,
            POST = 1 << 1,
            DELETE = 1 << 2
        };

        RequestFlag stringToRequestFlag(const std::string &method) const;
        bool isMethodAllowed(const std::string &location, RequestFlag method) const;
        std::string getCgiHandler(const std::string &extension) const;
		std::string getRootFolder() const;
		std::string getIndex() const;

    private:
        std::map<std::string, std::string>  cgiHandlers;
        std::map<std::string, int>          requestsFlag;
        std::string                         index_file;
        std::string                         rootFolder;
    	sa_family_t                         sin_family;
        int                          		sin_port;
};
