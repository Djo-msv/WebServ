#include <HttpError.hpp>

HttpError::HttpError(const std::string & what, int error_code) : invalid_argument(what) , error_code(error_code) {}

HttpError::HttpError(const std::string & what) : invalid_argument(what), error_code(0) {}

int HttpError::getError() {	return (error_code); }