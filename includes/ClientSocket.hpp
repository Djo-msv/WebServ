#pragma once

#include <iostream>
#include <cstdlib>
#include <errno.h>
#include <sys/socket.h>
#include <socket_utils.hpp>
#include <cstring>
#include <signal.h>
#include <ctime>

#define BUF_SIZE 5000//1024

#include <Socket.hpp>
#include <HttpErrors.hpp>
#include <ServerSocket.hpp>
#include <ProcessExecution.hpp>
#include <Request.hpp>
#include <Response.hpp>
#include <main.hpp>

class ClientSocket : public Socket
{
	public :
		ClientSocket(ServerSocket &serverSocket, int epoll, std::map<const int, Socket *> &sockets, std::map<std::string, std::string> &mime);
		~ClientSocket();

		//untouched readRequest(), now with adequate status-update
		void	readRequest();
		//parseRequest, with optional switch-back to readRequest if more data is needed
		void	parseRequest();
		//for the DELETE method, deletes file and adjusts response accordingly
		void	deleteFile(const char *filename);
		//writing to cgi pipein + appropriate _status, epoll and sockets update && starting cgi once write is over
		void	execWrite();
		void	writeToRead(); //in case of exec write timeout, switch to exec read
		//reading from running cgi pipeout + appropriate _status, epoll and sockets update
		void	execRead();
		void	readToWrite(); //in case of exec read timeout, switch to response write
		//sends response in buffers, currently closes connection in the future .clear() on all objects
		void sendResponse();
		//to switch from SendResponse back to WaitRequest, resets all variables and epoll_mods to EPOLLIN
		void reset();

		void	resetTimeout();
		bool	hasTimedOut() const;

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
			Done //close connection
		};
		state				status;

	private :
		std::map<const int, Socket *> &sockets;
		std::map<std::string, std::string> &mime;

		ServerSocket &		_serverSocket;
		const int 			epollInstance;
		time_t				timeout;
		Request				_request;
		ProcessExecution	_exec;
		Response			_response;
		size_t _sendpos;
		
		int 	createSocket(ServerSocket &);
		void	epollFdSwitch(int old_fd, int new_fd, int flags);
		void	errorHandling(HttpError &e);
		void	startExec(); //handles the fd_switches, setnonblock and starting the execution (if read-only)
};
