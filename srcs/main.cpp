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
	for (std::deque<ClientSocket *>::iterator it = pendingClientSockets.begin(); it != pendingClientSockets.end(); ++it) {
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
					sockets.erase(sockets.find(cSocket->getSocketFd()));
					epoll_del(epollInstance, cSocket->getSocketFd(), EPOLLOUT);
					delete cSocket;
					if (it == pendingClientSockets.end())
						return ;
					break ;
				default:
					break ;
			}
		} CATCH_AND_HANDLE(std::exception)
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
				//adding the client to sockets + epoll + pendingList
				if (!sockets.count(cSocket->getSocketFd())) {
					epoll_add(epollInstance, cSocket->getSocketFd(), EPOLLIN | EPOLLET);
					sockets.insert(std::make_pair(cSocket->getSocketFd(), 
							cSocket));
					pendingClientSockets.push_back(cSocket);
				}
			}
			else {
				ClientSocket *cSocket = dynamic_cast<ClientSocket *>(socketIterator->value);
				switch (cSocket->_status) {
					case ClientSocket::WaitRequest:
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
		}
		managePendingClients();
	}
}

/**
 * ! Fonction Temporaire a modifier une fois le parsing terminé
*/
ServerConfig initConfig()
{
	std::map<std::string, std::string> cgiHandlers;
	std::map<std::string, int> requestsFlag;
	std::string index_file("/html/retry.html");
	std::string rootFolder(".");
	std::string execFolder("/scripts");

	cgiHandlers.insert(std::make_pair(".py", "/usr/bin/python3"));
	requestsFlag.insert(std::make_pair("/html", ServerConfig::GET));
	requestsFlag.insert(std::make_pair("/img", ServerConfig::GET));
	requestsFlag.insert(std::make_pair("/scripts", ServerConfig::GET | ServerConfig::POST));// 0 = Rien, rajouter un | pour plus de flags
	return ServerConfig(cgiHandlers, requestsFlag, index_file, rootFolder, execFolder);
}

int	main(void)
{
	//TODO sera défini par la config (parser nécéssaire on verra pour définir sur quel standard partir)
	// AF_INET is used to allow ipv4 connection.
	// SOCK_STREAM is to tell the socket to use TCP protocol
	ServerConfig config = initConfig();
	config.sin_family = AF_INET;
	config.sin_port = 7500;
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
