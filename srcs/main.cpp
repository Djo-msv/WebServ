#include <netinet/in.h>
#include <signal.h>
#include <cstring>
#include <cstdlib>
#include <sys/epoll.h>

#define MAX_EVENTS 10
#define key first
#define value second
#include "ServerSocket.hpp"
#include "ClientSocket.hpp"
#define CATCH_AND_HANDLE(ExceptionType) \
    catch (const ExceptionType& e) { \
        handleError(e.what()); \
	}

std::map<int, ServerSocket *> serverSockets;
std::map<int, ClientSocket *> clientSockets;

void	stopServer(int)
{
	for (ServerSocketIterator it = serverSockets.begin(); it != serverSockets.end(); ++it)
		delete it->value;
	exit(0);
}

void handleError(const char* msg)
{
	for (ServerSocketIterator it = serverSockets.begin(); it != serverSockets.end(); ++it)
		delete it->value;
    std::cerr << msg << std::endl;
	exit(1);
}

void	manageRequests(int epollInstance)
{
	int nbfds;
	epoll_event events[MAX_EVENTS];

	while (true) {
		// nbfds defines the number of file descriptors ready for the requested I/O operation.
		// Specifying a timeout of -1 causes epoll_wait() to block indefinitely
		if ((nbfds = epoll_wait(epollInstance, events, MAX_EVENTS, -1)) == -1) {
			throw std::runtime_error("An error has occured while waiting for connections. Error code : " + ft_itoa(errno));
			exit(1);
		}
		for (int n = 0; n < nbfds; ++n) {
			ServerSocketIterator ssocketIterator = serverSockets.find(events[n].data.fd);
			if (ssocketIterator != serverSockets.end()) {
				ClientSocket *csocket = new ClientSocket(epollInstance, *ssocketIterator->value); 
				clientSockets.insert(std::make_pair(csocket->getSocketFd(), 
							csocket));
			}
			else {
				ClientSocketIterator csocketIterator = clientSockets.find(events[n].data.fd);
				if (csocketIterator == clientSockets.end() || csocketIterator->value->getStatus() != WRITE){}
					//trow error
				// TODO 
				// TODO exec CGI
				// TODO client packet response handling
//				do_use_fd(events[n].data.fd);
			}
		}
		for (std::map<int, ClientSocket *>::iterator it = clientSockets.begin(); it != clientSockets.end(); it++) {
			ClientSocket *csocket = it->value;
			if (csocket->getStatus() != READ)
				continue;
			csocket->read();
			if (csocket->getStatus() == WRITE)
				epoll_add(epollInstance, csocket->getSocketFd(), EPOLLOUT | EPOLLET);
		}
	}
}

// client fd 1
// server fd 2

int	main(void)
{
	//TODO sera défini par la config (parser nécéssaire on verra pour définir sur quel standard partir)
	int port = 7500;
	
	// AF_INET is used to allow ipv4 connection.
	// SOCK_STREAM is to tell the socket to use TCP protocol
	int epollInstance = epoll_create(1);
	ServerConfig config = (ServerConfig) {port, AF_INET, 0, 0};
	try
	{
		ServerSocket *socket = new ServerSocket(config, epollInstance);
		signal(SIGINT, stopServer);
		(void)socket;
		manageRequests(epollInstance);
	}
	CATCH_AND_HANDLE(std::runtime_error)
	CATCH_AND_HANDLE(std::bad_alloc)
	return (0);
}
