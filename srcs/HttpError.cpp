#include <HttpError.hpp>

HttpError::HttpError(const std::string & what, std::string &response) : invalid_argument(what) , response(response) {}

HttpError::HttpError(const std::string & what) : invalid_argument(what), response("") {}

std::string HttpError::getResponse() {	return (response); }