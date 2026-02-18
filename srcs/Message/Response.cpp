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
		msg += "HTTP/1.1 " + status + "\r\nContent-Type: text/html";
		msg += "\r\nTransfer-Encoding: chunked\r\n";
		this->chunkBody();
		msg += body;
	}
	return msg;
}

std::string toHex(size_t num)
{
	std::string res;
	static std::string hex = "0123456789ABCDEF";
	if (num == 0)
		return "\r\n0\r\n";
	while (num) {
		res = "a" + res;
		res[0] = hex[num % 16];
		num /= 16;
	}
	res = "\r\n" + res + "\r\n";
	return res;
}

void Response::chunkBody()
{
	//here, we chunk by 1024
	std::vector<std::string> ensemble;
	size_t pos = 0;
	while (pos != body.size())
	{
		size_t size = body.length() - pos;
		if (BUF_SIZE < size)
			size = BUF_SIZE;
		ensemble.push_back(toHex(size));
		ensemble.push_back(body.substr(pos, size));
		pos += size;
	}
	body = "";
	for (std::vector<std::string>::iterator it = ensemble.begin(); it != ensemble.end(); it++)
		body += *it;
	body += "\r\n0\r\n\r\n";
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
