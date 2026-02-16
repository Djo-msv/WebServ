#pragma once


# include <HttpErrors.hpp>
# include <ServerConfig.hpp>
# include <Request.hpp>
# include <sys/stat.h>
# include <cstdio>


std::string	extractCgi(std::string &file, ServerConfig &config);
std::string	seekErrorFile(HttpError error, ServerConfig &config);
std::string	seekFile(std::string &pathfile);

std::string	chunk_parse(std::string _body);
bool check_key(std::string key);
bool check_val(std::string &val);
