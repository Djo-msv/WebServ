#include "ClientSocket.hpp"

ClientSocket::ClientSocket(ServerSocket &serverSocket, int epoll, std::map<const int, Socket *> &sockets, std::map<std::string, std::string> &mime) :
	Socket(createSocket(serverSocket)), status(WaitRequest), sockets(sockets), mime(mime),
	_serverSocket(serverSocket), epollInstance(epoll), _request(Request(serverSocket.getConfig())), _sendpos(0) {
	resetTimeout();
}

ClientSocket::~ClientSocket(void) { std::cout << "deleting client of socket :: " << _socketFd << std::endl; }

/*
 * deletes the old_fd from our epoll map (except pipes, which are automatically deleted), adds the new one with flags
 * then erases the <old_fd, csocket> from the sockets map and replaces it with <new_fd, csocket>
*/
void	ClientSocket::epollFdSwitch(int old_fd, int new_fd, int flags)
{
	SocketIterator it = sockets.find(old_fd);
	if (it == sockets.end())
		throw std::out_of_range("incorrect old fd, not in sockets map");
	sockets.erase(it);
	sockets.insert(std::pair<int, Socket *>(new_fd, this));
	if (old_fd == _socketFd)
		epoll_del(epollInstance, old_fd, EPOLLOUT);
	epoll_add(epollInstance, new_fd, flags);
}

//makes error response, switches status, updates epoll fd
void	ClientSocket::errorHandling(HttpError &e)
{
	_response.makeErrorResponse(e, _serverSocket.getConfig());
	status = WaitResponse;
	epoll_mod(epollInstance, _socketFd, EPOLLOUT | EPOLLET);
}

/**
 * * reads into Client Socket and put result into buffer.
 * ! If read return -1 we check if the socket is still valid with getsockname,
 * ! if not an error is thrown.
 * ! If read return 0 the client disconnected and an error is thrown.
*/
void	ClientSocket::readRequest(void)
{
	unsigned char	buffer[BUF_SIZE + 1];

	std::memset(buffer, 0, BUF_SIZE + 1);
	ssize_t	size = ::read(_socketFd, buffer, BUF_SIZE);
	if (size == -1)
	{
		/**
		 * *	getsockname returns 0 on successful connection to the socket.
		 * *	we check that the socket is still valid ; if it is we 
		 * * 	read all available data
		*/
		struct sockaddr addr;
		socklen_t size = sizeof(addr);
		if (!getsockname(_socketFd, &addr, &size)) //reading done (so far)
			status = ParseRequest;
		else
			throw std::runtime_error("an error occured while reading into client : '" + \
				ft_itoa(_socketFd) + "' socket : " + std::string(strerror(errno)));
		return ;
	}
	else if (size == 0)
	{
		std::cout << "Client " << _socketFd << " disconnected" << std::endl;
		status = Done;
		return ;
	}
	resetTimeout();
	try { _request.add(buffer, size); }
	RETHROW(std::bad_alloc)
}

void	ClientSocket::parseRequest()
{
	try {
		_request.parse(mime);
		// returns if exec should be started
		if (_response.makeResponse(&_request))
			this->startExec();
		else {
			status = WaitResponse;
			epoll_mod(epollInstance, _socketFd, EPOLLOUT | EPOLLET);
		}
	}
	catch (Request::ChunkParsing &e) { status = ParseRequest; } // currently dechunking the request body
	catch (Request::MissingData &e) {
		//incomplete request
		if (this->hasTimedOut()) { status = Done; }
		else {status = ReadRequest; }
	}
	catch (Redirect &e) { this->redirect(e.what(), e.getDefaultFile()); return ;}
	catch (Request::DeleteRequest &e) {
		try { this->deleteFile(e.what()); }
		RETHROW(std::bad_alloc)
	}
	catch (HttpError &e) { errorHandling(e); }
	catch (std::exception &e) { throw; }
}

void	ClientSocket::redirect(std::string status, std::string target)
{
	std::string msg = "HTTP/1.1 " + status + "\r\nLocation: " + target + "\r\n\r\n";
	try {
		_response.add((unsigned char *)msg.c_str(), msg.size());
		_response.makeMsg();
	} RETHROW(std::bad_alloc)
	this->status = WaitResponse; 
	epoll_mod(epollInstance, _socketFd, EPOLLOUT | EPOLLET);
}

void	ClientSocket::deleteFile(const char *filename)
{
	InternalServerError e;
	if (std::remove(filename))
		errorHandling(e);
	else {
		//here we handle the pivot into response
		ustring response = (unsigned char *)"HTTP/1.1 204 No Content\r\n\r\n";
		try {
			_response.add(response.c_str(), response.size());
			_response.makeMsg();
		} RETHROW(std::bad_alloc)
		this->status = WaitResponse;
		epoll_mod(epollInstance, _socketFd, EPOLLOUT | EPOLLET);
	}
}

