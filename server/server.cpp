#include "server.hpp"

Server::Server(int sockfd, std::string port, std::string host) {
	
	_serv_data = new Serv();
	_serv_data->sockfd = sockfd;
	_serv_data->port = atoi(port.c_str());
	_serv_data->host = host;
	
	memset(&_serv_data->addr, 0, sizeof(sockaddr_in));
	_serv_data->addr.sin_family = AF_INET;
	_serv_data->addr.sin_addr.s_addr = inet_addr(_serv_data->host.c_str());
	_serv_data->addr.sin_port = htons(_serv_data->port);
	
	memset(&_buf, 0, sizeof(_buf));

	FD_ZERO(&_read_set);
    FD_ZERO(&_read_copy);
    FD_SET(_serv_data->sockfd, &_read_set);
	return;
}

Server::~Server(void) {
	return ;
}

void	Server::binded(void) {
	// bind the socket to localhost port 8080
	int	ret = bind(_serv_data->sockfd, (const struct sockaddr*)&_serv_data->addr, sizeof(_serv_data->addr));
	if (ret == -1)
	{
		close(_serv_data->sockfd);
		throw std::runtime_error ("Failed to bind to port " + std::to_string(_serv_data->port) + "<" + std::string(strerror(errno)) + ">");
	}
	else
		std::cout << GREEN << "Socket successfully binded..." << RESET << std::endl;
	return ;
}

void	Server::listened(void) {
	// try to specify maximum of 128 pending connections for the socket
    int ret = ::listen(_serv_data->sockfd, 128);
	if (ret == -1)
	{
		close(_serv_data->sockfd);
		throw std::runtime_error ("Failed to listen on socket. <" + std::string(strerror(errno)) + ">");
	}
	else
		std::cout << GREEN << "Socket successfully listened..." << RESET << std::endl << std::endl;
	return ;
}

void	Server::selected(void) {
	while (true) {
        _read_copy = _read_set;
        int ret = select(FD_SETSIZE, &_read_copy, 0, 0, 0);
		if ((ret == -1) && (errno != EINTR))
    	{
			close(_serv_data->sockfd);
			throw std::runtime_error ("An error occurred with select. <" + std::string(strerror(errno)) + ">");
    	}
        for (int fd = 0; fd <= FD_SETSIZE; ++fd)
        {
			if (FD_ISSET(fd, &_read_copy))
				this->process_socket(fd);
        }
    }
}

void	Server::process_socket(int fd) {
	if (fd == _serv_data->sockfd) {
        // listener socket is readable => accept the connection and create communication socket
        uint32_t addrlen = sizeof(_serv_data->addr);
		int comm = accept(_serv_data->sockfd, (struct sockaddr *)&_serv_data->addr, (socklen_t *)&addrlen);
		if (comm == -1)
        {
			close(_serv_data->sockfd);
			throw std::runtime_error ("Failed to accept. <" + std::string(strerror(errno)) + ">");
       	}
		else
		{
			FD_SET(comm, &_read_set);
			std::cout << GREEN << "Server acccept new client ! (fd=" << comm << ")" << RESET << std::endl;
		}
    }
	else {
		// communication socket is readable => read in the data
		for (size_t i = 0; i < sizeof(_buf); i++)
			_buf[i] = 0;
		std::string buffer;
        ssize_t bytesRecv = recv(fd, _buf, sizeof(_buf), 0);
        if (bytesRecv == 0)
        {
			std::cout << GREEN << "Connection lost... (fd=" << fd << ")" << RESET << std::endl;
			FD_CLR(fd, &_read_set);
			close(fd);
        }
		else if (bytesRecv == -1)
		{
			close(fd);
			throw std::runtime_error ("Failed to receive connection. <" + std::string(strerror(errno)) + ">");
		}	
        else
		{
			buffer += _buf;
			if (!(_buf[bytesRecv - 1] == LF && _buf[bytesRecv - 2] == CR ) || !(bytesRecv == 1 && _buf[0] == LF) )
				buffer += _read_socket(fd, bytesRecv);
			std::cout << buffer;
			getRequest a(buffer);
			getResponse response(a);
			this->error_code();
			std::cout << a << response.responsetosend(_err);
			send(fd, response.responsetosend(_err));
			memset(&_buf, 0, sizeof(_buf));
		}
	}
}

std::map<int, std::string> Server::error_code(void) {
	_err[200] = "OK";
	_err[400] = "Bad Request";
	_err[404] = "Not Found";
	_err[505] = "HTTP Version Not Supported";
	return _err;
}

void	Server::send(int connection, const std::string s)
{
	::send(connection, s.c_str(), s.size(), 0);
	return ;
}

void	run_server_socket(int sockfd, std::string port, std::string host) {
	Server server(sockfd, port, host);
	server.binded();
	server.listened();
	server.selected();
	return ;
}

void	set_server_socket(int server, char **av) {
	int	opt = true;
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		close(sockfd);
		throw std::runtime_error ("Failed to create socket. <" + std::string(strerror(errno)) + ">");
	}
	else
		std::cout << GREEN << "Socket successfully created..." << RESET << std::endl;
	
	// set socket to allow multiple connections
	int	ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char *>(&opt), sizeof(opt));
	if (ret == -1)
	{
		close(sockfd);
		throw std::runtime_error ("Failed to set socket reuse. <" + std::string(strerror(errno)) + ">");
	}
	if (server == 0)
		run_server_socket(sockfd, av[1], av[2]);
	if (server == 1)
		run_server_socket(sockfd, av[3], av[4]);
	if (server == 2)
		run_server_socket(sockfd, av[5], av[6]);
	return ;
}


std::string Server::_read_socket(int fd, ssize_t& bytesRecv)
{
	std::string buf;

	if (bytesRecv == 1 && static_cast<int>(_buf[0]) == LF)
		return "";
	for (size_t i = 0; i < sizeof(_buf); i++)
		_buf[i] = 0;
	bytesRecv = recv(fd, _buf, sizeof(_buf), 0);
    if (bytesRecv == 0)
    {
		std::cout << GREEN << "Connection lost... (fd=" << fd << ")" << RESET << std::endl;
		FD_CLR(fd, &_read_set);
		close(fd);
		return "";
    }
	else if (bytesRecv == -1)
	{
		close(fd);
		throw std::runtime_error ("Failed to receive connection. <" + std::string(strerror(errno)) + ">");
	}	
    else
	{
		buf += _buf;
		if (!(static_cast<int>(_buf[bytesRecv -  2]) == CR && static_cast<int>(_buf[bytesRecv - 1]) == LF ))
			buf += _read_socket(fd, bytesRecv);
	}
	return buf;
}