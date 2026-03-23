#include "Request.hpp"

Request::Request(ServerConfig &s) : _config(s), _env(NULL), _status(0), c_env(NULL), c_body(NULL), exec(false) {}

Request::~Request() {if (_env) { delete[] _env; delete[] c_env; } if (c_body) { delete[] c_body;} }

Request::Request(const Request &other) : _config(other._config), _request(other._request), _body(other._body), \
					_method(other._method), _cgi(other._cgi), _target(other._target), \
					_query(other._query), _env(other._env), c_env(other.c_env), \
					c_body(other.c_body), exec(other.exec) {}

Request& Request::operator=(const Request &other)
{
	if (this != &other)
	{
		_config = other._config;
		_request = other._request;
		_body = other._body;
		_method = other._method;
		_cgi = other._cgi;
		_query = other._query;
		_target = other._target;
		exec = other.exec;
		if (_env) {
			delete[] c_env;
			delete[] _env;
		}
		if (c_body)
			delete[] c_body;
		_env = other._env;
		c_env = other.c_env;
		c_body = other.c_body;
		headers.clear();
		for (std::map<std::string, std::string>::const_iterator it = other.headers.begin(); it != other.headers.end(); it++)
			headers.insert(*it);
	}
	return *this;
}

//replaces the += overload for non-NULL terminated buffers
void Request::add(const unsigned char *buffer, size_t size)
{
	ustring &ref = _request;
	if (_status)
		ref = _body;
	for (size_t i = 0; i != size; i++)
		ref.push_back(buffer[i]);
}
//clear func, self exp
void Request::clear()
{
	_request.clear();
	_body.clear();
	if (c_body)
		delete[] c_body;
	c_body = NULL;
	_method.clear();
	_cgi.clear();
	_target.clear();
	_query.clear();
	if (_env)
	{
		delete[] _env;
		delete[] c_env;
		_env = NULL;
		c_env = NULL;
	}
	_status = 0;
	exec = false;
	headers.clear();
}

		// public getters

bool Request::keepAlive() const
{
	//i think we assume keep alive, but will double check
	//think if that's the logic we should probably switch this bool around to a CloseConnection() bool for better readability
	if (headers.count("CONNECTION") && headers.at("CONNECTION") == "close")
		return false;
	return true;
}

std::string Request::getTarget() const { return _target; }

std::string Request::getMethod() const { return _method; }

std::string Request::getQuery() const { return _query; }

char **Request::getEnv() const { return (char **)c_env; }

bool Request::isExec() const { return exec; }

unsigned char *Request::getBody() const { return c_body; }

std::string Request::getCgi() const { return _cgi; }

ssize_t Request::getSize() const
{
	if (headers.count("CONTENT_LENGTH"))
		return (ssize_t)atol((headers.at("CONTENT_LENGTH")).c_str());
	if (headers.count("TRANSFER_ENCODING") && headers.at("TRANSFER_ENCODING") == "chunked")
		return CHUNKED;
	return 0;
}

//big parse distribution
void Request::parse(std::map<std::string, std::string> &mime)
{
	if (_status) //headers already parsed on a previous run, _env created etc.
	{
		try { this->parse_body(); }
		catch (std::exception &e) { throw ; }
		if (_method == "DELETE")
			throw DeleteRequest(_target);
		if (exec)
			this->create_env();
		else
			this->mime_check(mime);
		return ;
	}
	std::string header;
	std::size_t headerEnd = _request.find((unsigned char *)"\r\n\r\n");

	if (headerEnd == std::string::npos)
		throw MissingData();
	if (headerEnd + 4 == _request.size())
		header = (char *)(_request.c_str());
	else
	{
		header = (char *)(_request.substr(0, headerEnd).c_str());
		_body = _request.substr(headerEnd + 4);
	}
	try {
		this->parse_header(header);
		_status = 1; //headers are parsed with no error
		this->parse_body();
		if (_method == "DELETE")
			throw DeleteRequest(_target);
		if (exec)
			this->create_env();
		else
			this->mime_check(mime);
	}
	catch (MissingData &e) {
		if (!_status)
			this->clear();
		throw ;
	}
	catch (std::exception &e) { throw ; }
}



		//private parsing functions, in chronological order ::

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
			if (line.empty() || line == "\r")
				break ;
			this->headers_add(line);
		}
	}
	catch (std::exception &e) {throw ;}
}

bool Request::needsIndex()
{
	if (*(_target.rbegin()) == '/') { return false; }
	std::string full_target = _config.getRootFolder() + _target;
	struct stat s;
	if ( stat(full_target.c_str(), &s) != 0 ) { throw FileNotFound(); }
	if( s.st_mode & S_IFDIR ) {
		_target += "/";
		return true;
	}
	return false;
}

