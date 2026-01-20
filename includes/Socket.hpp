#pragma once

#include <stdexcept>
#include <unistd.h>
#include <map>


class Socket
{
	protected :
		const int _socketFd;
	public :
		Socket(int fd);
		virtual ~Socket();

		int getSocketFd();

		class SocketError : public std::runtime_error {
			public :
				SocketError(const std::string msg) : std::runtime_error(msg) {}
		};
};

typedef std::map<const int, Socket *>::iterator SocketIterator;
