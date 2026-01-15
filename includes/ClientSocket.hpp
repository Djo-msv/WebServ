#pragma once

#include <iostream>
#include <cstdlib>
#include <errno.h>
#include <sys/socket.h>
#include <Socket.hpp>
#include <socket_utils.hpp>

#define BUF_SIZE 1024

class ClientSocket : public Socket
{
	public :
		ClientSocket(int epollInstance, int server_socket);
		~ClientSocket();

		void read();

	private :
		int			_clientSocket;
		std::string	_clientRequest;
};

