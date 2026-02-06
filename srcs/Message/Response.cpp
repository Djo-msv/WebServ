#include "Response.hpp"

Response::Response() : status("200 OK"), body("\r\n"), exec(false) {}

Response::~Response() {}

Response::Response(const Response &other) : _target(other._target), msg(other.msg), status(other.status), body(other.body), exec(other.exec) {}

Response& Response::operator=(const Response &other)
{
	if (this != &other)
	{
		_target = other._target;
		msg = other.msg;
		status = other.status;
		body = other.body;
		exec = other.exec;
	}
	return *this;
}

Response &Response::operator+=(const char *buffer)
{
	msg += buffer;
	return *this;
}

bool Response::makeResponse(Request *req)
{
	try {
		exec = req->isExec();
		this->seekTarget(req);
		//this if will now only come into effect if there is no exec || exec script can't be found (or execed ?)
		if (!exec)
			this->makeBody();
	}
	catch (std::exception &e) {throw ;}
	return exec;
}

std::string Response::getResponse()
{
	//classic recipe here for a static webpage response
	if (msg.empty())
	{
		msg += "HTTP/1.1 " + status + "\r\n" + "Content-Type: text/html\r\nContent-Length: " + ft_itoa(body.length() - 2) + "\r\n";
		msg += body;
	}
	return msg;
}

void Response::seekTarget(Request *req)
{
	//check if file exists
	struct stat buf;
	if (stat(req->getTarget().c_str(), &buf) == -1)
	{
		this->fix_error("404 Not Found"); //update target and status accordingly
		exec = false;
		return ;
	}
	_target = req->getTarget();
}

void Response::makeBody()
{
	std::ifstream file(_target.c_str());
	if (!file.is_open())
	{
		//later this will likely be more of a "forbidden" type error
		body += "<html><body><h1>Error opening the file requested !</h1></body></hmtl>";
		return ;
	}
	std::stringstream buffer;
	buffer << file.rdbuf();
	file.close();
	body += buffer.str();
}

void Response::fix_error(std::string error)
{
	//in the future i would check the error code against the map of server configed error files
	//    (ex: if (server_errors.count(error.substr(0, 3))) { status = error; _target = server_errors.at(error.substr(0, 3); })
	//if the specific error isn't a match, switch back to default error page
	//for now it's just the error 404 basic stuff
	status = error;
	_target = "./html/error_404.html";
}
