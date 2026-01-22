#pragma once

#include <iostream>
#include <cstdlib>
#include <errno.h>
#include <sys/socket.h>
#include <socket_utils.hpp>
#include <cstring>
#include <signal.h>

#define BUF_SIZE 1024

#include <Socket.hpp>
#include <ServerSocket.hpp>

#define BUF_SIZE 1024

#define READ 0
#define EXEC 1
#define WRITE 2

class ProcessExecution;

class ClientSocket : public Socket
{
	public :
		ClientSocket(ServerSocket &serverSocket);
		~ClientSocket();

		void	readRequest();
		void	readProcess();
		// TODO send()

		unsigned int	getStatus();

		//void	setProcess(ProcessExecution *process);

	private :
		// ServerSocket &	_serverSocket;
		unsigned int		_status;
		std::string			_clientRequest;
		std::string			_processResponse;
		int					_processFd;
		ProcessExecution	_process;

		int createSocket(ServerSocket &);
};
