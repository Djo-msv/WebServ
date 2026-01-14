#include "ServerSocket.hpp"
#include <errno.h>

ServerSocket::ServerSocket(ServerConfig config, int epollInstance)
{
	// sin_family is always AF_INET.
	// SOCK_STREAM is to tell the socket to use TCP protocol.
	if ((_socketFd = socket(config.sin_family, SOCK_STREAM, 0)) == -1) {
		close(_socketFd);
		throw SocketError("Error during socket creation. Error code : " + ft_itoa(errno));
	}

	setnonblocking(_socketFd);
	sockaddr_in serverAddress = setupSocketAddress(config);
	
	if (bind(_socketFd, (struct sockaddr*) &serverAddress, sizeof(serverAddress)) == -1) {
		close(_socketFd);
		throw BindError("Error while binding adress to the server socket. Error code : " + ft_itoa(errno));
	}

	// listen at the server socket and allow 5 connexions at a time
	if (listen(_socketFd, 5) == -1) {
		close(_socketFd);
		throw ListenError("Error while setting socket to listening. Error code :" + ft_itoa(errno));
	}

	epoll_add(epollInstance, _socketFd, EPOLLIN | EPOLLET);

	std::cout << "Server started on port " << config.sin_port << std::endl;
}

ServerSocket::~ServerSocket(void)
{
	close(_socketFd);
}

int	ServerSocket::getSocketFd(void)
{
	return (_socketFd);
}

sockaddr_in ServerSocket::setupSocketAddress(ServerConfig config)
{
	sockaddr_in serverAddress;

	serverAddress.sin_family = config.sin_family;
	serverAddress.sin_port = htons(config.sin_port); /* sin_port is port number. */
	serverAddress.sin_addr.s_addr = INADDR_ANY; /* INADDR_ANY : accept connection from any IP address */

	return (serverAddress);
}