//starting the execution process here (exec + write or read)
void	ClientSocket::startExec()
{
	bool body = _request.getBody(); //do we need to write to script ?
	try { _exec.setupProcess(body); } //pipe setup
	catch (HttpError &e) { errorHandling(e); return ; }
	setnonblocking(_exec.getFdOut());
	if (body)
		setnonblocking(_exec.getFdIn());
	try { _exec.startProcess(body, _request.getCgi(), _request.getTarget(), _request.getEnv()); }
	catch (HttpError &e) { errorHandling(e); return ; }
	RETHROW(std::bad_alloc)
	if (body) {
		status = WaitExecWrite;
		epollFdSwitch(_socketFd, _exec.getFdIn(), EPOLLOUT | EPOLLET);
	}
	else {
		status = WaitExecRead;
		epollFdSwitch(_socketFd, _exec.getFdOut(), EPOLLIN | EPOLLET);
	}
}

void	ClientSocket::execWrite()
{
	resetTimeout();
	unsigned char *body = _request.getBody();
	ssize_t size = _request.getSize() - _sendpos;
	if (size > BUF_SIZE)
		size = BUF_SIZE;
	if (size) {
		if ((size = write(_exec.getFdIn(), (&body[_sendpos]), size)) == -1)
			status = WaitExecWrite;
		else
			_sendpos += size;
	}
	else { //write is done, appropriate status/epoll switching and close
		close(_exec.getFdIn());
		_sendpos = 0;
		epollFdSwitch(_exec.getFdIn(), _exec.getFdOut(), EPOLLIN | EPOLLET);
		status = WaitExecRead;
	}
}

//write is done (timeout), appropriate status/epoll switching and close
void	ClientSocket::writeToRead()
{
	close(_exec.getFdIn());
	_sendpos = 0;
	epollFdSwitch(_exec.getFdIn(), _exec.getFdOut(), EPOLLIN | EPOLLET);
	status = WaitExecRead;
	resetTimeout();
}

void	ClientSocket::execRead()
{
	unsigned char buffer[BUF_SIZE];

	ssize_t size = read(_exec.getFdOut(), buffer, BUF_SIZE);
	resetTimeout();
	if (size < 0)
		status = WaitExecRead;
	else if (!size) { //read is done, appropriate status/epoll switching and close
		close(_exec.getFdOut());
		epollFdSwitch(_exec.getFdOut(), _socketFd, EPOLLOUT | EPOLLET);
		status = WaitResponse;
	}
	else
	{
		try { _response.add(buffer, size); }
		RETHROW(std::bad_alloc)
	}
}

void	ClientSocket::readToWrite()
{
	//read is done (timeout), appropriate status/epoll switching and close
	close(_exec.getFdOut());
	epollFdSwitch(_exec.getFdOut(), _socketFd, EPOLLOUT | EPOLLET);
	status = WaitResponse;
	resetTimeout();
}

void ClientSocket::sendResponse()
{
	unsigned char *msg;

	resetTimeout();
	try { msg = _response.getResponse(mime); }
	RETHROW (std::bad_alloc)
	ssize_t size = _response.getSize() - _sendpos;
	if (size > BUF_SIZE)
		size = BUF_SIZE;
	if (size) {
		if ((size = write(_socketFd, (&msg[_sendpos]), size)) == -1)
			status = WaitResponse;
		else
			_sendpos += size;
	}
	else { //write is done, reset for next request or close the connection
		_sendpos = 0;
		if (_request.keepAlive())
			this->reset();
		else
			status = Done;
	}
}

void ClientSocket::reset()
{
	std::cout << "resetting connection socket " << _socketFd << std::endl << std::endl;
	_request.clear();
	_response.clear();
	_exec.clear();
	epoll_mod(epollInstance, _socketFd, EPOLLIN | EPOLLET);
	status = WaitRequest;
	resetTimeout();
	_sendpos = 0;
}


void ClientSocket::resetTimeout()
{
	timeout = std::time(NULL) + _serverSocket.getConfig().getTimeout();
}

bool ClientSocket::hasTimedOut() const { return (timeout <= std::time(NULL)); }

int ClientSocket::createSocket(ServerSocket & serverSocket)
{
	int sSocketFd;
	int cSocketFd;
	
	sSocketFd = serverSocket.getSocketFd();

	// recover the value of client fd, variables that are set to NULL represent the client informations
	cSocketFd = accept(sSocketFd, NULL, NULL);
	if (cSocketFd == -1) {
		close(cSocketFd);
		throw std::runtime_error("an error occured when accepting connection to the fd : " + \
			ft_itoa(sSocketFd) + ". Error code : " + ft_itoa(errno));
	}
	setnonblocking(cSocketFd);
	std::cout << "create socket : " << cSocketFd << std::endl;
	return (cSocketFd);
}
