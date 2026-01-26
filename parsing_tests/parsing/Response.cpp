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
	try {
		this->seekTarget(req);
		this->makeBody();
		//classic recipe here for a static webpage response
		msg += " " + status + "\r\n" + "Content-Type: text/html\r\nContent-Length: " + ft_itoa(body.length() - 2) + "\r\n";
		msg += body;
	}
	catch (std::exception &e) {throw ;}
}

std::string Response::getResponse() const { return msg; }

void Response::seekTarget(Request *req)
{
<<<<<<< HEAD
	struct stat buf;
	if (stat(_target.c_str(), &buf) == -1)
=======
	//check if file exists
	struct stat buf;
	if (stat(req->getTarget().c_str(), &buf) == -1)
>>>>>>> refs/remotes/origin/ExecutionProcess
	{
		this->fix_error("404 Not Found"); //update target and status accordingly
		return ;
	}
	_target = req->getTarget();
	if (req->isExec())
	{
		//handle execution here, acquire new target, hand it to _target component;
		const char **args = new const char*[2];
		args[0] = _target.c_str();
		args[1] = NULL;
		try {
			ProcessExecution obj;
			obj.startProcess((char **)args, (char **)req->getEnv());
			while (!obj.getStatus())
				obj.readDataProcess();
			_target = obj.getResponse();
			if (_target.length() <= 2)
				std::cout << std::endl << "error from script : " << _target << std::endl << std::endl;
			_target = "." + _target;
		}
		catch (std::exception &e) {delete[] args; throw ;}
		delete[] args;
	}
}

void Response::makeBody()
{
	std::ifstream file(_target.c_str());
	if (!file.is_open())
	{
		//later this will likely be more of a "forbidden" type error
		/*this->fix_error("404 Not Found");
		_target = "." + _target;
		this->makeBody();*/
		body += "<html><body><h1>Error opening the file requested !</h1></body></hmtl>";
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
