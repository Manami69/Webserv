#include "server.hpp"

Server::Server(void) :
_sockfd(0),
_server_nbr(0) {
	memset(&_buf, 0, sizeof(_buf));
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
	in_addr_t address = inet_addr(_listen->host.c_str());
	if (address == INADDR_NONE)
	{
		throw std::runtime_error ("Bad ip address. <" + std::string(strerror(errno)) + ">");
	}
	_listen->addr.sin_family = AF_INET;
	_listen->addr.sin_addr.s_addr = address;
	_listen->addr.sin_port = htons(_listen->port);
	return ;
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
	_server_nbr = this->get_server_nbr();
	return ;
}

int		Server::get_server_nbr(void) const {
	return (_server_lst.size());
}

std::vector<Listen*>	Server::get_server_lst(void) const {
	return (_server_lst);
}

void	Server::selected(void) {
	while (true) {
		FD_ZERO(&_read_set);
    	FD_ZERO(&_read_copy);
		
		std::vector<Listen*>::iterator it;
		for (it = _server_lst.begin(); it != _server_lst.end(); ++it) {
			FD_SET((*it)->sockfd, &_read_set);
			_max_fd = (*it)->sockfd;
		}

		for (_iter = _client_lst.begin(); _iter != _client_lst.end(); ++_iter) {
			int sd = *_iter;
			if(sd > 0)  
                FD_SET(sd , &_read_set);  
			if(sd > _max_fd) {
				_max_fd = sd;
			}
		}
		_read_copy = _read_set;
		int ret = select((_max_fd + 1), &_read_copy, 0, 0, 0);
		if ((ret == -1) && (errno != EINTR))
		{
			throw std::runtime_error ("An error occurred with select. <" + std::string(strerror(errno)) + ">");
		}
		for (int fd = 0; fd <= _max_fd; ++fd) {
            if (FD_ISSET(fd, &_read_copy))
				this->process_socket(fd);
        }
	}
}

void	Server::process_socket(int fd) {
	int	server_order = this->is_sockfd_found(fd);
	if (server_order > 0)
    {
        // listener socket is readable => accept the connection and create communication socket
        uint32_t addrlen = sizeof(_server_lst[server_order - 1]->addr);
		int comm = accept(fd, (struct sockaddr *)&_server_lst[server_order - 1]->addr, (socklen_t *)&addrlen); // add 2nd and 3rd arguments
		if (comm == -1)
			throw std::runtime_error ("Failed to accept. <" + std::string(strerror(errno)) + ">");
		else
		{
			std::cout << std::endl << GREEN << "Server acccept new client ! (fd=" << comm << ")" << RESET << std::endl;
			fcntl(comm, F_SETFL, O_NONBLOCK);
			FD_SET(comm, &_read_set);
			_client_lst.push_back(comm);
		}
    }
	else {
		ssize_t bytesRecv = recv(fd, _buf, sizeof(_buf), 0);
        if (bytesRecv == 0)
        {
			std::cout << std::endl << GREEN << "Connection lost... (fd=" << fd << ")" << RESET << std::endl;
			_iter = std::find(_client_lst.begin(), _client_lst.end(), fd);
			if (_iter != _client_lst.cend()) {
        		int index = std::distance(_client_lst.begin(), _iter);
				_client_lst.erase(_client_lst.begin() + index);
    		}
			FD_CLR(fd, &_read_set);
			close(fd);
        }
		else if (bytesRecv == -1)
		{
			throw std::runtime_error ("Failed to receive connection. <" + std::string(strerror(errno)) + ">");
		}
		else
		{
			std::cout << _buf;
			memset(&_buf, 0, sizeof(_buf));
		}
	}
}

int	Server::is_sockfd_found(int fd) {
	int i = 1;
	std::vector<Listen*>::iterator it;
	for (it = _server_lst.begin(); it != _server_lst.end(); ++it) {
		if (fd == (*it)->sockfd)
			return (i);
		i++;
	}
	return (false);
}

int		Server::get_client_socket_size() const {
	return (_client_lst.size());
}