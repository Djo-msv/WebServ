#pragma once

#include <stdexcept>
#include <unistd.h>


class Socket
{
	protected :
		const int _socketFd;
	public :
		Socket();
		~Socket();

		const int getSocketFd();
	
		virtual int createSocket() = 0;

		class SocketError : public std::runtime_error {
			public :
				SocketError(const std::string msg) : std::runtime_error(msg) {}
		};
};