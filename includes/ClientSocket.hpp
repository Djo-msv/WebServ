#pragma once

#include <iostream>
#include <cstdlib>
#include <errno.h>
#include <sys/socket.h>
#include <socket_utils.hpp>
#include <cstring>

#define BUF_SIZE 1024

#include <Socket.hpp>
#include <ServerSocket.hpp>

#define BUF_SIZE 1024

#define READ 0
#define EXEC 1
#define WRITE 2

class ClientSocket : public Socket
{
	public :
		ClientSocket(ServerSocket &serverSocket);
		~ClientSocket();

		void	readRequest();
		void	readProcess();
		// TODO send()

		unsigned int	getStatus();

	private :
		// ServerSocket &	_serverSocket;
		std::string		_clientRequest;
		std::string		_processResponse;
		unsigned int	_status;
		int				_processFd;

		int createSocket(ServerSocket &);
};
