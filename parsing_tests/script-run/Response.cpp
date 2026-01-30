#include "Response.hpp"

Response::Response() : msg("HTTP/1.1"), status("200 OK"), body("\r\n"), _status(2), exec(NULL) {}

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
	try {
		this->seekTarget(req);
		//this if will now only come into effect if there is no exec to be parsed through :: ie, _status does not switch to 1 or 0
		if (_status == 2)
			this->makeBody();
	}
	catch (std::exception &e) {throw ;}
}

std::string Response::getResponse()
{
	//classic recipe here for a static webpage response
	msg += " " + status + "\r\n" + "Content-Type: text/html\r\nContent-Length: " + ft_itoa(body.length() - 2) + "\r\n";
	msg += body;
	return msg;
}

void Response::seekTarget(Request *req)
{
	//check if file exists
	struct stat buf;
	if (stat(req->getTarget().c_str(), &buf) == -1)
	{
		this->fix_error("404 Not Found"); //update target and status accordingly
		return ;
	}
	_target = req->getTarget();
	if (req->isExec())
	{
		//handle execution here
		try {
			exec = new ProcessExecution;
			if (req->getBody().empty())
				_status = 0; // no writing needed, read directly from pipeOut once exec is running
			else
			{
				body = req->getBody();
				_status = 1; // next up, write body to pipeIn to exec running
			}
			//no more need for a body in startProcess, since that'll be sent directly through this response instead
			exec->startProcess(!body.empty(), req->getTarget(), req->getEnv());
		}
		catch (std::exception &e) {throw ;}
	}
}
//a "take_action" function that checks in on the exec
void Response::actionExec()
{
	switch (_status) {
		case 1:
			writeExec();
			break ;
		case 0:
			readExec();
	}
}
//remember, no writeExec() if we don't have a body to send to the cgi-exec
void Response::writeExec()
{
	if (!exec)
	{
		_status = 2;
		return ;
	}
	exec->writeDataProcess(body);
	_status = 0;
}

void Response::readExec()
{
	if (!exec)
	{
		_status = 2;
		return ;
	}
	exec->readDataProcess();
	if (exec->getStatus())
	{
		_status = 2;
		body = exec->getResponse();
		delete exec;
		exec = NULL;
	}
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

unsigned int Response::getStatus() const { return _status; }

int Response::getFd() const
{
	switch (_status) {
		case 0:
			return exec->getFdOut();
		case 1:
			return exec->getFdIn();
		default:
			return -1;
	}
}
