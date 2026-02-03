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

//this func :: deletes the old_fd from our epoll map (except pipes, which are automatically deleted), adds the new one with flags
//it then erases the <old_fd, csocket> from the sockets map and replaces it with <new_fd, csocket>
void	fd_switch(int old_fd, int new_fd, ClientSocket *csocket, int flags)
{
	SocketIterator it = sockets.find(old_fd);
	if (it == sockets.end())
		throw std::out_of_range("incorrect old fd, not in sockets map");
	sockets.erase(it);
	sockets.insert(std::pair<int, Socket *>(new_fd, csocket));
	if (old_fd == csocket->getSocketFd())
		epoll_del(epollInstance, old_fd, EPOLLOUT);
	epoll_add(epollInstance, new_fd, flags);
}

//this function now does the reads + fd_switches (obv once the parsing is separate the switch-case will be post parsing instead of readrequest
void	managePendingClients()
{	
	if (pendingClientSockets.empty())
		return ;
	for (std::deque<ClientSocket *>::iterator it = pendingClientSockets.begin(); it != pendingClientSockets.end(); ++it) {
		bool done = (it + 1 == pendingClientSockets.end());
		ClientSocket *csocket = *it;
		try
		{
			if (csocket->getStatus() == ClientSocket::ReadRequest)
			{
				csocket->readRequest();
				switch (csocket->getStatus()) {
				//if parsing is done, we switch the fds according to status
					case ClientSocket::SendResponse:
						epoll_mod(epollInstance, csocket->getSocketFd(), EPOLLOUT | EPOLLET);
						break ;
					case ClientSocket::WriteProcess:
						fd_switch(csocket->getSocketFd(), csocket->getProcessFd(), csocket, EPOLLOUT | EPOLLET);
						break ;
					case ClientSocket::WaitProcess:
						fd_switch(csocket->getSocketFd(), csocket->getProcessFd(), csocket, EPOLLIN | EPOLLET);
						break ;
					default:
						break ;
				}
			}
			else if (csocket->getStatus() == ClientSocket::ReadProcess)
			{
				csocket->pipeProcess();
				//if pipeOut is read, response is done, send it
				if (csocket->getStatus() == ClientSocket::SendResponse)
					fd_switch(csocket->getProcessFd(), csocket->getSocketFd(), csocket, EPOLLOUT | EPOLLET);
			}
			//if we're done reading (request and pipe), boot from the client list
			if (csocket->getStatus() == ClientSocket::SendResponse)
			{
				it = pendingClientSockets.erase(it);
				if (done)
					return ;
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
				// WTF ?? THROW ERROR -> literally impossible
			};
			ServerSocket *sSocket = dynamic_cast<ServerSocket *>(socketIterator->value);
			if (sSocket != NULL) {
				ClientSocket *csocket = new ClientSocket(*sSocket);
				//adding the client to sockets + epoll
				epoll_add(epollInstance, csocket->getSocketFd(), EPOLLIN | EPOLLET);
				sockets.insert(std::make_pair(csocket->getSocketFd(), 
							csocket));
				//pendingClientSockets.push_back(csocket); //too early
			}
			else {
				ClientSocket *cSocket = dynamic_cast<ClientSocket *>(socketIterator->value);
				try {
				switch (cSocket->getStatus()) {
					case ClientSocket::Start:
						//first POLLIN alert from epoll : start reading
						pendingClientSockets.push_back(cSocket);
						cSocket->step();
						break ;
					case ClientSocket::SendResponse:
						//first, send the response back
						cSocket->sendResponse();
						//then, close the client socket and erase the client from the sockets map
						sockets.erase(socketIterator);
						epoll_del(epollInstance, cSocket->getSocketFd(), EPOLLOUT);
						close(cSocket->getSocketFd());
						delete cSocket;
						//if keep-alive, edit above
						break ;
					case ClientSocket::WriteProcess:
						cSocket->pipeProcess();
						fd_switch(events[n].data.fd, cSocket->getProcessFd(), cSocket, EPOLLIN | EPOLLET);
						break ;
					case ClientSocket::WaitProcess:
						//process is ready to be read : start reading
						cSocket->step();
						break ;
					default:
						break ;
				}
				}
				CATCH_AND_HANDLE(std::exception)
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
