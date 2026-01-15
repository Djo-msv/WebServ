#include "ClientSocket.hpp"

ClientSocket::ClientSocket(int epollInstance, ServerSocket &serverSocket) : _status(READ)
{
	_serverSocket = serverSocket.getSocketFd();
	// recover the value of client fd, variables that are set to NULL represent the client informations could be useful later...
	_clientSocket = accept(_serverSocket, NULL, NULL);
	if (_clientSocket == -1) {
		throw std::runtime_error("an error occured when accepting connection to the fd : " + \
			ft_itoa(server_socket) + \
			". Error code : " + ft_itoa(errno));
	}
	setnonblocking(_clientSocketFd);
}

ClientSocket::~ClientSocket(void)
{
	if (_clientSocket > 0)
		close(_clientSocket);
}

void	ClientSocket::read(void)
{
	char	buffer[BUF_SIZE];

	// read into Client Socket and put result into buffer
	// read return size of char put into buffer, -1 is for error
	ssize_t	size = ::read(_clientSocket, static_cast<void*>(buffer), BUF_SIZE);

	if (size == -1){
		if (errno == EWOULDBLOCK) {
			_status = WRITE;
			return ;
		}
		close(_clientSocket);
		throw std::runtime_error("an error occured when read the fd : " + \
				ft_itoa(_clientSocket) + \
				". Error code : " + ft_itoa(errno));
	}
	_clientRequest += buffer;
}

int	ClientSocket::getSocketFd(void)
{
	return (_clientSocketFd);
}

bool	ClientSocet::getStatus(void)
{
	return (_status);
}
