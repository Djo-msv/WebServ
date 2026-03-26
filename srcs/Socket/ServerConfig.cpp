#include <ServerConfig.hpp>

ServerConfig::ServerConfig(int port, location root_location, std::map<std::string, location *> locations,
			std::map<std::string, std::string> cgi_extensions, std::string exec_folder, std::map<int, std::string> error_files, time_t timeout) :
		sin_family(AF_INET), sin_port(port), root_location(root_location), locations(locations),
		cgi_extensions(cgi_extensions), error_files(error_files), exec_folder(exec_folder), timeout(timeout) {}

ServerConfig::~ServerConfig() {}

bool ServerConfig::isMethodAllowed(std::list<std::string> &full, int method) const
{
    std::map<std::string, location *>::const_iterator it;
    std::string loc;

    for (std::list<std::string>::const_iterator itt = full.begin(); itt != full.end(); itt++) {
        if (!loc.empty() && (it = locations.find(*itt + loc)) != locations.end() && it->value->allowed_methods)
            return it->value->allowed_methods & method;
        if ((it = locations.find(*itt)) != locations.end() && it->value->allowed_methods)
            return it->value->allowed_methods & method;
        if ((*itt).find('.') != std::string::npos) {
            std::string ext = (*itt).substr((*itt).rfind('.'));
            if ((it = locations.find(ext)) != locations.end() && it->value->allowed_methods
                    && it->value->allowed_methods & method)
                return true;
        }
        loc = *itt;
    }
    if (root_location.allowed_methods)
        return root_location.allowed_methods & method;
    return false;
}

std::string ServerConfig::getIndex(std::list<std::string> &full) const
{
    std::map<std::string, location *>::const_iterator it;
    std::string loc;

    for (std::list<std::string>::const_iterator itt = full.begin(); itt != full.end(); itt++) {
        if (itt->empty()) { continue; }
        if (!loc.empty() && (it = locations.find(*itt + loc)) != locations.end() && !it->value->index.empty())
            return it->value->index;
        if ((it = locations.find(*itt)) != locations.end() && !it->value->index.empty())
            return it->value->index;
        loc = *itt;
    }
    if (!root_location.index.empty())
        return root_location.index;
    return "";
}

std::string ServerConfig::getFullPath(std::list<std::string> &full) const
{
    std::string path;
    std::string back;
    if (!full.empty()) { back = full.back(); }
    if (full.size() > 1) {
        for (std::list<std::string>::const_iterator itt = full.begin(); itt != --full.end(); itt++)
            path = *itt + path;
    }
    std::map<std::string, location *>::const_iterator it;
    if ((it = locations.find(back)) != locations.end()) { path = it->value->root + path; }
    else { path = root_location.root + back + path; }
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

std::string ServerConfig::getRootFolder() const { return (root_location.root); }

time_t	ServerConfig::getTimeout(void) const { return (timeout); }

bool ServerConfig::isExecFolder(std::list<std::string> &full, int method) const
{
    if (full.empty()) { return false; }
    for (std::list<std::string>::const_iterator itt = full.begin(); itt != full.end(); itt++) {
        std::string extension = *itt;
        if (extension.find('.') != std::string::npos) {
            extension = extension.substr(extension.rfind('.'));
            if (!extension.empty() && extension == exec_folder && locations.count(extension)
                    && locations.at(extension)->allowed_methods && locations.at(extension)->allowed_methods & method)
                return true;
        }
        if (*itt == exec_folder) {  return true; }
    }
    return false;
}
