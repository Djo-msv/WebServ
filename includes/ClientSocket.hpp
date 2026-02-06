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

class ClientSocket : public Socket
{
	public :
		ClientSocket(ServerSocket &serverSocket, int _epoll, std::map<const int, Socket *> &_sockets);
		~ClientSocket();

		//untouched readRequest(), now with adequate status-update
		void	readRequest();
		//parseRequest, with optional switch-back to readRequest if more data is needed
		void	parseRequest();
		//writing to cgi pipein + appropriate _status, epoll and sockets update && starting cgi once write is over
		void	execWrite();
		//reading from running cgi pipeout + appropriate _status, epoll and sockets update
		void	execRead();
		//sends response in buffers, currently closes connection in the future .clear() on all objects
		void sendResponse();

		enum state {
			WaitRequest,
			ReadRequest,
			ParseRequest,
			WaitExecWrite,
			ExecWrite,
			WaitExecRead,
			ExecRead,
			WaitResponse,
			SendResponse,
			Done//temporary, since we don't have request/response/exec .clear() functions yet
		};
		state				_status;

		class GatewayTimeout : public HttpError {
			public :
				GatewayTimeout(); 
		};

	private :
		ServerSocket &		_serverSocket;
		const int epollInstance;
		std::map<const int, Socket *> &sockets;
		
		Request				_request;
		ProcessExecution		_exec;
		Response			_response;
		
		std::string			_processResponse;

		int 	createSocket(ServerSocket &);
		void	fd_switch(int old_fd, int new_fd, int flags);
		void	startExec(); //handles the fd_switches, setnonblock and starting the execution (if read-only)
};
