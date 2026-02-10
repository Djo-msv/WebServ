#pragma once

#include <stdexcept>

class HttpError : public std::invalid_argument
{
	public :
		HttpError(const std::string &what);
		HttpError(std::string what, std::string defaultFile, int errorCode);
		~HttpError() throw() {};

		std::string	getDefaultFile() const;
		int			getErrorCode() const;
	private : 
		std::string	defaultFile;
		int			errorCode;
};
