#pragma once

#include <iostream>
#include <cstdlib>
#include <errno.h>
#include <sys/socket.h>
#include <socket_utils.hpp>

#define BUF_SIZE 1024

#include <Socket.hpp>
#include <ServerSocket.hpp>

#define BUF_SIZE 1024

#define READ 0
#define WRITE 1

class ClientSocket : public Socket
{
	public :
		ClientSocket(ServerSocket &serverSocket);
		~ClientSocket();

		void	read();
		// TODO send()

		bool	getStatus();

	private :
		// ServerSocket &	_serverSocket;
		std::string		_clientRequest;
		bool			_status;

		int createSocket(ServerSocket &);
};
