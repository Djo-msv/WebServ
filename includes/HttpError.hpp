#pragma once

#include <stdexcept>

class HttpError : public std::invalid_argument
{
	public :
		HttpError(const std::string &what);
		HttpError(const std::string &what, int error_code);

		int getError();
	private : 
		int error_code;
};