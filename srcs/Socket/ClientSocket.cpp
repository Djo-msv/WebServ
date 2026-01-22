#include "ClientSocket.hpp"

ClientSocket::ClientSocket(ServerSocket &serverSocket) : Socket::Socket(createSocket(serverSocket)), _status(READ){}

ClientSocket::~ClientSocket(void) {}

unsigned int	ClientSocket::getStatus(void)
{
	return (_status);
}

void	ClientSocket::readRequest(void)
{
	char	buffer[BUF_SIZE];

	std::memset(buffer, 0, BUF_SIZE);
	// read into Client Socket and put result into buffer
	// read return size of char put into buffer, -1 is for error
	ssize_t	size = ::read(_socketFd, buffer, BUF_SIZE);

	std::cout << "Size read :" << size << std::endl;
	std::cout << buffer << std::endl;
	if (size == -1)
		throw std::runtime_error("an error occured while reading into client : '" + \
			ft_itoa(_socketFd) + "' socket : " + std::string(strerror(errno)));
	if (size == 0)
		_status = EXEC;
	else
		_clientRequest += buffer;
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
		_status = WRITE;
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
		throw std::runtime_error("an error occured when accepting connection to the fd : " + \
			ft_itoa(sSocketFd) + ". Error code : " + ft_itoa(errno));
	}
	setnonblocking(cSocketFd);
	return (cSocketFd);
}
