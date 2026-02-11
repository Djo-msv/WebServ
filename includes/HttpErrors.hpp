#pragma once

# include <HttpError.hpp>


class GatewayTimeout : public HttpError {
    public :
        GatewayTimeout() : HttpError("504 Gateway Time-out", "~/html/errors/504_def.html", 504) {};
};

class FileNotFound : public HttpError {
    public :
        FileNotFound() : HttpError("404 Not Found", "~/html/errors/404_def.html", 404) {};
};

class Forbidden : public HttpError {
    public :
        Forbidden() : HttpError("403 Forbidden", "~/html/errors/403_def.html", 403) {};
};

class InternalServerError : public HttpError {
    public :
        InternalServerError() : HttpError("500 Internal Server Error", "~/html/errors/500_def.html", 500) {};
};


/*
    200 OK
    400 Bad Request
	403 Forbidden
    404 Not Found
	500 Internal Server Error
    501 Not Implemented (method isn't allowed for this server/location, encoding isn't handled, etc.)
    414 URI Too Long
    301 Moved Permanently
    204 No Content
    411 Length Required (message body but no length provided)
*/