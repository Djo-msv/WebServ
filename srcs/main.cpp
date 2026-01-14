#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <string>
#include <cstring>
#include <cstdlib>
#include <sys/epoll.h>
#include <errno.h>

#define MAX_EVENTS 10
#include <vector>
#include <sys/epoll.h>

#include "ServerSocket.hpp"

std::vector<ServerSocket *> serverSockets;

void	stopServer(int)
{
	for (ServerSocketIterator it = serverSockets.begin(); it != serverSockets.end(); ++it)
		delete *it;
	exit(0);
}



void	manageRequests(ServerSocket *socket, int epollInstance)
{
	int	conn_sock;
	int nfds;
	struct epoll_event ev;
	struct epoll_event events[MAX_EVENTS];

	while (true) {
		// nfds is number of file descriptors ready for the requested I/O operation.
		// Specifying a timeout of -1 causes epoll_wait() to block indefinitely
		if ((nfds = epoll_wait(epollInstance, events, MAX_EVENTS, -1)) == -1) {
			throw std::runtime_error("an error occured when wait connection to the fd : '" + \
				ft_itoa(socket->getSocketFd()) + \
				"'. Error code : " + ft_itoa(errno));
			exit(1);
		}
		for (int n = 0; n < nfds; ++n) {
			//TODO handle multiple listen_stock with map
			if (events[n].data.fd == socket->getSocketFd()) {
				// recover the value of client fd, variables that are set to NULL represent the client informations could be useful later...
				conn_sock = accept(socket->getSocketFd(), NULL, NULL);
				if (conn_sock == -1) {
					throw std::runtime_error("an error occured when accepting connection to the fd : " + \
						ft_itoa(socket->getSocketFd()) + \
						". Error code : " + ft_itoa(errno));
				}
				setnonblocking(conn_sock);
				// add client to the list
				ev.events = EPOLLIN | EPOLLET;
				ev.data.fd = conn_sock;
				if (epoll_ctl(epollInstance, EPOLL_CTL_ADD, conn_sock, &ev) == -1) {
					throw std::runtime_error("an error occured when allowing connection to the fd : '" + \
						ft_itoa(socket->getSocketFd()) + \
						"'. Error code : " + ft_itoa(errno));
				}
			}
			else {
				// TODO client packet response handling
//				do_use_fd(events[n].data.fd);
			}
		}
	}
}

int	main(void)
{
	//TODO sera défini par la config (parser nécéssaire on verra pour définir sur quel standard partir)
	int port = 7500;
	
	// AF_INET is used to allow ipv4 connection.
	// SOCK_STREAM is to tell the socket to use TCP protocol
	int epollInstance = epoll_create(1);
	ServerConfig config = (ServerConfig) {port, AF_INET};
	try
	{
		ServerSocket *socket = new ServerSocket(config, epollInstance);
		signal(SIGINT, stopServer);

		manageRequests(socket, epollInstance);
	} catch (std::runtime_error e)
	{
		for (ServerSocketIterator it = serverSockets.begin(); it != serverSockets.end(); ++it)
			delete *it;
		std::cout << e.what() << std::endl;
		return (1);
	}
	return (0);
}
