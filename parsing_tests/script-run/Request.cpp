#include "Request.hpp"

Request::Request() : _env(NULL), c_env(NULL), exec(false) {}

Request::~Request() {if (_env) { delete[] _env; delete[] c_env; } }

Request::Request(const Request &other) : _method(other._method), _target(other._target), _body(other._body), _query(other._query), _env(other._env), c_env(other.c_env), exec(other.exec) {}

Request& Request::operator=(const Request &other)
{
	if (this != &other)
	{
		_method = other._method;
		_query = other._query;
		_target = other._target;
		exec = other.exec;
		_body = other._body;
		c_env = other.c_env;
		_env = other._env;
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
	{
		exec = true;
		//first we should add relevant variables :: cgi version, redirect status, query string, method request, etc.
		headers.insert(std::pair<std::string, std::string>("REDIRECT_STATUS", "true"));
		headers.insert(std::pair<std::string, std::string>("GATEWAY_INTERFACE", "CGI/1.1"));
		std::string filename = _target;
		if (_target.rfind('/') != std::string::npos)
			filename = _target.substr(_target.rfind('/') + 1);
		headers.insert(std::pair<std::string, std::string>("SCRIPT_FILENAME", filename));
		headers.insert(std::pair<std::string, std::string>("REQUEST_METHOD", _method));
		if (!_query.empty())
			headers.insert(std::pair<std::string, std::string>("QUERY_STRING", _query));
		//here creating the char * environment which we can use for execve, in two steps as previously
		_env = new std::string[headers.size()];
		c_env = new const char*[headers.size() + 1];
		size_t i = 0;
		for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); it++)
		{
			_env[i] = it->first + "=" + it->second;
			c_env[i] = _env[i].c_str();
			i++;
		}
		c_env[i] = NULL;
	}
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
	
	std::getline(s, key, ':');
	std::getline(s, val, '\r');
	if (key.empty() || val.empty())
		return ;
	try {
		//here checking the a-num values
		//check_key(key); -> that'll be the header parsing error
		if (val[0] == ' ')
			val.erase(val.begin());
		//turning 'Content-Length' into 'CONTENT_LENGTH' for future environment and lack of case-conflict
		std::transform(key.begin(), key.end(), key.begin(), ::toupper);
		size_t n = key.find('-');
		while (n != std::string::npos)
		{
			key[n] = '_';
			n = key.find('-');
		}
		headers.insert(std::pair<std::string, std::string>(key, val));
	}
	catch (std::exception &e) {throw;}
}

int Request::getSize() const
{
	if (headers.count("CONTENT_LENGTH"))
		return atoi((headers.at("CONTENT_LENGTH")).c_str());
	if (headers.count("TRANSFER_ENCODING"))
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
		_query = _target.substr(_target.find("?") + 1);
		//check query for the key=value&key=value standard ?
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

std::string Request::getTarget() const { return _target; }

std::string Request::getMethod() const { return _method; }

std::string Request::getQuery() const { return _query; }

char **Request::getEnv() const { return (char **)c_env; }

bool Request::isExec() const { return exec; }

std::string Request::getBody() const { return _body; }

void Request::read() const
{
	std::cout << "this request ";
	if (exec)
		std::cout << "needs execution";
	else
		std::cout << "needs no execution";
	std::cout << ", has method : " << _method << ", target : " << _target << ", and map ::\n";
	if (!headers.empty())
	{
		for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); it++)
			std::cout << it->first << "; " << it->second << std::endl;
	}
	std::cout << "and the body" << std::endl << _body << std::endl;
}
