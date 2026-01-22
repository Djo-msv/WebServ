#include "Response.hpp"

Response::Response() : msg("HTTP/1.1"), status("200 OK"), body("\r\n") {}

Response::~Response() {}

Response::Response(const Response &other) : _target(other._target), msg(other.msg), status(other.status), body(other.body) {}

Response& Response::operator=(const Response &other)
{
	if (this != &other)
	{
		_target = other._target;
		msg = other.msg;
		status = other.status;
		body = other.body;
	}
	return *this;
}

void Response::makeResponse(Request *req)
{
	this->seekTarget(req);
	this->makeBody();
}

void Response::seekTarget(Request *req)
{
	struct stat restrict buf;
	if (stat(_target.c_str(), &buf) == -1)
	{
		this->fix_error("404 Not Found"); //update target and status accordingly
		return ;
	}
	if (req->isExec())
	{
		//handle execution here, acquire new target, hand it to _target component;
	}
	else
		_target = req->getTarget();
}

void Response::makeBody()
{
	std::ifstream file(_target.c_str());
	if (!file.is_open())
	{
		this->fix_error("404 Not Found");
		this->makeBody();
	}
	std::stringstream buffer;
	buffer << file.rdbuf();
	file.close();
	body = buffer.str();
}

void Response::fix_error(std::string error)
{
	//in the future i would check the error code against the map of server configed error files
	//    (ex: if (server_errors.count(error.substr(0, 3))) { status = error; _target = server_errors.at(error.substr(0, 3); })
	//if the specific error isn't a match, switch back to default error page
	//for now it's just the error 404 basic stuff
	status = error;
	_target = "/html/error_404.html";
}
