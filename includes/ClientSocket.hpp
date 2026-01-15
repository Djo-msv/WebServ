#pragma once

#include <iostream>
#include <cstdlib>
#include <errno.h>
#include <sys/socket.h>

#include "socket_utils.hpp"
#include "ServerSocket.hpp"

#define BUF_SIZE 1024

#define READ 0
#define WRITE 1

class ClientSocket
{
	public :
		ClientSocket(int epollInstance, ServerSocket &serverSocket);
		~ClientSocket();

		int		read();
		// TODO send()

		int		getSocketFd();
		bool	getStatus();

	private :
		int				_clientSocketFd;
		ServerSocket	_serverSocket;
		std::string	_clientRequest;
		bool		_status;
};

typedef std::map<int, ClientSocket *>::iterator ClientSocketIterator;
