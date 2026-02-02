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
#include <HttpError.hpp>
#include <ServerSocket.hpp>
#include <ProcessExecution.hpp>
#include <../parsing_tests/parsing/Request.hpp>

class ClientSocket : public Socket
{
	public :
		ClientSocket(ServerSocket &serverSocket);
		~ClientSocket();

		void	readRequest();
		void	readProcess();
		void	execProcess();
		void	parseRequest();
		// TODO send()

		unsigned int	getStatus();

		//void	setProcess(ProcessExecution *process);

		enum state {
			ReadRequest,
			ParseRequest,
			ExecProcess,
			ReadProcess,
			SendResponse
		};

		class GatewayTimeout : public HttpError {
			public :
				GatewayTimeout(); 
		};

	private :
		ServerSocket &		_serverSocket;
		state				_status;
		Request				_request;
		
		std::string			_processResponse;
		int					_processFd;
		ProcessExecution	_process;

		int 	createSocket(ServerSocket &);
};
