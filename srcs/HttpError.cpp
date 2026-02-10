#include <HttpError.hpp>

HttpError::HttpError(std::string what, std::string defaultFile, int errorCode) : invalid_argument(what), 
	defaultFile(defaultFile), errorCode(errorCode) {}

HttpError::HttpError(const std::string & what) : invalid_argument(what), defaultFile("") {}

std::string HttpError::getDefaultFile() const { return (defaultFile); }

int HttpError::getErrorCode() const { return (errorCode); }
