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
#include <ProcessExecution.hpp>
#include <Request.hpp>

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

		enum state {
			Read_request,
			Parse_request,
			Read_process,
			Write
		};

	private :
		// ServerSocket &	_serverSocket;
		state				_status;
		std::string			_clientRequest;
		// Request				_request;
		std::string			_processResponse;
		int					_processFd;
		ProcessExecution	_process;

		int createSocket(ServerSocket &);
};
