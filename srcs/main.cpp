#include <netinet/in.h>
#include <signal.h>
#include <cstring>
#include <cstdlib>
#include <sys/epoll.h>

#define MAX_EVENTS 10
#define key first
#define value second
#include "ServerSocket.hpp"
#define CATCH_AND_HANDLE(ExceptionType) \
    catch (const ExceptionType& e) { \
        handleError(e.what()); \
	}

std::map<int, ServerSocket *> serverSockets;

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
	int	client_socket;
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
			//TODO handle multiple listen_stock with map
			ServerSocketIterator ssocket = serverSockets.find(events[n].data.fd);
			if (ssocket != serverSockets.end()) {
				// recover the value of client fd, variables that are set to NULL represent the client informations could be useful later...
				client_socket = accept(ssocket->key, NULL, NULL);
				if (client_socket == -1) {
					throw std::runtime_error("An error occured while allowing connection to the fd : '" + \
						ft_itoa(ssocket->key) + "'. Error code : " + ft_itoa(errno));
				}
				setnonblocking(client_socket);
				// add client to the list
				epoll_add(epollInstance, client_socket, EPOLLIN | EPOLLET);
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
	ServerConfig config = (ServerConfig) {port, AF_INET, 0, 0};
	try
	{
		ServerSocket *socket = new ServerSocket(config, epollInstance);
		signal(SIGINT, stopServer);

		manageRequests(epollInstance);
	}
	CATCH_AND_HANDLE(std::runtime_error)
	CATCH_AND_HANDLE(std::bad_alloc)
	return (0);
}
