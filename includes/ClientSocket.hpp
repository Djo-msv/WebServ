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
#include <Request.hpp>
#include <Response.hpp>

//ok ok, trying it out here

class ClientSocket : public Socket
{
	public :
		ClientSocket(ServerSocket &serverSocket);
		~ClientSocket();

		//untouched readRequest() so far with parsing added
		void	readRequest();
		//pipeProcess(), a write/read call to the Response->actionExec() ; write/read + adjust on status
		void	pipeProcess();
		//parseRequest() :: todo
		//void	parseRequest();
		void sendResponse() const;

		int getProcessFd() const;
		void step();

		//void	setProcess(ProcessExecution *process);

		enum state {
			Start,
			ReadRequest,
			WriteProcess,
			WaitProcess,
			ReadProcess,
			SendResponse
		};
		state	getStatus();

		class GatewayTimeout : public HttpError {
			public :
				GatewayTimeout(); 
		};

	private :
		ServerSocket &		_serverSocket;
		state				_status;
		Request				_request;
		Response			_response;
		
		std::string			_processResponse;
		int					_processFd;
		ProcessExecution	_process;

		int 	createSocket(ServerSocket &);
};
