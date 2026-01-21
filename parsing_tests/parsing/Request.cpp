#include "Request.hpp"

Request::Request() : _env(NULL)
{
	std::cout << "Request default constructor called\n";
}

Request::~Request()
{
	delete[] _env;
	std::cout << "Request destructor called\n";
}

Request::Request(const std::string &request)//, ServerSocket *server)
{
	std::cout << "Request constructor called\n";
	//here parsing the request ::
		//startline first ::
			//methodx
			//targetx
			//http versionx
		//then headers ::
			//content-type accepted
			//content-length::transfer-encoding (is there a body ? also parsing changes)
			//maybe specific user agents authorized ?
		//then body, if body :: if chunked, do that
	std::stringstream s(request);
	std::string line;
	getline(s, line);
	//startline
	try {
		this->startline_check(line);//, server);
		//no header or body read for now, just regular old get requests :: start-line only
		//this->headers_check(line, server); ->later, since the headers are sort of irrelevant rn
	}
	catch (std::exception &e) {throw ;}
	
}

Request::Request(const Request &other) : _method(other._method), _target(other._target) , _env(other._env)
{
	std::cout << "Request copy constructor called\n";
}

Request& Request::operator=(const Request &other)
{
	if (this != &other)
	{
		_method = other._method;
		_env = other._env;
		_target = other._target;
		std::cout << "Request assignment operator called\n";
	}
	return *this;
}

void Request::startline_check(std::string line)//, ServerSocket *server)
{
	std::cout << "show me line : " << line;
	std::stringstream l(line);
	std::string current;
	getline(l, current, ' ');
	_method = current;
	//method check here against authorized in server
	/*if (!isin(server->methods, _method))
		throw std::exception(); //unauthorized method*/
	if (l.eof())
		throw std::out_of_range("1"); //bad request
	getline(l, current, ' ');
	_target = current;
	//check if target has a query
	if (_target.find("?") != std::string::npos)
	{
		current = _target.substr(_target.find("?") + 1);
		//check current for the key=value&key=value standard, then translate into an environment char **tab
		/*if (!check_params(current))
			throw std::exception(); //bad request ?? bad query formatting*/
		_env = make_env(current);
		_target = _target.substr(0, _target.find("?"));
	}
	else
		_env = NULL;
	this->read();
	if (l.eof())
		throw std::out_of_range("2"); //bad request
	getline(l, current, '\r');
	if (current != "HTTP/1.1")
		throw std::out_of_range("3"); //wrong http version -> unauthorized ? not provided ?
	getline(l, current);
	if (!current.empty() && !l.eof())
		throw std::out_of_range("4"); //bad request (formatting) 
}

void Request::read()
{
	std::cout << "this request has method : " << _method << ", target : " << _target << ", and env : {";
	if (_env)
	{
		for (int i = 0; _env[i]; i++)
			std::cout << _env[i] << ";";
	}
	std::cout << "}\n";
}
