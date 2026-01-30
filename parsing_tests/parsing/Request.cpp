#include "Request.hpp"

Request::Request() : _env(NULL), c_env(NULL), env_size(0), exec(false) {}

Request::~Request()
{
	delete[] _env;
	delete[] c_env;
}

Request::Request(const Request &other) : _method(other._method), _target(other._target), _env(other._env), _body(other._body), c_env(other.c_env), exec(other.exec) {}

Request& Request::operator=(const Request &other)
{
	if (this != &other)
	{
		_method = other._method;
		_env = other._env;
		c_env = other.c_env;
		_target = other._target;
		exec = other.exec;
		_body = other._body;
	}
	return *this;
}

Request &Request::operator+=(const char *buffer)
{
	_request += buffer;
	return *this;
}

void Request::parse()
{
	std::string header;
	std::size_t headerEnd = _request.find("\r\n\r\n");

	if (headerEnd == std::string::npos)
		throw std::out_of_range("0"); //no header-delim, bad request/connection lost
	if (headerEnd + 4 >= _request.size())
		header = _request;
	else
	{
		header = _request.substr(0, headerEnd);
		_body = _request.substr(headerEnd + 4);
	}
	try {
		this->parse_header(header);
		this->parse_body();
	}
	catch (std::exception &e) {throw ;}
}

void Request::body_check(int diff)
{
	if (diff < 0)
		throw std::out_of_range("7"); //additional data
	if (diff > 0)
		throw std::out_of_range("8"); //missing data
}

void Request::parse_body()
{
	try {
		switch (this->getSize()) {
			case CHUNKED:
				//this->chunk_parse(); //future chunked parse with size->substr loop()
				break ;
			case 0:
				if (!_body.empty())
					throw std::out_of_range("6"); //there should be a content-length
				break ;
			default:
				this->body_check(this->getSize() - _body.size());
		}
	}
	catch (std::exception &e) { throw ; }
}

void Request::adjust_exec()
{
	//method check here against authorized in server at the location + adjusting for exec/get/post/delete
	if (_target.substr(0, 9) == "/scripts/" || _target.substr(0, 8) == "scripts/")
		exec = true;
}

void	Request::parse_header(std::string header)
{
	std::stringstream s(header);
	std::string line;
	std::getline(s, line, '\n');
	try {
		this->startline_check(line);
		while (!s.eof())
		{
			std::getline(s, line, '\n');
			this->headers_add(line);
		}
	}
	catch (std::exception &e) {throw ;}
}

void Request::headers_add(std::string line)
{
	std::stringstream s(line);
	std::string key;
	std::string val;
	
	std::cout << line << std::endl;
	std::getline(s, key, ':');
	std::getline(s, val, '\r');
	std::cout << val << std::endl;
	// should throw error 400 bad request, error case are when there is no ':', no char before or after ':'
	if (key.empty() || val.empty())
		throw BadRequest();
	// Happens is if there is a \r in the value 
	if (!s.eof())
		throw BadRequest();
	try {
		//here checking the a-num values
		//check_key(key); -> that'll be error 5
		if (val[0] == ' ')
			val.erase(val.begin());
		headers.insert(std::pair<std::string, std::string>(key, val));
	}
	catch (std::exception &e) {throw;}
}

int Request::getSize() const
{
	if (headers.count("Content-Length"))
		return atoi((headers.at("Content-Length")).c_str());
	if (headers.count("Transfer-Encoding"))
		return CHUNKED;
	return 0;
}

void Request::startline_check(std::string line)
{
	std::stringstream l(line);
	std::string current;
	getline(l, current, ' ');
	_method = current;
	if (l.eof())
		throw std::out_of_range("1"); //bad request
	getline(l, current, ' ');
	_target = current;
	//check if target has a query
	if (_target.find("?") != std::string::npos)
	{
		current = _target.substr(_target.find("?") + 1);
		//check current for the key=value&key=value standard
		/*if (!check_params(current))
			throw std::exception(); //bad request ?? bad query formatting*/
		//translate into an environment string and const char **tab
		this->make_env(current);
		_target = _target.substr(0, _target.find("?"));
	}
	if (l.eof())
		throw std::out_of_range("2"); //bad request
	getline(l, current, '\r');
	if (current != "HTTP/1.1")
		throw std::out_of_range("3"); //wrong http version -> unauthorized ? not provided ?
	getline(l, current);
	if (!current.empty() && !l.eof())
		throw std::out_of_range("4"); //bad request (formatting)
	this->adjust_exec();
	_target = "." + _target;
}

void Request::make_env(std::string params)
{
	std::vector<std::string> tab1;
	std::stringstream s(params);
	std::string param;
	while (!s.eof())
	{
		getline(s, param, '&');
		tab1.push_back(param);
	}
	_env = new std::string[tab1.size()];
	for (std::vector<std::string>::iterator it = tab1.begin(); it != tab1.end(); it++)
	{
		_env[env_size] = *it;
		env_size++;
	}
	c_env = new const char*[env_size + 1];
	for (size_t j = 0; j < env_size; j++)
		c_env[j] = _env[j].c_str();
	c_env[env_size] = NULL;
}

const char **Request::getEnv() const { return c_env; }

std::string Request::getTarget() const { return _target; }

std::string Request::getMethod() const { return _method; }

bool Request::isExec() const { return exec; }

std::string Request::getBody() const { return _body; }

void Request::read() const
{
	std::cout << "this request ";
	if (exec)
		std::cout << "needs execution";
	else
		std::cout << "needs no execution";
	std::cout << ", has method : " << _method << ", target : " << _target << ", env : {";
	if (_env)
	{
		for (size_t i = 0; i < env_size; i++)
			std::cout << _env[i] << ";";
	}
	std::cout << "}, and map ::\n";
	if (!headers.empty())
	{
		for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); it++)
			std::cout << it->first << "; " << it->second << std::endl;
	}
	std::cout << "and the body" << std::endl << _body << std::endl;
}

Request::BadRequest::BadRequest() : HttpError("HTTP/1.1 400 Bad request\r\n"
											  "Content-length: 90\r\n"
											  "Cache-Control: no-cache\r\n"
											  "Connection: close\rn"
											  "Content-Type: text/html\r\n"
											  "\r\n"
											  "<html><body><h1>400 Bad request</h1>\n"
											  "Your browser sent an invalid request.\n"
											  "</body></html>", 400) {}