#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <string>
#include <cstring>
#include <cstdlib>
#include <sys/epoll.h>

// This will basically correspond to the FD of our server
#define MAX_EVENTS 10

void	stopServer(int)
{
	exit(0);
}

#include "ServerSocket.hpp"

void	manageRequests(ServerSocket *socket)
{
	int	conn_sock;
	int epfd, nfds;
	struct epoll_event ev;
	struct epoll_event events[MAX_EVENTS];
	sockaddr_in serverAddress = socket->getServerAddress();

	while (true) {
		// nfds is number of file descriptors ready for the requested I/O operation.
		// Specifying a timeout of -1 causes epoll_wait() to block indefinitely
		if ((nfds = epoll_wait(epfd, events, MAX_EVENTS, -1)) == -1) {
			std::cerr << "epoll failure" << std::endl;
			exit(1);
		}
		socklen_t socklen = static_cast<socklen_t>(socket->getAddressLen());
		for (int n = 0; n < nfds; ++n) {
			//TODO handle multiple listen_stock with map
			if (events[n].data.fd == socket->getSocketFd()) {
				// recover the value of client fd
				conn_sock = accept(socket->getSocketFd(), \
						(struct sockaddr *) &serverAddress, \
						&socklen);
				if (conn_sock == -1) {
					std::cerr << "an error occure when accept" << std::endl;
					exit(1);
				}
//				setnonblocking(conn_sock);
				// add client to the list
				ev.events = EPOLLIN | EPOLLET;
				ev.data.fd = conn_sock;
				if (epoll_ctl(epfd, EPOLL_CTL_ADD, conn_sock, &ev) == -1) {
					std::cerr << "An error occure when epoll create" << std::endl;
					exit(1);
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
	ServerSocket *socket = new ServerSocket(port, AF_INET);
	
	signal(SIGINT, stopServer);

	manageRequests(socket);
	return (0);
}
