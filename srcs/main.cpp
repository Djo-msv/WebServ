#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <string>
#include <cstring>
#include <cstdlib>

// This will basically correspond to the FD of our server

void	stopServer(int)
{
	exit(1);
}

void	manageRequests(int serverSocket)
{
	while (true) {
		int clientSocket = accept(serverSocket, NULL, NULL);
		if (clientSocket == -1) {
			std::cerr << "An error occurred when recieving client connection" << std::endl;
			continue;
		}

		std::cout << "New Connection accepted" << std::endl;

		char buffer[1024] = {0};
		ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
		
		if (bytesRead > 0) {
			buffer[bytesRead] = '\0';
			std::cout << "Client Message :\n" << buffer << std::endl;

			// a \r\n is required when changing instruction in http
			std::string httpResponse = 
				"HTTP/1.1 200 OK\r\n"
				"Content-Type: text/html\r\n"
				"Content-Length: 53\r\n"
				"Connection: close\r\n"
				"\r\n"
				"<html><body><h1>Hello from WebServ!</h1></body></html>";

			send(clientSocket, httpResponse.c_str(), httpResponse.length(), 0);
		}

		close(clientSocket);
		std::cout << "Connection closed" << std::endl;
	}
}

#include "ServerSocket.hpp"

int	main(void)
{
	//TODO sera défini par la config (parser nécéssaire on verra pour définir sur quel standard partir)
	int port = 7500;
	
	// AF_INET is used to allow ipv4 connection.
	// SOCK_STREAM is to tell the socket to use TCP protocol
	ServerSocket *socket = new ServerSocket(port, AF_INET);
	signal(SIGINT, stopServer);

	manageRequests(socket->getSocketFd());
	return (0);
}



