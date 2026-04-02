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
	if (_status) {
		for (size_t i = 0; i != size; i++)
			_body.push_back(buffer[i]);
		return ;
	}
	for (size_t i = 0; i != size; i++)
		_request.push_back(buffer[i]);
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
	//assume keep-alive unless connection: close is a present header
	if (headers.count("CONNECTION") && headers.at("CONNECTION") == "close")
		return false;
	return true;
}

std::string Request::getTarget() const { return _target; }

std::string Request::getMethod() const { return _method; }

std::string Request::getQuery() const { return _query; }

char **Request::getEnv() const { return (char **)c_env; }

bool Request::isExec() const { return exec; }

bool Request::isPost() const { return (_method == "POST"); }

unsigned char *Request::getBody() const { return c_body; }

std::string Request::getCgi() const { return _cgi; }

ssize_t Request::getSize() const
{
	if (headers.count("CONTENT_LENGTH"))
		return (ssize_t)atol((headers.at("CONTENT_LENGTH")).c_str());
	if (headers.count("HTTP_TRANSFER_ENCODING") && headers.at("HTTP_TRANSFER_ENCODING") == "chunked")
		return CHUNKED;
	return 0;
}

//parse distribution
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
		else if (_method == "GET")
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
		else if (_method == "GET")
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
	if (_target.empty() || l.eof())
		throw BadRequest();
	getline(l, current, '\r');
	if (current != "HTTP/1.1" && current != "HTTP/1.*")
		throw NotImplemented();
	getline(l, current);
	if (!current.empty())
		throw BadRequest();
	this->target_work();
}

void Request::target_work()
{
	if (_target[0] != '/')
		_target = "/" + _target;
	//make location list
	std::list<std::string> location = target_list(_target);
	//cut path_info and update target + list + script_name
	std::string path_info = get_path_info(_config.getFullPath(location));
	if (!path_info.empty() && _target.find(path_info) != std::string::npos) {
		_target = _target.substr(0, _target.rfind(path_info));
		location = target_list(_target);
		if (!_config.isExecFolder(location, _config.stringToMethodFlag(_method))) {
			_target += path_info;
			location = target_list(_target);
		}
	}
	std::string script_name = _target;
	// method check
	if (!_config.isMethodAllowed(location, _config.stringToMethodFlag(_method)))
		throw NotAllowed();
	//add index
	std::string index;
	if (_target == "/" || needsIndex(_config.getFullPath(location))) {
		index = _config.getIndex(location);
		if (!index.empty() && index[0] != '/')
			index = "/" + index;
		if (!index.empty()) { location.push_front(index); script_name += index; }
	}
	//target to full_path
	_target = _config.getFullPath(location);
	//NotFound / Forbidden catch
	try { seekFile(_target, (_method == "POST")); }
	catch (std::exception &e) { throw ; }
	//check_exec, adjust
	if (_config.isExecFolder(location, _config.stringToMethodFlag(_method)))
		this->adjust_exec(path_info, script_name);
	else if (_method == "POST" && !_query.empty()) //in case of malformed POST request
		_body = (unsigned char *)_query.c_str() + _body;
}

bool Request::needsIndex(std::string full_target)
{
	if (*(_target.rbegin()) == '/') { return false; }
	struct stat s;
	if ( stat(full_target.c_str(), &s) != 0 ) { throw FileNotFound(); }
	if( s.st_mode & S_IFDIR ) {
		_target += "/";
		return true;
	}
	return false;
}

void Request::adjust_exec(std::string path_info, std::string script_name)
{
	exec = true;
	//looking for cgi executable file
	try { _cgi = extractCgi(_target, _config); }
	catch (std::exception &e) { throw ; }
	//adding relevant variables :: cgi version, redirect status, query string, method request, etc.
	headers.insert(std::pair<std::string, std::string>("REDIRECT_STATUS", "true"));
	headers.insert(std::pair<std::string, std::string>("GATEWAY_INTERFACE", "CGI/1.1"));
	headers.insert(std::pair<std::string, std::string>("SERVER_PROTOCOL", "HTTP/1.1"));
	headers.insert(std::pair<std::string, std::string>("SERVER_PORT", ft_itoa(_config.sin_port)));
	headers.insert(std::pair<std::string, std::string>("SERVER_NAME", "localhost"));
	headers.insert(std::pair<std::string, std::string>("SERVER_SOFTWARE", "HOMEMADE/1.0"));
	if (path_info.empty()) { path_info = script_name; }
	headers.insert(std::pair<std::string, std::string>("PATH_INFO", path_info));
	headers.insert(std::pair<std::string, std::string>("SCRIPT_FILENAME", script_name));
	headers.insert(std::pair<std::string, std::string>("REQUEST_METHOD", _method));
	if (!_query.empty())
		headers.insert(std::pair<std::string, std::string>("QUERY_STRING", _query));
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
		throw BadRequest();
	//checking for an empty value + trimming whitespaces
	if (!check_val(val))
		throw BadRequest();
	
	//turning 'Content-Length' into 'CONTENT_LENGTH' for future cgi environment and lack of case-conflict
	std::transform(key.begin(), key.end(), key.begin(), ::toupper);
	size_t n = key.find('-');
	while (n != std::string::npos)
	{
		key[n] = '_';
		n = key.find('-');
	}
	//adding the HTTP prefix for HTTP-specific cgi environment variables
	if (key != "CONTENT_LENGTH" && key != "CONTENT_TYPE")
		key = "HTTP_" + key;
	headers.insert(std::pair<std::string, std::string>(key, val));
}

void Request::mime_check(std::map<std::string, std::string> &mime)
{
	if (_target.rfind('.') == std::string::npos)
		return ;
	std::string extension = _target.substr(_target.rfind('.'));
	if (!mime.count(extension))
		return ;
	if (!headers.count("HTTP_ACCEPT"))
		return ;
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
	throw BadRequest(); //client requests a content-type it does not accept
}

void Request::parse_body()
{
	try {
		if (this->getSize() == CHUNKED) {
			//un-chunk the body
			_body = chunk_parse(_body, _parse_body);
			//adjust size headers accordingly]
			headers.erase(headers.find("HTTP_TRANSFER_ENCODING"));
			headers.insert(std::pair<std::string, std::string>("CONTENT_LENGTH", ft_itoa(_body.size())));
		}
		else
			this->body_check(this->getSize(), _body.size());
		//create the unsigned char body to send to the cgi program
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
	if (!size_told && real_size && !headers.count("CONTENT_LENGTH"))
		throw LengthRequired();
	if (real_size > size_told)
		_body = _body.substr(0, size_told);
}

void Request::create_env()
{
	//here creating the char * environment which we use for execve, in two steps for ease of reading
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

//class exceptions :: more data needed, more parsing needed && request is delete
Request::MissingData::MissingData() : std::out_of_range("data missing from request !") {}
Request::ChunkParsing::ChunkParsing() : std::out_of_range("parsing of the chunked body is unfinished !") {}
Request::DeleteRequest::DeleteRequest(std::string target) : std::out_of_range(target.c_str()) {}
