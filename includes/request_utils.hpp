#pragma once


# include <HttpErrors.hpp>
# include <ServerConfig.hpp>
# include <Request.hpp>
# include <list>
# include <sys/stat.h>
# include <cstdio>


std::string	extractCgi(std::string &file, ServerConfig &config);
std::string	seekErrorFile(HttpError error, ServerConfig &config);
void	seekFile(std::string &pathfile, std::string method);

ustring	chunk_parse(ustring &_body, ustring &new_body);
bool check_key(std::string key);
bool check_val(std::string &val);
std::list<std::string> target_list(std::string loc);
std::string get_path_info(std::string loc);
