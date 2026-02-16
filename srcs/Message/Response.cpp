#include "Response.hpp"

Response::Response() : status("200 OK"), body("\r\n"), exec(false) {}

Response::~Response() {}

Response::Response(const Response &other) : _target(other._target), msg(other.msg), status(other.status), body(other.body), exec(other.exec) {}

void Response::clear()
{
	_target = "";
	msg = "";
	status = "200 OK";
	body = "\r\n";
	exec = false;
}

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
	exec = req->isExec();
	_target = req->getTarget();
	//this if will now only come into effect if there is : no exec || error
	if (!exec)
		this->readFile();
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
	//std::cout << "sending :: \n" << msg << std::endl << std::endl;
	return msg;
}

void Response::readFile()
{
	if (_target.empty()) //no body to make || body is from default
		return ;
	std::ifstream file(_target.c_str());
	if (!file.is_open()) // should not happen ever at this point, but in case
		return ;
	std::stringstream buffer;
	buffer << file.rdbuf();
	file.close();
	body += buffer.str();
}

void Response::fix_error(HttpError &error, ServerConfig &s)
{
	status = error.what();
	try { _target = seekErrorFile(error, s); this->readFile(); }
	catch (InternalServerError &e) {status = e.what(); _target = ""; body += e.getDefaultFile();}
}
