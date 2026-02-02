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
#include "ProcessExecution.hpp"
#include "MymlParser.hpp"

#define CATCH_AND_HANDLE(ExceptionType) \
    catch (const ExceptionType& e) { \
        handleError(e.what()); \
	}

std::map<const int, Socket *>	sockets;
std::deque<ClientSocket *>		pendingClientSockets;
const int 						epollInstance = epoll_create(1);

void	stopServer(int)
{
	for (SocketIterator it = sockets.begin(); it != sockets.end(); ++it)
		delete it->value;
	close(epollInstance);
	exit(0);
}

void handleError(const char* msg)
{
	for (SocketIterator it = sockets.begin(); it != sockets.end(); ++it)
		delete it->value;
	close(epollInstance);
    std::cerr << msg << std::endl;
	exit(1);
}

void	managePendingClients()
{	
	for (std::deque<ClientSocket *>::iterator it = pendingClientSockets.begin(); it != pendingClientSockets.end(); ++it) {
			ClientSocket *csocket = *it;

			if (csocket->getStatus() == ClientSocket::Read_request)
				csocket->readRequest();
			else if (csocket->getStatus() == ClientSocket::Read_process)
				csocket->readProcess();
			// if (csocket->getStatus() == WRITE) {
			// 	epoll_add(epollInstance, csocket->getSocketFd(), EPOLLOUT | EPOLLET);
			// 	it = pendingClientSockets.erase(it);
			// 	if (it == pendingClientSockets.end())
			// 		break ;
			// }
	}
}

void	manageRequests()
{
	int 						nbfds;
	epoll_event 				events[MAX_EVENTS];

	while (true) {
		// nbfds defines the number of file descriptors ready for the requested I/O operation.
		// Specifying a timeout of -1 causes epoll_wait() to block indefinitely
		if ((nbfds = epoll_wait(epollInstance, events, MAX_EVENTS, 10)) == -1)
			throw std::runtime_error("An error has occured while waiting for connections. Error code : " + ft_itoa(errno));
		// TODO Boucle for a déplacer dans une fonction "handle_epoll_events"
		for (int n = 0; n < nbfds; ++n) {
			SocketIterator socketIterator = sockets.find(events[n].data.fd);
			if (socketIterator == sockets.end()) {
				std::cout << "WTf ?" << std::endl;
				// WTF ?? THROW ERROR
			};
			ServerSocket *sSocket = dynamic_cast<ServerSocket *>(socketIterator->value);
			if (sSocket != NULL)
			{
				ClientSocket *csocket = new ClientSocket(*sSocket); 
				sockets.insert(std::make_pair(csocket->getSocketFd(), 
							csocket));
				pendingClientSockets.push_back(csocket);
			}
			else {
				ClientSocket *cSocket = dynamic_cast<ClientSocket *>(socketIterator->value);
				if (cSocket->getStatus() != ClientSocket::Write) {};
					//trow error
				// TODO client packet response handling
//				do_use_fd(events[n].data.fd);
			}
		}
		// TODO Pareil déplacer dans une fonction "readClients"
		managePendingClients();
	}
}

int	main(int argc, char **argv)
{
	if (argc == 2)
		MymlParser	config(argv[1]);
	else
		return (1);
	return (0);
}
