#pragma once

#include <stdexcept>

class HttpError : public std::invalid_argument
{
	public :
		HttpError(const std::string &what);
		HttpError(const std::string &what, std::string &response);

		std::string getResponse();
	private : 
		std::string response;
};