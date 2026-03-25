#pragma once

# include <HttpError.hpp>


class GatewayTimeout : public HttpError {
    public :
        GatewayTimeout() : HttpError("504 Gateway Time-out", "./errors/504_def.html", 504) {};
};

class FileNotFound : public HttpError {
    public :
        FileNotFound() : HttpError("404 Not Found", "./errors/404_def.html", 404) {};
};

class Forbidden : public HttpError {
    public :
        Forbidden() : HttpError("403 Forbidden", "./errors/403_def.html", 403) {};
};

class InternalServerError : public HttpError {
    public :
        InternalServerError() : HttpError("500 Internal Server Error", "<html><body><h1>A server error has occured internally !</h1></body></hmtl>", 500) {};
};

class NotImplemented : public HttpError {
    public:
        NotImplemented() : HttpError("501 Not Implemented", "./errors/501_def.html", 501) {};
};

class BadRequest : public HttpError {
    public:
        BadRequest() : HttpError("400 Bad Request", "./errors/400_def.html", 400) {};
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
