#include "ClientSocket.hpp"

ClientSocket::ClientSocket(ServerSocket &serverSocket) : Socket(createSocket(serverSocket)), _serverSocket(serverSocket), _status(Start) {}

ClientSocket::~ClientSocket(void) {}

ClientSocket::state	ClientSocket::getStatus(void)
{
	return _status;
}

/**
 * * reads into Client Socket and put result into buffer.
 * ! If read return -1 we check if the socket is still valid with getsockname,
 * ! if not an error is thrown.
 * ! If read return 0 the client disconnected and an error is thrown.
*/
void	ClientSocket::readRequest(void)
{
	char	buffer[BUF_SIZE];

	std::memset(buffer, 0, BUF_SIZE);
	ssize_t	size = ::read(_socketFd, buffer, BUF_SIZE);
	if (size == -1)
	{
		/**
		 * *	getsockname returns 0 on successful connection to the socket.
		 * *	we check that the socket is still valid if it this it means we 
		 * * 	read all available data
		*/
		struct sockaddr addr;
		socklen_t size = sizeof(addr);
		if (!getsockname(_socketFd, &addr, &size)) {
			try {
				//maybe stash this in a separate function later, but still called here
				//in the future : a separate catch for "missing data" error to start the timeout counter and wait for additional data
				_request.parse();
				//start response_making
				_response.makeResponse(&_request);
				//updating the processfd and status based on response-making
				_processFd = _response.getFd();
				switch (_response.getStatus()) {
					case 2:
						_status = SendResponse;
						_processResponse = _response.getResponse();
						break ;
					case 0:
						_status = WaitProcess;
						setnonblocking(_processFd);
						//make _processfd nonblocking ? do we do it here or back at exec
						//epoll add here :: POLLIN, POLLET, also add to the int, Socket* map
						break ;
					case 1:
						_status = WriteProcess;
						setnonblocking(_processFd);
						//make _processfd nonblocking ? do we do it here or back at exec
						//epoll add here :: POLLOUT, POLLET, also add to the int, Socket* map
						break ;
				}
			}
			catch (HttpError &e) //where am i catching this from ??
			{
				_processResponse = e.what();
				_status = SendResponse;
			}
			catch (std::exception &e) { throw; }
		}
		else
			throw std::runtime_error("an error occured while reading into client : '" + \
				ft_itoa(_socketFd) + "' socket : " + std::string(strerror(errno)));
		return ;
	}
	else if (size == 0)
	{
		//TODO disconnect from client, maybe create specific exception
		std::cout << "Client disconnected" << std::endl;
		return ;
	}
	_request += buffer;
}

//pipeProcess doesboth reading and writing
void	ClientSocket::pipeProcess()
{
	_response.actionExec();
	if (_response.getStatus() == 2) {
		_status = SendResponse;
		_processResponse = _response.getResponse();
		return ;
	}
	if (_status == WriteProcess && _response.getStatus() == 0) {
		_status = WaitProcess;
		_processFd = _response.getFd();
		setnonblocking(_processFd);
		//do the add to e-poll and int, Socket* map here instead so we can read the response next
	}
}

void ClientSocket::sendResponse() const
{
	if (write(_socketFd, _processResponse.c_str(), _processResponse.length()) == -1)
		throw std::out_of_range("writing to client went wrong"); //probably closing the connection at this point
	//ideally here (post-Message Abstraction + pointing), we'd do a simple delete _response and set our status back at _readrequest
}

int ClientSocket::getProcessFd() const { return _processFd; }

void ClientSocket::step() {
	switch (_status) {
		case Start:
			_status = ReadRequest;
			break ;
		case WaitProcess:
			_status = ReadProcess;
			break ;
		default:
			break ;
	}
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


ClientSocket::GatewayTimeout::GatewayTimeout() : HttpError("HTTP/1.1 504 Gateway Time-out\r\n"
														   "Content-length: 92\r\n"
												    	   "Cache-Control: no-cache\r\n"
													       "Content-Type: text/html\r\n"
														   "\r\n"
														   "<html><body><h1>504 Gateway Time-out</h1>\n"
														   "The server didn't respond in time.\n"
														   "</body></html>", 504) {}
