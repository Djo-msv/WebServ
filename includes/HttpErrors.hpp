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

class NotAllowed : public HttpError {
    public:
        NotAllowed() : HttpError("405 Method Not Allowed", "./errors/405_def.html", 405) {};
};

class LengthRequired : public HttpError {
    public:
        LengthRequired() : HttpError("411 Length Required", "./errors/411_def.html", 411) {};
};

class TooLarge : public HttpError {
    public:
        TooLarge() : HttpError("413 Content Too Large", "./errors/413_def.html", 413) {};
};

/*
    next up :: 301 Moved Permanently
*/
