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

//this function now does the reads + fd_switches (obv once the parsing is separate the switch-case will be post parsing instead of readrequest
void	managePendingClients()
{	
	if (pendingClientSockets.empty())
		return ;
	for (std::deque<ClientSocket *>::iterator it = pendingClientSockets.begin(); it != pendingClientSockets.end(); ++it) {
		bool done = (*it == pendingClientSockets.back());
		ClientSocket *cSocket = *it;
		try {
			switch (cSocket->_status) {
				case ClientSocket::ReadRequest:
					cSocket->readRequest();
					break ;
				case ClientSocket::ParseRequest:
					cSocket->parseRequest();
					break ;
				case ClientSocket::ExecWrite:
					cSocket->execWrite();
					break ;
				case ClientSocket::ExecRead:
					cSocket->execRead();
					break ;
				case ClientSocket::SendResponse:
					cSocket->sendResponse();
					break ;
				case ClientSocket::Done:
					std::cout << "closing connection with socket : " << cSocket->getSocketFd() << std::endl;
					it = pendingClientSockets.erase(it);
					sockets.erase(sockets.find(cSocket->getSocketFd())); //temp also
					epoll_del(epollInstance, cSocket->getSocketFd(), EPOLLOUT); //temp also
					delete cSocket;
					if (done)
						return ;
					break ;
				default:
					break ;
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
		if ((nbfds = epoll_wait(epollInstance, events, MAX_EVENTS, 100)) == -1)
			throw std::runtime_error("An error has occured while waiting for connections. Error code : " + ft_itoa(errno));
		// TODO Boucle for a déplacer dans une fonction "handle_epoll_events"
		for (int n = 0; n < nbfds; ++n) {
			SocketIterator socketIterator = sockets.find(events[n].data.fd);
			if (socketIterator == sockets.end()) {
				std::cout << "WTf ?" << std::endl;
				// WTF ?? THROW ERROR -> literally impossible
			};
			ServerSocket *sSocket = dynamic_cast<ServerSocket *>(socketIterator->value);
			if (sSocket != NULL) {
				ClientSocket *cSocket = new ClientSocket(*sSocket, epollInstance, sockets);
				//adding the client to sockets + epoll
				if (!sockets.count(cSocket->getSocketFd())) {
					epoll_add(epollInstance, cSocket->getSocketFd(), EPOLLIN | EPOLLET);
					sockets.insert(std::make_pair(cSocket->getSocketFd(), 
							cSocket));
				}
			}
			else {
				ClientSocket *cSocket = dynamic_cast<ClientSocket *>(socketIterator->value);
				try {
				switch (cSocket->_status) {
					case ClientSocket::WaitRequest:
						pendingClientSockets.push_back(cSocket);
						cSocket->_status = ClientSocket::ReadRequest;
						break ;
					case ClientSocket::WaitExecWrite:
						cSocket->_status = ClientSocket::ExecWrite;
						break ;
					case ClientSocket::WaitExecRead:
						cSocket->_status = ClientSocket::ExecRead;
						break ;
					case ClientSocket::WaitResponse:
						cSocket->_status = ClientSocket::SendResponse;
						break ;
					default:
						break ;
				}
				}
				CATCH_AND_HANDLE(std::exception)
			}
		}
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
