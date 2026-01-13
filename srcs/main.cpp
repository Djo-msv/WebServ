#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <string>
#include <cstring>
#include <cstdlib>

// This will basically correspond to the FD of our server
int serverSocket = -1;

void	stopServer(int)
{
	close(serverSocket);
	exit(1);
}

void	manageRequests()
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

int	main(void)
{
	//TODO sera défini par le parser
	int port = 7500;
	
	// AF_INET is used to allow ipv4 connection.
	// SOCK_STREAM is to tell the socket to use TCP protocol
	if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		return (0);

	// sockaddr_in is an un obfuscated version of sockaddr
	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(port);

	// INADDR_ANY : accept connection from any IP address
	serverAddress.sin_addr.s_addr = INADDR_ANY;

	if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
		std::cerr << "Error while binding adress to the server socket" << std::endl;
		close(serverSocket);
		return 1;
	}

	signal(SIGINT, stopServer);

	// listen at the server socket and allow 5 connexions at a time
	if (listen(serverSocket, 5) == -1) {
		std::cerr << "Error while listening" << std::endl;
		close(serverSocket);
		return 1;
	}

	std::cout << "Server started on port " << port << std::endl;
	manageRequests();
	return (0);
}



