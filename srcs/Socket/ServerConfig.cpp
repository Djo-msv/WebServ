#include <ServerConfig.hpp>

ServerConfig::ServerConfig(int port, location root_location, std::map<std::string, location *> locations,
			std::map<std::string, std::string> cgi_extensions, std::string exec_folder, std::map<int, std::string> error_files, time_t timeout) :
		sin_family(AF_INET), sin_port(port), root_location(root_location), locations(locations),
		cgi_extensions(cgi_extensions), error_files(error_files), timeout(timeout) {}

ServerConfig::~ServerConfig() {}

bool ServerConfig::isMethodAllowed(std::list<std::string> &full, int method) const
{
    std::map<std::string, location>::const_iterator it;
    std::string loc;

    for (std::list<std::string>::const_iterator itt = full.begin(); itt != full.end(); itt++) {
        if (!loc.empty() && (it = locations.find(*itt + loc)) != locations.end() && it->value->allowed_methods)
            return it->value->allowed_methods & method;
        if ((it = locations.find(*itt)) != locations.end() && it->value->allowed_methods)
            return it->value->allowed_methods & method;
        loc = *itt;
    }
    if (root_location.allowed_methods)
        return root_location.allowed_methods & method;
    return false;
}

std::string ServerConfig::getIndex(std::list<std::string> &full) const
{
    std::map<std::string, location>::const_iterator it;
    std::string loc;

    for (std::list<std::string>::const_iterator itt = full.begin(); itt != full.end(); itt++) {
        if (itt->empty()) { continue; }
        if (!loc.empty() && (it = locations.find(*itt + loc)) != locations.end() && !it->value->index.empty())
            return it->value->index;
        if ((it = locations.find(*itt)) != locations.end() && !it->value->index.empty())
            return it->value->index;
        loc = *itt;
    }
    if (root_location.allowed_methods)
        return root_location.index;
}

std::string ServerConfig::getFullPath(std::list<std::string> &full) const
{
    std::string path;
    std::map<std::string, location>::const_iterator it;
    for (std::list<std::string>::const_iterator itt = ++full.begin(); itt != full.end(); itt++) {
        if ((it = locations.find(*itt)) != locations.end()) { path = it->value->path + path; }
        else { path = *itt + path; }
    }
    return path;
}

std::string ServerConfig::getCgi(const std::string &extension) const
{
    std::map<std::string, std::string>::const_iterator it = cgi_extensions.find(extension);
    if (it != cgi_extensions.end()) {
        return it->value;
    }
    throw NotImplemented();
}

ServerConfig::MethodFlag ServerConfig::stringToMethodFlag(const std::string &method)
{
    if (method == "GET") return GET;
    if (method == "POST") return POST;
    if (method == "DELETE") return DELETE;
    throw NotAllowed();
}

std::string ServerConfig::getErrorFile(int errorCode) const
{
    std::map<int, std::string>::const_iterator it = error_files.find(errorCode);
    if (it != error_files.end()) {
        return it->value;
    }
    throw FileNotFound();
}

std::string ServerConfig::getRootFolder() const { return (root_location.path); }

time_t	ServerConfig::getTimeout(void) const { return (timeout); }

bool ServerConfig::isExecFolder(std::list<std::string> &full) const
{
    for (std::list<std::string>::const_iterator itt = full.begin(); itt != full.end(); itt++) {
        if (*itt == exec_folder) {  return true; }
    }
    return false;
}
