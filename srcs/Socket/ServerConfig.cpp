#include <ServerConfig.hpp>

ServerConfig::ServerConfig(std::map<std::string, std::string> cgiHandlers, std::map<std::string, int> requestsFlag, 
	std::string index_file, std::string rootFolder, std::string execFolder)
    : cgiExtensions(cgiHandlers), requestsFlag(requestsFlag), index_file(index_file), rootFolder(rootFolder), execFolder(execFolder) {}

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
    throw std::invalid_argument("No CGI handler found for extension: " + extension);
}

ServerConfig::RequestFlag ServerConfig::stringToRequestFlag(const std::string &method) const
{
    if (method == "GET") return GET;
    if (method == "POST") return POST;
    if (method == "DELETE") return DELETE;
    throw std::invalid_argument("Invalid HTTP method: " + method);
}

std::string ServerConfig::getErrorFile(int errorCode) const
{
    std::map<int, std::string>::const_iterator it = errorFiles.find(errorCode);
    if (it != errorFiles.end()) {
        return it->value;
    }
    throw std::invalid_argument("No error file found");
}

std::string ServerConfig::getRootFolder() const { return (rootFolder); }

bool ServerConfig::isExecFolder(std::string location) const { return (execFolder == location); }

std::string ServerConfig::getIndex() const { return (index_file); }
