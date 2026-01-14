#include "ServerSocket.hpp"

ServerSocket::ServerSocket(int sin_port, sa_family_t sin_family)
{
	// sin_family is always AF_INET.
	// SOCK_STREAM is to tell the socket to use TCP protocol.
	if ((_socketFd = socket(sin_family, SOCK_STREAM, 0)) == -1) {
		close(_socketFd);
		throw SocketError("Error during socket creation");
	}

	_serverAddress.sin_family = sin_family;
	_serverAddress.sin_port = htons(sin_port); /* sin_port is port number. */
	_serverAddress.sin_addr.s_addr = INADDR_ANY; /* INADDR_ANY : accept connection from any IP address */
	_addressLen = sizeof(_serverAddress);

	if (bind(_socketFd, (struct sockaddr*)&_serverAddress, sizeof(_serverAddress)) == -1) {
		close(_socketFd);
		throw BindError("Error while binding adress to the server socket");
	}

	// listen at the server socket and allow 5 connexions at a time
	if (listen(_socketFd, 5) == -1) {
		close(_socketFd);
		throw ListenError("Error while listening");
	}

	std::cout << "Server started on port " << sin_port << std::endl;
}

ServerSocket::~ServerSocket(void)
{
	close(_socketFd);
}

int	ServerSocket::getSocketFd(void)
{
	return (_socketFd);
}

sockaddr_in	ServerSocket::getServerAddress(void)
{
	return(_serverAddress);
}

int	ServerSocket::getAddressLen(void)
{
	return (_addressLen);
}
