#include "ClientSocket.hpp"

ClientSocket::ClientSocket(ServerSocket &serverSocket, int _epoll, std::map<const int, Socket *> &_sockets) : Socket(createSocket(serverSocket)), _status(WaitRequest),  _serverSocket(serverSocket), epollInstance(_epoll), sockets(_sockets), _request(Request(serverSocket.getConfig())), _sendpos(0) {}

ClientSocket::~ClientSocket(void) { std::cout << "deleting client of socket :: " << _socketFd << std::endl; }

//this func :: deletes the old_fd from our epoll map (except pipes, which are automatically deleted), adds the new one with flags
//it then erases the <old_fd, csocket> from the sockets map and replaces it with <new_fd, csocket>
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

//new function for error handling :: makes error response, switches status, updates epoll fd
void	ClientSocket::ErrorHandling(HttpError &e, bool exec)
{
	_response.makeErrorResponse(e, _serverSocket.getConfig());
	_status = WaitResponse;
	if (exec)
		epollFdSwitch(_exec.getFdIn(), _socketFd, EPOLLOUT | EPOLLET);
	else
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
			_status = ParseRequest;
		else
			throw std::runtime_error("an error occured while reading into client : '" + \
				ft_itoa(_socketFd) + "' socket : " + std::string(strerror(errno)));
		return ;
	}
	else if (size == 0)
	{
		std::cout << "Client " << _socketFd << " disconnected" << std::endl;
		_status = Done;
		return ;
	}
	_request.add(buffer, size);
}

void	ClientSocket::parseRequest()
{
	try {
		_request.parse();
		//based on response :: exec/no exec
		if (_response.makeResponse(&_request))
			this->startExec();
		else {
			_status = WaitResponse;
			epoll_mod(epollInstance, _socketFd, EPOLLOUT | EPOLLET);
		}
	}
	catch (Request::MissingData &e) {
		_status = ReadRequest;
		//do the timeout specification here
	}
	catch (HttpError &e) { ErrorHandling(e, false); }
	catch (std::exception &e) { throw; }
}

//starting the execution process here (write or exec + read)
void	ClientSocket::startExec()
{
	bool body = _request.getBody();
	try { _exec.setupProcess(body); }
	catch (HttpError &e) { ErrorHandling(e, false); return ; }
	if (!body) {
		_status = WaitExecRead;
		setnonblocking(_exec.getFdOut());
		try { _exec.startProcess(false, _request.getCgi(), _request.getTarget(), _request.getEnv()); }
		catch (HttpError &e) { ErrorHandling(e, false); return ; }
		epollFdSwitch(_socketFd, _exec.getFdOut(), EPOLLIN | EPOLLET);
	}
	else {
		_status = WaitExecWrite;
		setnonblocking(_exec.getFdIn());
		setnonblocking(_exec.getFdOut());
		epollFdSwitch(_socketFd, _exec.getFdIn(), EPOLLOUT | EPOLLET);
	}
}

//new cgi write
void	ClientSocket::execWrite()
{
	unsigned char *body = _request.getBody();
	ssize_t size = _request.getSize() - _sendpos;
	if (size > BUF_SIZE)
		size = BUF_SIZE;
	if (size) {
		if (write(_exec.getFdIn(), (&body[_sendpos]), size) == -1)
			_status = WaitExecWrite;
		else
			_sendpos += size;
	}
	else { //write is done, start up the process appropriate _status/epoll switching and close
		try { _exec.startProcess(true, _request.getCgi(), _request.getTarget(), _request.getEnv()); }
		catch (HttpError &e) { ErrorHandling(e, true); return ; }
		close(_exec.getFdIn());
		_sendpos = 0;
		epollFdSwitch(_exec.getFdIn(), _exec.getFdOut(), EPOLLIN | EPOLLET);
		_status = WaitExecRead;
	}
}

void	ClientSocket::execRead()
{
	unsigned char buffer[BUF_SIZE];
	ssize_t size = read(_exec.getFdOut(), buffer, BUF_SIZE);
	if (size < 0)
		_status = WaitExecRead;
	else if (!size) { //read is done, appropriate _status/epoll switching and close
		close(_exec.getFdOut());
		epollFdSwitch(_exec.getFdOut(), _socketFd, EPOLLOUT | EPOLLET);
		_status = WaitResponse;
	}
	else
		_response.add(buffer, size);
}

void ClientSocket::sendResponse()
{
	unsigned char *msg = _response.getResponse();
	size_t size = _response.getSize() - _sendpos;
	if (size > BUF_SIZE)
		size = BUF_SIZE;
	if (size) {
		if (write(_socketFd, (&msg[_sendpos]), size) == -1)
			_status = WaitResponse;
		else
			_sendpos += size;
	}
	else { //write is done, reset for next request or close the connection
		_sendpos = 0;
		if (_request.keepAlive())
			this->reset();
		else
			_status = Done;
	}
}

void ClientSocket::reset()
{
	std::cout << "resetting connection socket " << _socketFd << std::endl << std::endl;
	_request.clear();
	_response.clear();
	_exec.clear();
	epoll_mod(epollInstance, _socketFd, EPOLLIN | EPOLLET);
	_status = WaitRequest;
	_sendpos = 0;
}

int ClientSocket::createSocket(ServerSocket & serverSocket)
{
	int sSocketFd;
	int cSocketFd;
	
	sSocketFd = serverSocket.getSocketFd();

	// recover the value of client fd, variables that are set to NULL represent the client informations could be useful later...
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
