#pragma once

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdexcept>
#include <map>
#include <socket_utils.hpp>
#include <Socket.hpp>

struct ServerConfig
{
	int 		sin_port;
	sa_family_t sin_family;
	std::string	cgi_path;
	std::string	index_file;
	std::string	name;
};

class ServerSocket : public Socket
{

	public :
		ServerSocket(ServerConfig, int epollInstance);
		~ServerSocket();

	private :
		ServerConfig _config;	

		int			createSocket(ServerConfig &);
		sockaddr_in	setupSocketAddress(ServerConfig);
		
		// Error class
		class BindError : public std::runtime_error {
			public :
				BindError(const std::string msg) : std::runtime_error(msg) {}
		};
		class ListenError : public std::runtime_error {
			public :
				ListenError(const std::string msg) : std::runtime_error(msg) {}
		};
};
