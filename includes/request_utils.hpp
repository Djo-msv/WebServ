#pragma once


# include <HttpErrors.hpp>
# include <ServerConfig.hpp>
# include <sys/stat.h>


std::string	extractCgi(std::string &file, ServerConfig &config);
std::string	seekErrorFile(HttpError error, ServerConfig &config);


std::string	seekFile(std::string &pathfile);