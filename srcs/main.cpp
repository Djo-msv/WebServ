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
	if (pendingClientSockets.empty())
		return ;
	for (std::deque<ClientSocket *>::iterator it = pendingClientSockets.begin(); it != pendingClientSockets.end(); ++it) {
		bool done = (it + 1 == pendingClientSockets.end());
		ClientSocket *csocket = *it;
		try
		{
			switch (csocket->getStatus())
			{
				case ClientSocket::ReadRequest:
					csocket->readRequest();
					break ;
				case ClientSocket::SendResponse:
					epoll_mod(epollInstance, csocket->getSocketFd(), EPOLLOUT | EPOLLET);
					pendingClientSockets.erase(it);
					if (done)
						return ;
					break ;
				//this wont be here after :: it needs to be handled at the e-poll tree
				default:
					while (csocket->getStatus() != ClientSocket::SendResponse)
						csocket->readProcess();
			}
		} CATCH_AND_HANDLE(std::exception)//catch(const std::exception& e){ throw; }
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
				// WTF ?? THROW ERROR -> literally impossible lmao
			};
			ServerSocket *sSocket = dynamic_cast<ServerSocket *>(socketIterator->value);
			if (sSocket != NULL) {
				ClientSocket *csocket = new ClientSocket(*sSocket);
				//lmao we were missing the most important part ; incredible
				epoll_add(epollInstance, csocket->getSocketFd(), EPOLLIN | EPOLLET);
				sockets.insert(std::make_pair(csocket->getSocketFd(), 
							csocket));
				//pendingClientSockets.push_back(csocket); //too early, i think
			}
			else {
				ClientSocket *cSocket = dynamic_cast<ClientSocket *>(socketIterator->value);
				//future exec (cgi-write + cgi-read) will be handled here as well,
				//by switching out socketFd for processFd in sockets map (so we're sure where the problem)
				//and epoll_del then _add again the socketFd once exec is over and we're ready to send response
				if (cSocket->getStatus() == ClientSocket::Start) {
					pendingClientSockets.push_back(cSocket);
					cSocket->step();
					//continue ; // Impossible variable -> starting reading or still reading
				}
				else if (cSocket->getStatus() == ClientSocket::SendResponse) {
					//first, send the response back
					try { cSocket->sendResponse(); }
					CATCH_AND_HANDLE(std::exception)
					//then, close the client socket and erase the client from the sockets map
					sockets.erase(socketIterator);
					epoll_del(epollInstance, cSocket->getSocketFd(), EPOLLOUT);
					close(cSocket->getSocketFd());
					delete cSocket;
				}
//					do_use_fd(events[n].data.fd);
			}
		}
		// TODO Pareil déplacer dans une fonction "readClients"
		managePendingClients();
	}
}

int	main(void)
{
	//TODO sera défini par la config (parser nécéssaire on verra pour définir sur quel standard partir)
	int port = 7500;
	
	// AF_INET is used to allow ipv4 connection.
	// SOCK_STREAM is to tell the socket to use TCP protocol
	ServerConfig config = (ServerConfig) {port, AF_INET, "", ""};
	try
	{
		ServerSocket *socket = new ServerSocket(config, epollInstance);
		signal(SIGINT, stopServer);
		signal(SIGPIPE, SIG_IGN);
		sockets.insert(std::make_pair(socket->getSocketFd(), socket));
		manageRequests();
	}
	CATCH_AND_HANDLE(std::runtime_error)
	CATCH_AND_HANDLE(std::bad_alloc)
	return (0);
}
