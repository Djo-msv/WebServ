#include "ClientSocket.hpp"
#include "ProcessExecution.hpp"

ClientSocket::ClientSocket(ServerSocket &serverSocket) : Socket(createSocket(serverSocket)), _serverSocket(serverSocket), _status(ReadRequest) {}

ClientSocket::~ClientSocket(void) {}

unsigned int	ClientSocket::getStatus(void)
{
	return (_status);
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
		if (!getsockname(_socketFd, NULL, NULL))
			_status = ParseRequest;
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

void	ClientSocket::parseRequest(void)
{
	try {
		_request.parse();
		_status = ExecProcess;
	} catch (HttpError &e)
	{
		_processResponse = e.what();
		_status = SendResponse;
	}
	catch (std::exception &e) {throw ;}
}


void	ClientSocket::execProcess(void)
{
	// TODO Logique de L'exécution
	(void) _serverSocket;
	// _process.startProcess(&_serverSocket.getConfig().cgi_path)
	_status = ReadProcess;
}

void	ClientSocket::readProcess(void)
{
	char	buffer[BUF_SIZE];

	std::memset(buffer, 0, BUF_SIZE);
	// read into CGI process and put result into buffer
	// read return size of char put into buffer, -1 is for error
	ssize_t	size = ::read(_processFd, buffer, BUF_SIZE);
	std::cout << "buffer : " << buffer << " size read : " << size << std::endl;
	if (size == -1)
		throw std::runtime_error("an error occured while reading the process fd : " + \
			ft_itoa(_processFd) + " of client '" + ft_itoa(_socketFd) + "' : " + strerror(errno));
	if (size == 0)
		_status = SendResponse;
	else
		_processResponse += buffer;
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