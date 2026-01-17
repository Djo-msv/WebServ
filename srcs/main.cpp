#include <netinet/in.h>
#include <signal.h>
#include <cstring>
#include <cstdlib>
#include <sys/epoll.h>
#include <deque>

#define MAX_EVENTS 10
#define key first
#define value second
#include "ServerSocket.hpp"
#include "ClientSocket.hpp"
#define CATCH_AND_HANDLE(ExceptionType) \
    catch (const ExceptionType& e) { \
        handleError(e.what()); \
	}

std::map<const int, Socket *> sockets;

void	stopServer(int)
{
	for (SocketIterator it = sockets.begin(); it != sockets.end(); ++it)
		delete it->value;
	exit(0);
}

void handleError(const char* msg)
{
	for (SocketIterator it = sockets.begin(); it != sockets.end(); ++it)
		delete it->value;
    std::cerr << msg << std::endl;
	exit(1);
}

void	manageRequests(int epollInstance)
{
	int 						nbfds;
	epoll_event 				events[MAX_EVENTS];
	std::deque<ClientSocket *>	clientsToRead;

	while (true) {
		// nbfds defines the number of file descriptors ready for the requested I/O operation.
		// Specifying a timeout of -1 causes epoll_wait() to block indefinitely
		if ((nbfds = epoll_wait(epollInstance, events, MAX_EVENTS, -1)) == -1)
			throw std::runtime_error("An error has occured while waiting for connections. Error code : " + ft_itoa(errno));
		// TODO Boucle for a déplacer dans une fonction "handle_epoll_events"
		for (int n = 0; n < nbfds; ++n) {
			SocketIterator socketIterator = sockets.find(events[n].data.fd);
			if (socketIterator == sockets.end()) {};
				// WTF ?? THROW ERROR
			ServerSocket *sSocket = dynamic_cast<ServerSocket *>(socketIterator->value);
			if (sSocket != NULL)
			{
				ClientSocket *csocket = new ClientSocket(*sSocket); 
				sockets.insert(std::make_pair(csocket->getSocketFd(), 
							csocket));
				clientsToRead.push_back(csocket);
			}
			else {
				ClientSocket *cSocket = dynamic_cast<ClientSocket *>(socketIterator->value);
				if (cSocket->getStatus() != WRITE) {};
					//trow error
				// TODO 
				// TODO exec CGI
				// TODO client packet response handling
//				do_use_fd(events[n].data.fd);
			}
		}
		// TODO Pareil déplacer dans une fonction "readClients"
		for (std::deque<ClientSocket *>::iterator it = clientsToRead.begin(); it != clientsToRead.end(); ++it) {
			ClientSocket *csocket = *it;
			csocket->read();
			if (csocket->getStatus() == WRITE) {
				epoll_add(epollInstance, csocket->getSocketFd(), EPOLLOUT | EPOLLET);
				it = clientsToRead.erase(it);
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
