#include "getRequest.hpp"
#include <sys/socket.h> // For socket functions
#include <netinet/in.h> // For sockaddr_in
#include <cstdlib> // For exit() and EXIT_FAILURE
#include <iostream> // For cout
#include <unistd.h> // For read

#define BLACK   "\033[1m\033[30m"       /* Bold Black */
#define RED     "\033[1m\033[31m"      /* Bold Red */
#define GREEN   "\033[1m\033[32m"       /* Bold Green */
#define YELLOW  "\033[1m\033[33m"       /* Bold Yellow */
#define BLUE    "\033[1m\033[34m"       /* Bold Blue */
#define MAGENTA "\033[1m\033[35m"       /* Bold Magenta */
#define CYAN    "\033[1m\033[36m"	    /* Bold Cyan */
#define WHITE   "\033[1m\033[37m"
#define RESET   "\033[0m"

void		send(int connection, const std::string s)
{
	::send(connection, s.c_str(), s.size(), 0);
}

int main() {
	// Create a socket (IPv4, TCP)
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		std::cout << "Failed to create socket. errno: " << std::endl;
		exit(EXIT_FAILURE);
	}

	// Listen to port 80 on any address
	sockaddr_in sockaddr;
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr = INADDR_ANY;
	sockaddr.sin_port = htons(8080); // htons is necessary to convert a number to
									// network byte order
	if (bind(sockfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0) {
		std::cout << "Failed to bind to port 80. errno: " << std::endl;
		exit(EXIT_FAILURE);
	}

	// Start listening. Hold at most 500 connections in the queue
	if (listen(sockfd, 500) < 0) {
		std::cout << "Failed to listen on socket. errno: " << std::endl;
		exit(EXIT_FAILURE);
	}

	// Wait connection
	while (1)
	{
		std::cout << MAGENTA << "Wait connection ..." << RESET << std::endl;
		// Grab a connection from the queue
		size_t addrlen = sizeof(sockaddr);
		int connection = accept(sockfd, (struct sockaddr*)&sockaddr, (socklen_t*)&addrlen);
		if (connection < 0) {
			std::cout << "Failed to grab connection. errno: " << std::endl;
			exit(EXIT_FAILURE);
		}

		std::cout << GREEN << "Connected :" << RESET << std::endl;
		char buffer[10000];
		size_t bytesRead = read(connection, buffer, 10000);
		static_cast<void>(bytesRead);
		std::cout << buffer;
		getRequest a(buffer);
		std::cout << a;
		send(connection, "HTTP/1.1 200 OK\n");
		send(connection, "content returned: <!DOCTYPE html>\n");
		send(connection, "\n");
		send(connection, """\
			<html>\
			<body>\
			<h1>Hello World</h1> I'm Terry!\
			</body>\
			</html>\
			""");
		// Close the connections
		close(connection);
		std::cout << BLUE << "\nDiconnected" << RESET << std::endl;
	}
	close(sockfd);
}
