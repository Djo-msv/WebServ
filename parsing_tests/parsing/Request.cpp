#include "Request.hpp"

Request::Request() : _request(""), _env(NULL), c_env(NULL), env_size(0)
{
	std::cout << "Request default constructor called\n";
}

Request::~Request()
{
	delete[] _env;
	delete[] c_env;
	std::cout << "Request destructor called\n";
}

Request::Request(const std::string &request) : _request(request), _env(NULL), c_env(NULL), env_size(0)
{
	std::cout << "Request constructor called\n";
}

Request::Request(const Request &other) : _request(other._request), _method(other._method), _target(other._target), _env(other._env), c_env(other.c_env)
{
	std::cout << "Request copy constructor called\n";
}

Request& Request::operator=(const Request &other)
{
	if (this != &other)
	{
		_method = other._method;
		_request = other._request;
		_env = other._env;
		c_env = other.c_env;
		_target = other._target;
		std::cout << "Request assignment operator called\n";
	}
	return *this;
}

void Request::check_request()
{
	if (_request.empty())
		throw std::out_of_range("request empty !\n");
	//here parsing the request ::
		//startline first ::x
			//method x
			//target x
			//http version x
		//then headers ::
			//content-type accepted
			//content-length::transfer-encoding (is there a body ? also parsing changes)
			//maybe specific user agents authorized ?
		//then body, if body :: if chunked, do that
	std::stringstream s(_request);
	std::string line;
	getline(s, line);
	//startline
	try {
		this->startline_check(line);
		//header-reading :: the basics
		//      the map first :: make pairs with ':' delim (checkers that its all alphanumerical, i think)
		//      then the getters :: spec getSize() for now
		while (!s.eof() && !line.empty() && line != "\r")
		{
			getline(s, line);
			this->headers_add(line);
		}
		//this->headers_check(line); ->later, since the headers are sort of irrelevant rn
	}
	catch (std::exception &e) {throw ;}
}

void Request::headers_add(std::string line)
{
	std::stringstream s(line);
	std::string key;
	std::string val;
	getline(s, key, ':');
	getline(s, val, '\r');
	if (key.empty() || val.empty())
		return ;
	try {
		//here checking the a-num values
		//check_key(key);
		if (val[0] == ' ')
			val = val.substr(1);
		headers.insert(std::pair<std::string, std::string>(key, val));
	}
	catch (std::exception &e) {throw;}
}

int Request::getSize() const
{
	if (headers.count("Content-Length"))
		return atoi((headers.at("Content-Length")).c_str());
	if (headers.count("Transfer-Encoding"))
		return -1;
	return 0;
}

void Request::startline_check(std::string line)
{
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

const char **Request::getEnv() const
{
	return c_env;
}
std::string Request::getTarget() const
{
	return _target;
}

std::string Request::getMethod() const
{
	return _method;
}

void Request::read() const
{
	std::cout << "this request has method : " << _method << ", target : " << _target << ", env : {";
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
}
