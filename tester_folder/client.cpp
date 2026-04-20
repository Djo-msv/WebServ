#include <stdio.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

#define BUF_SIZE 700

int main(int argc, char *argv[])
{
	int              sfd, s;
	char             buf[BUF_SIZE +1];
	ssize_t           size;
	struct addrinfo  hints;
	struct addrinfo  *result, *rp;

	if (argc < 3) {
		fprintf(stderr, "Usage: %s host port...\n", argv[0]);
		return 1;
	}

	/* Obtain address(es) matching host/port.  */

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
	hints.ai_socktype = SOCK_STREAM; /* Datagram socket */
	hints.ai_flags = 0;
	hints.ai_protocol = 0;          /* Any protocol */

	s = getaddrinfo(argv[1], argv[2], &hints, &result);
	if (s != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
		return 1;
	}

	/* getaddrinfo() returns a list of address structures.
	Try each address until we successfully connect(2).
	If socket(2) (or connect(2)) fails, we (close the socket
	and) try the next address.  */

	for (rp = result; rp != NULL; rp = rp->ai_next) {
		sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (sfd == -1)
			continue;

		if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1)
			break;                  /* Success */

		close(sfd);
	}

	freeaddrinfo(result);           /* No longer needed */

	if (rp == NULL) {               /* No address succeeded */
		fprintf(stderr, "Could not connect\n");
		return 1;
	}
	std::string sd;
	//---------------here the different options ::
		
		//method not allowed/implemented
	//sd = "DELETE /html HTTP/1.1\r\nHostname: localhost\r\n\r\n";
	sd = "GET /html HTTP/2.1\r\nHostname: localhost\r\n\r\n";
	
		//bad request
	//sd = "GET	target	HTTP/1.1\r\n\r\n";
	//sd = "GET tar get HTTP/1.1\r\n\r\n";
	//sd = "POST /scripts/login.py HTTP/1.1\r\nHostname: localhost\r\n\r\ndddddddddddddddddddd";
	//sd = "POST /scripts/login.py HTTP/1.1\r\nHostna*me: localhost\r\n\r\n";
	//sd = "                                POST /scripts/login.py HTTP/1.1\r\nHostname: localhost\r\n\r\n";
	//sd = "POST\r\nHostname: localhost\r\n\r\n";
	
		//timeout (unfinished request)
	//sd = "HERE AN UNFINISHED REQUEST\r\n";
	//sd = "GET /html HTTP/1.1\r\nContent-Type: text/plain\r\nContent-Length: 6\r\n\r\nfff";
	//sd = "GET /html HTTP/1.1\r\nContent-Type: text/plain\r\nTransfer-Encoding: chunked\r\n\r\n4\r\ndddd\r\n";
	//sd = "";
	
		//regular client behavior
	//sd = "GET /html HTTP/1.1\r\nHostname: localhost\r\n\r\n";
	//sd = "POST /logs HTTP/1.1\r\nContent-Type: text/plain\r\nContent-Length: 28\r\nHostname: localhost\r\n\r\nffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff";
	//sd = "GET /logs HTTP/1.1\r\nHostname: localhost\r\n\r\n";
	//sd = "DELETE /uploads/to_delete.txt HTTP/1.1\r\nHostname: localhost\r\n\r\n";
	
	//std::cout << "sending message :: " << sd << "\n";
	size = sd.size();
	if (write(sfd, sd.c_str(), sd.size()) < size) {
		//std::cerr << "couldnt write whole message :: " << sd << std::endl;
		return 1;
	}
	//std::cout << "reading\n";
	ssize_t n = read(sfd, buf, BUF_SIZE);
	while (n != 0) {
		if (n == -1) { perror("read"); return 1; }
		buf[n] = '\0';
		std::cout << "message received :: " << buf << std::endl;
		//std::cout << "sending message\n";
		size = sd.size();
		if (write(sfd, sd.c_str(), sd.size()) < size) {
			//std::cerr << "couldnt write whole message :: " << sd << std::endl;
			return 1;
		}
		n = read(sfd, buf, BUF_SIZE);
	}
	
	//std::cout << "job done\n";
	close(sfd);
	return 0;
}
