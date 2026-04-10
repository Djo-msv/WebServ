#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <string>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <exception>

//fonctions annexes de utils.cpp
std::string switch_replace(std::string s, std::string n1, std::string n2);
std::string file_to_string(std::string filename);
std::string make_response(std::string body);
std::string value_extract(std::string request, std::string key);
bool check_logins(std::string filename, std::string key, std::string value);


// This will basically correspond to the FD of our server
int serverSocket = -1;

void	stopServer(int)
{
	close(serverSocket);
	exit(1);
}

//tous les changements sont ici !
void	manageRequests()
{
	while (true) {
		int clientSocket = accept(serverSocket, NULL, NULL);
		if (clientSocket == -1) {
			std::cerr << "An error occurred when recieving client connection" << std::endl;
			continue;
		}

		std::cout << "New Connection accepted" << std::endl << std::endl;

		char buffer[1024] = {0};
		ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
		
		try {
		if (bytesRead > 0) {
			buffer[bytesRead] = '\0';
			std::string request = buffer;
			std::cout << "Client Message :\n" << request << std::endl;
			std::string httpResponse;
			//3 options :: bien log in, erreur de log in, page de login
			if (request.find("/login.html") == 5)
			{
				//on extrait les infos du corps du client request
				std::string name = value_extract(request, "username");
				std::string pwd = value_extract(request, "password");
				if (check_logins("./logs/logins.txt", name, pwd))
				{
					httpResponse = file_to_string("./html/success.html");
					httpResponse = switch_replace(httpResponse, "[username]", name);
					httpResponse = make_response(httpResponse);
				}
				else
				{
					httpResponse = file_to_string("./html/retry.html");
					httpResponse = make_response(httpResponse);
				}
			}
			else
			{
				httpResponse = file_to_string("./html/login.html");
				httpResponse = make_response(httpResponse);
			}

			std::cout << "Server Response :\n\n" << httpResponse << std::endl << std::endl;
			send(clientSocket, httpResponse.c_str(), httpResponse.length(), 0);
		}}
		catch (std::exception &e)
		{
			std::cout << "error here : " << e.what() << std::endl;
		}

		close(clientSocket);
		std::cout << "Connection closed" << std::endl;
	}
}

int	main(void)
{
	//TODO sera défini par la config (parser nécéssaire on verra pour définir sur quel standard partir)
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
