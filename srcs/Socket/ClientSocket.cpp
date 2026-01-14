#include "ClientSocket.hpp"

ClientSocket::ClientSocket(int epollInstance, int server_socket)
{
	// recover the value of client fd, variables that are set to NULL represent the client informations could be useful later...
	_clientSocket = accept(server_socket, NULL, NULL);
	if (_clientSocket == -1) {
		throw std::runtime_error("an error occured when accepting connection to the fd : " + \
			ft_itoa(server_socket) + \
			". Error code : " + ft_itoa(errno));
	}
	setnonblocking(_clientSocket);
	// add client to the list
	epoll_add(epollInstance, _clientSocket, EPOLLIN | EPOLLET);
}

ClientSocket::~ClientSocket(void)
{
	if (_clientSocket > 0)
		close(_clientSocket);
}

void	ClientSocket::read(void)
{
	char	buffer[BUF_SIZE];

	ssize_t	size = ::read(_clientSocket, static_cast<void*>(buffer), BUF_SIZE);

	if (size == -1){
		close(_clientSocket);
		throw std::runtime_error("an error occured when read the fd : " + \
				ft_itoa(_clientSocket) + \
				". Error code : " + ft_itoa(errno));
	}
	_clientRequest += buffer;
}