void Request::startline_check(std::string line)
{
	std::stringstream l(line);
	std::string current;
	getline(l, current, ' ');
	_method = current;
	if (l.eof())
		throw BadRequest();
	getline(l, current, ' ');
	_target = current;
	//check if target has a query
	if (_target.find('?') != std::string::npos)
	{
		_query = _target.substr(_target.find('?') + 1);
		_target = _target.substr(0, _target.find('?'));
	}
	if (l.eof())
		throw BadRequest();
	getline(l, current, '\r');
	if (current != "HTTP/1.1" && current != "HTTP/1.*")
		throw NotImplemented(); //wrong http version -> unauthorized ? not provided ?
	getline(l, current);
	if (!current.empty())
		throw BadRequest();
	
	if (_target[0] != '/')
		_target = "/" + _target;
	
	//location/extension lists
	std::list<std::string> extension;
	std::list<std::string> location = target_list(_target);
	if (_target.rfind('.') != std::string::npos)
		extension.push_back(_target.substr(_target.rfind('.')));
	
	//"/" or non '/' terminated folder to index
	if (_target == "/" || needsIndex())
		_target += _config.getIndex(location);
	// method check
	bool loc = _config.isMethodAllowed(location, _config.stringToMethodFlag(_method));
	bool ext = _config.isMethodAllowed(extension, _config.stringToMethodFlag(_method));
	if (!loc && !ext)
		throw NotAllowed(); //method not supported (NotImplemented ? check needed)
	//add roots and aliases
	_target = _config.getRootFolder() + _config.getFullPath(location);
	//error 404 catch
	try { seekFile(_target); }
	catch (std::exception &e) { throw ; }
	//check_exec, adjust
	if ((loc && _config.isExecFolder(location)) || (ext && _config.isExecFolder(extension)))
		this->adjust_exec();
}

void Request::adjust_exec()
{
	exec = true;
	//looking for cgi executable file
	try { _cgi = extractCgi(_target, _config); }
	catch (std::exception &e) { throw ; }
	//adding relevant variables :: cgi version, redirect status, query string, method request, etc.
	headers.insert(std::pair<std::string, std::string>("REDIRECT_STATUS", "true"));
	headers.insert(std::pair<std::string, std::string>("GATEWAY_INTERFACE", "CGI/1.1"));
	std::string filename = _target;
	if (_target.rfind('/') != std::string::npos)
		filename = _target.substr(_target.rfind('/') + 1);
	headers.insert(std::pair<std::string, std::string>("SCRIPT_FILENAME", filename));
	headers.insert(std::pair<std::string, std::string>("REQUEST_METHOD", _method));
	if (!_query.empty())
		headers.insert(std::pair<std::string, std::string>("QUERY_STRING", _query));
	if (getenv("PATH")) {
		std::string path = getenv("PATH");
		headers.insert(std::pair<std::string, std::string>("PATH", path));
	}
}

void Request::headers_add(std::string line)
{
	std::stringstream s(line);
	std::string key;
	std::string val;
	
	std::getline(s, key, ':');
	std::getline(s, val, '\r');
	if (key.empty() || val.empty())
		throw BadRequest();
	//checking for a-num values (-)
	if (!check_key(key))
		throw BadRequest();// bad key formatting
	//checking for an empty value + trimming whitespaces
	if (!check_val(val))
		throw BadRequest();// value is empty
	
	//turning 'Content-Length' into 'CONTENT_LENGTH' for future environment and lack of case-conflict
	std::transform(key.begin(), key.end(), key.begin(), ::toupper);
	size_t n = key.find('-');
	while (n != std::string::npos)
	{
		key[n] = '_';
		n = key.find('-');
	}
	if (key != "CONTENT_LENGTH" && key != "CONTENT_TYPE")
		key = "HTTP_" + key;
	headers.insert(std::pair<std::string, std::string>(key, val));
}

void Request::mime_check(std::map<std::string, std::string> &mime)
{
	if (_target.rfind('.') == std::string::npos)
		throw BadRequest(); //i think ? this is all very murky territory, needs testing - maybe BadRequest ?
	std::string extension = _target.substr(_target.rfind('.'));
	if (!mime.count(extension))
		return ; //again, guessing here
	if (!headers.count("HTTP_ACCEPT"))
		return ; //no accept header, not sure what that would mean for me but i assume just no checking
	extension = mime.at(extension);
	std::stringstream line(headers.at("HTTP_ACCEPT"));
	while (!line.eof()) {
		std::string type;
		getline(line, type, ',');
		if (type.empty())
			break ;
		if (type == extension)
			return ;
	}
	throw BadRequest(); //again, guessing at the error
}

void Request::parse_body()
{
	try {
		if (this->getSize() == CHUNKED) {
			//un-chunk the body
			_body = chunk_parse(_body);
			//adjust size headers accordingly (in case of cgi)
			headers.erase(headers.find("TRANSFER_ENCODING"));
			headers.insert(std::pair<std::string, std::string>("CONTENT_LENGTH", ft_itoa(_body.size())));
		}
		else
			this->body_check(this->getSize(), _body.size());
		//create the unsigned char body to send to the cgi program (or download pure ?)
		if (_body.size()) {
			c_body = new unsigned char[_body.size()];
			size_t i = 0;
			while (i < _body.size()) {
				c_body[i] = _body.at(i);
				i++;
			}
		}
	}
	catch (std::exception &e) { throw ; }
}

//is the body the size given in header ?
void Request::body_check(size_t size_told, size_t real_size)
{
	if (real_size < size_told)
		throw MissingData();
	if (real_size > size_told)
		_body = _body.substr(0, size_told);
}

void Request::create_env()
{
	//here creating the char * environment which we can use for execve, in two steps as previously established
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


//error checking only
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
	std::cout << "and the body, in body.txt\n";
	int fd = open("body.txt", O_WRONLY);
	write(fd, c_body, _body.size());
	close(fd);
}

Request::MissingData::MissingData() : std::out_of_range("data missing from request !") {}

Request::DeleteRequest::DeleteRequest(std::string target) : std::out_of_range(target.c_str()) {}
