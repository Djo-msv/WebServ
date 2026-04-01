#include "Response.hpp"

Response::Response() : _status("200 OK"), _msg(NULL), exec(false), sizer(0) {}

Response::~Response() { if (_msg) { delete[] _msg; } }

Response::Response(const Response &other) : _target(other._target), _status(other._status), \
					_headers(other._headers), _body(other._body), exec(other.exec), \
					sizer(other.sizer) {}

Response& Response::operator=(const Response &other)
{
	if (this != &other)
	{
		_target = other._target;
		_headers = other._headers;
		_status = other._status;
		_body = other._body;
		exec = other.exec;
		if (_msg)
			delete[] _msg;
		_msg = NULL;
		sizer = 0;
	}
	return *this;
}
//self-exp clear function
void Response::clear()
{
	_target.clear();
	_headers.clear();
	_status = "200 OK";
	_body.clear();
	if (_msg)
		delete[] _msg;
	_msg = NULL;
	exec = false;
	sizer = 0;
}

//replaces += overload for non NULL-terminated buffers
void Response::add(const unsigned char *buffer, size_t size)
{
	for (size_t i = 0; i != size; i++)
		_body.push_back(buffer[i]);
}

		//public getters

size_t Response::getSize() const { return sizer; }

unsigned char *Response::getResponse(std::map<std::string, std::string> &mime)
{
	//classic recipe for a static GET response (no exec) + exec handling below
	if (_msg == NULL)
	{
		if (!exec) {
			_headers += "HTTP/1.1 " + _status;
			if (_target.rfind('.') != std::string::npos && mime.count(_target.substr(_target.rfind('.'))))
				_headers += "\r\nContent-Type: " + mime.at(_target.substr(_target.rfind('.')));
			_headers += "\r\nTransfer-Encoding: chunked\r\n";
			this->chunkBody();
		}
		else { this->handleExec(); }
		this->makeMsg();
	}
	return _msg;
}

		//response-maker && error-response-maker, respectively

bool Response::makeResponse(Request *req)
{
	exec = req->isExec();
	_target = req->getTarget();
	if (!exec)
		this->readFile();
	return exec;
}

void Response::makeErrorResponse(HttpError &error, ServerConfig &s)
{
	_status = error.what();
	try { _target = seekErrorFile(error, s); this->readFile(); }
	catch (InternalServerError &e) {_status = e.what(); _target = ""; this->add((unsigned char *)(e.getDefaultFile().c_str()), e.getDefaultFile().size());}
}

		//allocating the (unsigned char*) message to return 

void Response::makeMsg()
{
	sizer = _body.size();
	if (!exec && !_headers.empty())
		sizer += _headers.size();
	try {_msg = new unsigned char[sizer]; }
	catch (std::bad_alloc &e) {
		_msg = NULL;
		throw e;
	}
	size_t pos = 0;
	if (!exec && !_headers.empty()) {
		for (std::string::iterator it = _headers.begin(); it != _headers.end(); it++) {
			_msg[pos] = *it;
			pos++;
		}
	}
	for (ustring::iterator it = _body.begin(); it != _body.end(); it++) {
		_msg[pos] = *it;
		pos++;
	}
}

		//private message-making functions, in chronological order

void Response::handleExec() //
{
	if (_body.empty() || _body.find((unsigned char *)"\n") == ustring::npos) {
		_body = (unsigned char *)"HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\nContent-Length: 90\r\n\r\n<html><h1>The CGI program did not return anything or took too long to respond.</h1></html>";
		return ;
	}
	ustring fin = (unsigned char *)"\r\n";
	if (_body.find(fin) == ustring::npos) { fin = (unsigned char *)"\n"; }
	std::string stat = (char *)_body.substr(0, _body.find(fin)).c_str();
	if (stat.size() > 9 && stat.substr(0, 8) == "HTTP/1.1") { return; }
	if (stat.size() > 8) {
		if (stat.substr(0, 7) != "Status:" && stat.substr(0, 7) != "status:") { stat = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n"; }
		else {
			stat = stat.substr(7);
			if (stat[0] != ' ') { stat = " " + stat; }
			stat = "HTTP/1.1" + stat;
			_body = _body.substr(_body.find(fin));
		}
		_body = (unsigned char*)stat.c_str() + _body;
	}
}

void Response::readFile()
{
	if (_target.empty()) //no _body to make || _body is from default
		return ;
	struct stat s;
	if ( stat(_target.c_str(), &s) == 0 )
	{
		if( s.st_mode & S_IFDIR )
		{
			DIR *dir;
			struct dirent *ent;
			if ((dir = opendir (_target.c_str())) != NULL) {
				//print all the files and directories within directory
				while ((ent = readdir (dir)) != NULL) {
					std::string name = ent->d_name;
					if (name == "." || name == "..")
						continue;
					_body += (unsigned char *)ent->d_name;
					_body.push_back('\n');
				}
				closedir (dir);
			}
		}
		else if ( s.st_mode & S_IFREG )
		{
			int fd = open(_target.c_str(), O_RDONLY);
			if (fd == -1)
				return ;
			unsigned char a;
			//read file
			try
			{
				while (read(fd, &a, 1))
					_body.push_back(a);
			} RETHROW(std::bad_alloc)
			close(fd);
		}
	}
}

//for chunkBody()
ustring toHex(size_t num)
{
	ustring res;
	ustring nest;
	nest.push_back('a');
	static unsigned char hex[] = "0123456789ABCDEF";
	if (num == 0)
		return (unsigned char *)"\r\n0\r\n";
	while (num) {
		res = nest + res;
		res[0] = hex[num % 16];
		num /= 16;
	}
	nest = (unsigned char *)"\r\n";
	res = nest + res + nest;
	return res;
}

void Response::chunkBody()
{
	//here, chunking by BUF_SIZE
	std::vector<ustring> ensemble;
	size_t pos = 0;
	while (pos != _body.size())
	{
		size_t size = _body.length() - pos;
		if (BUF_SIZE < size)
			size = BUF_SIZE;
		ensemble.push_back(toHex(size));
		ensemble.push_back(_body.substr(pos, size));
		pos += size;
	}
	_body.clear();
	ustring end = (unsigned char *)"\r\n0\r\n\r\n";
	for (std::vector<ustring>::iterator it = ensemble.begin(); it != ensemble.end(); it++)
		_body += *it;
	_body += end;
}
