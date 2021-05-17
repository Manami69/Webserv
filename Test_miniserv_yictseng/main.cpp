#include <sys/socket.h> // socket functions
#include <netinet/in.h> // sockaddr_in
#include <cstdlib> // exit() and EXIT_FAILURE
#include <cstring> // memset
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h> // struct timeval
#include <iostream>
#include <unistd.h>

#define RED     "\033[1m\033[31m" 
#define GREEN   "\033[1m\033[32m" 
#define YELLOW  "\033[1m\033[33m"  
#define BLUE    "\033[1m\033[34m" 
#define MAGENTA "\033[1m\033[35m"
#define RESET   "\033[0m"

int		main()
{
	// socket creation and verification
	// int sockfd = socket(domain, type, protocol);
	// sockfd: socket descriptor, an integer (like a file-handle)
	// domain: integer, communication domain e.g., AF_INET (IPv4 protocol) , AF_INET6 (IPv6 protocol)
	// type: communication type ; SOCK_STREAM: TCP(reliable, connection oriented) ;SOCK_DGRAM: UDP(unreliable, connectionless)
	// protocol: Protocol value for Internet Protocol(IP), which is 0.
	
	int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == -1)
	{
		std::cout << "Socket creation failed. Errno: " << errno << std::endl;
		exit(EXIT_FAILURE);
	}
	else
		std::cout << "Socket successfully created..." << std::endl;

	// NOT SURE IF WE NEED TO USE FCNTL OR NOT; IF YES HOW TO USE IT WITH SELECT ?
	// int flags = fcntl(server_fd, F_GETFL); // the subject doesn't allow us to use flags other than F_SETFL and O_NONBLOCK
	// if (flags == -1)					   	  // need to find another way to do it
	// {
	// 	std::cout << "Could not get flags on TCP listening socket" << std::endl;
	// 	exit(EXIT_FAILURE);
	// }
	// // O_NONBLOCK : Non-blocking I/O;
	// // if no data is available to a read call, or if a write operation would block,
	// // the read or write call returns -1 with the error EAGAIN.
	// if (fcntl(server_fd, F_SETFL, flags | O_NONBLOCK) == -1)
	// 	std::cout << "Could not set TCP listening socket to be non-blocking" << std::endl;


	// assign IP, Port
	sockaddr_in	server_addr;
	memset(&server_addr, 0, sizeof(sockaddr_in));
	
	// The address family that we used when we set up the socket.
	server_addr.sin_family = AF_INET;

	// The address for this socket. The special address for this is 0.0.0.0,
	// defined by the symbolic constant INADDR_ANY.
	// htonl converts a long integer (e.g. address) to a network representation 
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	// The port number (the transport address).
	// Server : pick a specific number since clients will need to know a port number to connect to.
	// htons converts a short integer (e.g. port) to a network representation
	server_addr.sin_port = htons(80);

	// int bind(int socket, const struct sockaddr *address, socklen_t address_len);
	// After creation of the socket, bind function binds the socket to the address
	// and port number specified in addr(custom data structure).
	if ((bind(server_fd, (const struct sockaddr*)&server_addr, sizeof(server_addr))) != 0)
	{
		std::cout << "Socket bind failed. errno: " << errno << std::endl;
		exit(EXIT_FAILURE);
	}
	else
		std::cout << "Socket successfully binded..." << std::endl;
	
	// int listen(int socket, int backlog)
	// It puts the server socket in a passive mode, where it waits for the client to approach the server to make a connection.
	// backlog defines max length of pending queue, if it's full, the client may receive an error with an indication of ECONNREFUSED.
	if ((listen(server_fd, 10)) != 0)
	{
		std::cout << "Socket cannot listen. errno: " << errno << std::endl;
		exit(EXIT_FAILURE);
	}
	
	fd_set sockSet; // Set of socket descriptors for select()
	// struct timeval selTimeout; // Timeout for select()

	while(true)
    {
        std::cout << MAGENTA << "+++++++ Waiting for new connection ++++++++" << RESET << std::endl;
		
		// Zero socket descriptor vector and set for server sockets
    	// This must be reset every time select() is called
		FD_ZERO(&sockSet);
        FD_SET(server_fd, &sockSet); // if we have a list of clients that we listen to, they all need to be initialized.

		// Timeout specification
    	// This must be reset every time select() is called
    	// selTimeout.tv_sec = 0;     // timeout (secs.)
    	// selTimeout.tv_usec = 0;    // 0 microseconds 

		
		// int select (int nfds, fd_set *read-fds, fd_set *write-fds, fd_set *except-fds, struct timeval *timeout);
		int res = select(server_fd + 1, &sockSet, NULL, NULL, NULL);
		if (res == -1)
    	{
    	    std::cout << "Select : an error occurred. errno: " << errno << std::endl;
			exit(EXIT_FAILURE);
    	}
		// if a socket is ready
		else if (res > 0)
		{
			std::cout << res << std::endl;
			if (FD_ISSET(server_fd, &sockSet)) // need to check also if we have a list of clients
			{
				// int new_socket = accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
				int addrlen = sizeof(server_addr);
				int new_socket = accept(server_fd, (struct sockaddr *)&server_addr, (socklen_t *)&addrlen);
        		if (new_socket == -1)
        		{
					std::cout << "Server acccept failed. errno: " << errno << std::endl;
            		exit(EXIT_FAILURE);
       			}
        		else
					std::cout << GREEN << "Server acccept client..." << RESET << std::endl;
				char buffer[1024]; // limits ?
       			read(new_socket, buffer, 1024); // here we cannot check errno
        		std::cout << buffer;
				//ssize_t send(int socket, const void *buffer, size_t length, int flags);
				std::string msg("HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!");
				send(new_socket, msg.c_str(), msg.size(), 0);
				close(new_socket);
			}
		}
    }
	close(server_fd);
    return 0;
}
