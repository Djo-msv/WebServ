#pragma once

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdexcept>

class ServerSocket
{
	public :
		ServerSocket(int sin_port, sa_family_t sin_family);
		~ServerSocket();

		int getSocketFd();

	private :
		int	_socketFd;

		// Error class
		class SocketError : public std::runtime_error {
			public :
				SocketError(const char *msg) : std::runtime_error(msg) {}
		};
		class BindError : public std::runtime_error {
			public :
				BindError(const char *msg) : std::runtime_error(msg) {}
		};
		class ListenError : public std::runtime_error {
			public :
				ListenError(const char *msg) : std::runtime_error(msg) {}
		};
};
