#include <ServerConfig.hpp>

ServerConfig::ServerConfig(int port, location root_location, std::map<std::string, location *> locations,
			std::map<std::string, std::string> cgi_extensions, std::string exec_folder, std::map<int, std::string> error_files, time_t timeout) :
		sin_family(AF_INET), sin_port(port), root_location(root_location), locations(locations),
		cgi_extensions(cgi_extensions), error_files(error_files), timeout(timeout) {}

ServerConfig::~ServerConfig() {}

// Need to adapt to location
bool ServerConfig::isMethodAllowed(const std::string &location, int method) const
{
    if (location.empty()) { return false ; }
    std::map<std::string, int>::const_iterator it = requestsFlag.find(location);
    if (it != requestsFlag.end())
        return it->value & method;

    std::list<std::string> full;
    std::string loc = location;
    if (*(loc.rbegin()) == '/' && loc.size() > 1)
        loc.erase(loc.size() - 1);
    if (loc.rfind('/') != std::string::npos && loc.size() > 1) {
        full.push_back(loc.substr(loc.rfind('/')));
        loc = loc.substr(0, loc.rfind('/'));
    }
    while (!loc.empty() && loc.size() > 1 && loc.rfind('/') != std::string::npos) {
        full.push_back(loc.substr(loc.rfind('/')));
        loc = loc.substr(0, loc.rfind('/'));
    }
    loc.clear();
    for (std::list<std::string>::const_iterator itt = full.begin(); itt != full.end(); itt++) {
        if (!loc.empty() && (it = requestsFlag.find(*itt + loc)) != requestsFlag.end())
            return it->value & method;
        if ((it = requestsFlag.find(*itt)) != requestsFlag.end())
            return it->value & method;
        loc = *itt;
    }
    if ((it = requestsFlag.find("/")) != requestsFlag.end())
        return it->value & method;
    return false;
}

std::string ServerConfig::getCgi(const std::string &extension) const
{
    std::map<std::string, std::string>::const_iterator it = cgi_extensions.find(extension);
    if (it != cgi_extensions.end()) {
        return it->value;
    }
    throw NotImplemented();//std::invalid_argument("No CGI handler found for extension: " + extension);
}

ServerConfig::MethodFlag ServerConfig::stringToMethodFlag(const std::string &method)
{
    if (method == "GET") return GET;
    if (method == "POST") return POST;
    if (method == "DELETE") return DELETE;
    throw NotAllowed();//std::invalid_argument("Invalid HTTP method: " + method);
}

std::string ServerConfig::getErrorFile(int errorCode) const
{
    std::map<int, std::string>::const_iterator it = error_files.find(errorCode);
    if (it != error_files.end()) {
        return it->value;
    }
    throw FileNotFound();
}

std::string ServerConfig::getRootFolder() const { return (rootFolder); }

time_t	ServerConfig::getTimeout(void) const { return (timeout); }

bool ServerConfig::isExecFolder(std::string location) const { return (execFolder == location); }

std::string ServerConfig::getIndex() const { return (index_file); }
