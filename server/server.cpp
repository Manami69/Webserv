#include "server.hpp"

Server::Server(void) :
_sockfd(0),
_server_nbr(0) {
	return ;
}

Server::~Server(){
}

void	Server::setup_server_socket(std::string const &port) {
	get_master_socket_fd();
	_listen = new Listen();
	_listen->sockfd = _sockfd;
	_listen->port = atoi(port.c_str());
	_listen->host = "127.0.0.1"; // Need to accepter other host later
	
	memset(&_listen->addr, 0, sizeof(sockaddr_in));
	_listen->addr.sin_family = AF_INET;
	_listen->addr.sin_addr.s_addr = inet_addr(_listen->host.c_str());
	_listen->addr.sin_port = htons(_listen->port);
	return;
}

void	Server::get_master_socket_fd(void) {
	// TCP, IPv4
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	std::cout << "Socket FD : " << sockfd << std::endl; // DELETE LATER
	if (sockfd == -1)
	{
		close(sockfd);
		throw std::runtime_error ("Failed to create socket. <" + std::string(strerror(errno)) + ">");
	}
	_sockfd = sockfd;
	std::cout << GREEN << "Socket successfully created... " << RESET << std::endl;
	return ;
}

void	Server::set_socket_reuse(void) {
	// set socket to allow multiple connections
	int opt = true;
	int	ret = setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char *>(&opt), sizeof(opt));
	if (ret == -1)
	{
		close(_sockfd);
		throw std::runtime_error ("Failed to set socket reuse. <" + std::string(strerror(errno)) + ">");
	}
	std::cout << GREEN << "Set socket reuse successfully... " << RESET << std::endl;
}

void	Server::binded(void) {
	// bind the socket to localhost port 8080
	int	ret = bind(_listen->sockfd, (const struct sockaddr*)&_listen->addr, sizeof(_listen->addr));
	if (ret == -1)
	{
		close(_listen->sockfd);
		throw std::runtime_error ("Failed to bind to port " + std::to_string(_listen->port) + "<" + std::string(strerror(errno)) + ">");
	}
	std::cout << GREEN << "Socket successfully binded..." << RESET << std::endl;
	return ;
}

void	Server::listened(void) {
	// try to specify maximum of 128 pending connections for the socket
    int ret = ::listen(_listen->sockfd, 128);
	if (ret == -1)
	{
		close(_listen->sockfd);
		throw std::runtime_error ("Failed to listen on socket. <" + std::string(strerror(errno)) + ">");
	}
	std::cout << GREEN << "Socket successfully listened..." << RESET << std::endl << std::endl;
	return ;
}

void	Server::add_server_lst(void) {
	_server_lst.push_back(_listen);
	_server_nbr += 1;
	return ;
}

int		Server::get_server_nbr(void) const {
	return (_server_nbr);
}