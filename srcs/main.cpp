#include <main.hpp>

std::map<std::string, std::string> mime;
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

void closeSocketconnection(ClientSocket *cSocket)
{
	sockets.erase(sockets.find(cSocket->getSocketFd()));
	epoll_del(epollInstance, cSocket->getSocketFd(), EPOLLOUT);
	delete cSocket;
}

//this function now does the reads + fd_switches (obv once the parsing is separate the switch-case will be post parsing instead of readrequest
void	managePendingClients()
{	
	for (std::deque<ClientSocket *>::iterator it = pendingClientSockets.begin(); it != pendingClientSockets.end(); ++it) {
		ClientSocket *cSocket = *it;
		try {
			switch (cSocket->status) {
				case ClientSocket::ReadRequest:
					cSocket->readRequest();
					break ;
				case ClientSocket::WaitRequest:
					if (cSocket->hasTimedOut())
						cSocket->status = ClientSocket::Done;
					break;
				case ClientSocket::ParseRequest:
					cSocket->parseRequest();
					break ;
				case ClientSocket::ExecWrite:
					cSocket->execWrite();
					break ;
				case ClientSocket::ExecRead:
					cSocket->execRead();
					break ;
				case ClientSocket::WaitResponse:
					if (cSocket->hasTimedOut())
						cSocket->status = ClientSocket::Done;
					break;
				case ClientSocket::SendResponse:
					cSocket->sendResponse();
					break ;
				case ClientSocket::Done:
					std::cout << "closing connection with socket : " << cSocket->getSocketFd() << std::endl;
					it = pendingClientSockets.erase(it);
					closeSocketconnection(cSocket);
					if (it == pendingClientSockets.end())
						return ;
					break ;
				default:
					break ;
			}
		} CATCH_AND_HANDLE(std::exception)
	}
}

void handle_events(epoll_event events[], int nbfds)
{
	for (int n = 0; n < nbfds; ++n) {
			SocketIterator socketIterator = sockets.find(events[n].data.fd);
			if (socketIterator == sockets.end()) {
				std::cout << "WTf ?" << std::endl;
				// WTF ?? THROW ERROR -> literally impossible
			};
			ServerSocket *sSocket = dynamic_cast<ServerSocket *>(socketIterator->value);
			if (sSocket != NULL) {
				ClientSocket *cSocket = new ClientSocket(*sSocket, epollInstance, sockets, mime);
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
				switch (cSocket->status) {
					case ClientSocket::WaitRequest:
						cSocket->status = ClientSocket::ReadRequest;
						break ;
					case ClientSocket::WaitExecWrite:
						cSocket->status = ClientSocket::ExecWrite;
						break ;
					case ClientSocket::WaitExecRead:
						cSocket->status = ClientSocket::ExecRead;
						break ;
					case ClientSocket::WaitResponse:
						cSocket->status = ClientSocket::SendResponse;
						break ;
					default:
						break ;
				}
			}
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
		handle_events(events, nbfds);
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
	std::map<int, std::string> errorFiles;
	std::string index_file("/html/index.html");
	std::string rootFolder("./server00");
	std::string execFolder("/scripts");

	cgiHandlers.insert(std::make_pair(".py", "/usr/bin/python3"));
	requestsFlag.insert(std::make_pair("/html", ServerConfig::GET));
	requestsFlag.insert(std::make_pair("/img", ServerConfig::GET));
	requestsFlag.insert(std::make_pair("/uploads", ServerConfig::DELETE));
	requestsFlag.insert(std::make_pair("/scripts", ServerConfig::GET | ServerConfig::POST));// 0 = Rien, rajouter un | pour plus de flags
	errorFiles.insert(std::make_pair(404, rootFolder + "/html/errors/404_def.html"));
	return (ServerConfig(cgiHandlers, requestsFlag, index_file, rootFolder, execFolder, errorFiles, 15)); // timeout en secondes
}

void initServerSockets(Parser &tree)
{
	std::list<MymlObject *> root = tree.getRoot();

	for (std::list<MymlObject *>::iterator it = root.begin(); it != root.end(); ++it)
	{
		if (!(*it)->isDictionnary())
			continue ;

		MymlDictionary *serverRepertory = (*it)->getAsDictionnary();
		try	{
			ServerConfig config = initServerConfig(serverRepertory);
			ServerSocket *socket = new ServerSocket(config, epollInstance);
			sockets.insert(std::make_pair(socket->getSocketFd(), socket));
		}
		catch (std::exception &e){
			std::cout << e.what() << std::endl;
			continue ;
		}
	}
}

ServerConfig initServerConfig(MymlDictionary *serverRepertory)
{
	std::map<std::string, std::string> cgiHandlers;
	std::map<std::string, int> requestsFlag;
	std::map<int, std::string> errorFiles;
	
	try
	{
		std::string index_file(serverRepertory->getValueAsString("index"));
		std::string rootFolder(serverRepertory->getValueAsString("root_folder"));
	}
	catch (std::bad_cast &e) { throw e; }
	catch (std::invalid_argument &e) { throw std::invalid_argument(std::string("missing mandatory argument : ") + e.what()); }
	
	try
	{
		std::string execFolder;
	}
	
}

int	main(int argc, char **argv)
{
	//TODO sera défini par la config (parser nécéssaire on verra pour définir sur quel standard partir)
	// AF_INET is used to allow ipv4 connection.
	// SOCK_STREAM is to tell the socket to use TCP protocol
	if (argc < 2)
	{
		std::cout << "usage : " << argv[0] << " <config file/folder>" << std::endl;
		return (1);
	}
	
	Parser	tree(argv[1]);
	
	//ServerConfig config = initConfig();
	mime = initMimetype();
	//config.sin_family = AF_INET;
	//config.sin_port = 7500;
	try
	{
		// ServerSocket *socket = new ServerSocket(config, epollInstance);
		initServerSockets(tree);
		signal(SIGINT, stopServer);
		signal(SIGPIPE, SIG_IGN);
		// sockets.insert(std::make_pair(socket->getSocketFd(), socket));
		manageRequests();
	}
	CATCH_AND_HANDLE(std::runtime_error)
	CATCH_AND_HANDLE(std::bad_alloc)
}
