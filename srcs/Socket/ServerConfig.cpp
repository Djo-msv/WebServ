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
    throw std::runtime_error("No CGI handler found for extension: " + extension);
}

ServerConfig::RequestFlag ServerConfig::stringToRequestFlag(const std::string &method) const
{
    if (method == "GET") return GET;
    if (method == "POST") return POST;
    if (method == "DELETE") return DELETE;
    throw std::invalid_argument("Invalid HTTP method: " + method);
}

std::string ServerConfig::getRootFolder() const { return (rootFolder); }

std::string ServerConfig::getExecFolder() const { return (execFolder); }

std::string ServerConfig::getIndex() const { return (index_file); }