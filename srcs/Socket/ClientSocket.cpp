#include "ClientSocket.hpp"

ClientSocket::ClientSocket(ServerSocket &serverSocket) : _status(READ)
{
	int serverSocketFd = serverSocket.getSocketFd();
	_serverSocket = &serverSocket;
	// recover the value of client fd, variables that are set to NULL represent the client informations could be useful later...
	_clientSocketFd = accept(serverSocketFd, NULL, NULL);
	if (_clientSocketFd == -1) {
		throw std::runtime_error("an error occured when accepting connection to the fd : " + \
			ft_itoa(serverSocketFd) + \
			". Error code : " + ft_itoa(errno));
	}
	setnonblocking(_clientSocketFd);
}

ClientSocket::~ClientSocket(void)
{
	if (_clientSocketFd > 0)
		close(_clientSocketFd);
}

void	ClientSocket::read(void)
{
	char	buffer[BUF_SIZE];

	// read into Client Socket and put result into buffer
	// read return size of char put into buffer, -1 is for error
	ssize_t	size = ::read(_clientSocketFd, static_cast<void*>(buffer), BUF_SIZE);

	if (size == -1){
		if (errno == EWOULDBLOCK) {
			_status = WRITE;
			return ;
		}
		close(_clientSocketFd);
		throw std::runtime_error("an error occured when read the fd : " + \
				ft_itoa(_clientSocketFd) + \
				". Error code : " + ft_itoa(errno));
	}
	_clientRequest += buffer;
}

int	ClientSocket::getSocketFd(void)
{
	return (_clientSocketFd);
}

bool	ClientSocket::getStatus(void)
{
	return (_status);
}
