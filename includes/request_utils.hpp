#pragma once


#include <Request.hpp>



std::string	extractCgi(std::string &file, ServerConfig &config);
std::string	seekErrorFile(HttpError &error, ServerConfig &config);


std::string	seekFile(std::string &pathfile);