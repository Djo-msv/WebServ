#pragma once

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdexcept>
#include <vector>
#include "socket_utils.hpp"

struct ServerConfig
{
	int sin_port;
	sa_family_t sin_family;
};

class ServerSocket
{

	public :
		ServerSocket(ServerConfig, int epollInstance);
		~ServerSocket();

		int getSocketFd();
		sockaddr_in getServerAddress();
		socklen_t	getSockAdressLength();

	private :
		int			_socketFd;
		socklen_t	_sockaddrLength;
		
		sockaddr_in setupSocketAddress(ServerConfig);
		
		// Error class
		class SocketError : public std::runtime_error {
			public :
				SocketError(const std::string msg) : std::runtime_error(msg) {}
		};
		class BindError : public std::runtime_error {
			public :
				BindError(const std::string msg) : std::runtime_error(msg) {}
		};
		class ListenError : public std::runtime_error {
			public :
				ListenError(const std::string msg) : std::runtime_error(msg) {}
		};
};

typedef std::vector<ServerSocket *>::iterator ServerSocketIterator;
