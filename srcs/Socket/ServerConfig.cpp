#include <ServerConfig.hpp>

ServerConfig::ServerConfig(int port, std::map<std::string, std::string> cgiHandlers, std::map<std::string, int> requestsFlag, 
	std::string index_file, std::string rootFolder, std::string execFolder, std::map<int, std::string> errorFiles, time_t timeout) :
	sin_port(port), cgiExtensions(cgiHandlers), requestsFlag(requestsFlag), errorFiles(errorFiles), index_file(index_file),
	execFolder(execFolder), rootFolder(rootFolder), timeout(timeout) {}

ServerConfig::~ServerConfig() {}

bool ServerConfig::isMethodAllowed(const std::string &location, int method) const
{
    std::map<std::string, int>::const_iterator it = requestsFlag.find(location);
    if (it != requestsFlag.end()) {
        return it->value & method;
    }
    return false;
}


std::string ServerConfig::getCgi(const std::string &extension) const
{
    std::map<std::string, std::string>::const_iterator it = cgiExtensions.find(extension);
    if (it != cgiExtensions.end()) {
        return it->value;
    }
    throw NotImplemented();//std::invalid_argument("No CGI handler found for extension: " + extension);
}

ServerConfig::RequestFlag ServerConfig::stringToRequestFlag(const std::string &method) const
{
    if (method == "GET") return GET;
    if (method == "POST") return POST;
    if (method == "DELETE") return DELETE;
    throw NotImplemented();//std::invalid_argument("Invalid HTTP method: " + method);
}

std::string ServerConfig::getErrorFile(int errorCode) const
{
    std::map<int, std::string>::const_iterator it = errorFiles.find(errorCode);
    if (it != errorFiles.end()) {
        return it->value;
    }
    throw FileNotFound();
}

std::string ServerConfig::getRootFolder() const { return (rootFolder); }

time_t	ServerConfig::getTimeout(void) const { return (timeout); }

bool ServerConfig::isExecFolder(std::string location) const { return (execFolder == location); }

std::string ServerConfig::getIndex() const { return (index_file); }
