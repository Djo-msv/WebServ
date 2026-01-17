#pragma once

#include <stdexcept>
#include <unistd.h>
#include <map>


class Socket
{
	protected :
		const int _socketFd;
	public :
		Socket();
		virtual ~Socket();

		int getSocketFd();
	
		virtual int createSocket() = 0;

		class SocketError : public std::runtime_error {
			public :
				SocketError(const std::string msg) : std::runtime_error(msg) {}
		};
};

typedef std::map<const int, Socket *>::iterator SocketIterator;
