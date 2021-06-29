#include "../includes/server.hpp"

Server::Server( void ) {
	_sockfd = 0;
	_server_nbr = 0;
	return ;
}

Server::~Server( void ) {
}

void	Server::setup_server_socket(Config conf, int idx) {
	get_master_socket_fd();
	_listen = new Listen();
	_listen->sockfd = _sockfd;
	_listen->port = atoi(conf.get_config(idx)->port.c_str());
	_listen->host = conf.get_config(idx)->host;
	
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
	/* TCP, IPv4 */
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	//std::cout << "Socket FD : " << sockfd << std::endl; // DELETE LATER
	if (sockfd == -1)
	{
		close(sockfd);
		//throw std::runtime_error ("Failed to create socket. <" + std::string(strerror(errno)) + ">");
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
		std::stringstream ss;
		ss << _listen->port;
		throw std::runtime_error ("Failed to bind to port " + ss.str() + "<" + std::string(strerror(errno)) + ">");
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

void	Server::selected(Config conf) {
	while (true) {
		FD_ZERO(&_read_set);
    	FD_ZERO(&_read_copy);
		
		std::vector<Listen*>::iterator it;
		for (it = _server_lst.begin(); it != _server_lst.end(); ++it) {
			FD_SET((*it)->sockfd, &_read_set);
			_max_fd = (*it)->sockfd;
			//std::cout << "Max sd server: " << _max_fd << std::endl;
		}

		//std::cout << std::endl << "//// CLIENT LIST ////" << std::endl;
		for (_iter = _client_lst.begin(); _iter != _client_lst.end(); ++_iter) {
			int sd = (*_iter).first;
			//std::cout << "client sd : " << (*_iter).first  << " and its server order : "  << (*_iter).second << std::endl;
			if(sd > 0)  
                FD_SET(sd , &_read_set);  
			if(sd > _max_fd) {
				_max_fd = sd;
				//std::cout << "Max sd clien: " << _max_fd << std::endl;
			}
		}
		_read_copy = _read_set;
		int ret = select((_max_fd + 1), &_read_copy, 0, 0, 0);

		//std::cout << std::endl << "select ret : " << ret << std::endl;

		if ((ret == -1) && (errno != EINTR))
			throw std::runtime_error ("An error occurred with select. <" + std::string(strerror(errno)) + ">");
		for (int fd = 0; fd <= _max_fd; ++fd) {
            if (FD_ISSET(fd, &_read_copy))
				this->process_socket(conf, fd);
        }
	}
}

void	Server::process_socket(Config conf, int fd) {

	//std::cout << std::endl << "Process_socket fd : " << fd << std::endl;

	int	server_order = this->is_sockfd_found(fd);

	//std::cout << "server_order : " << server_order << std::endl;

	if (server_order > 0) {
        // listener socket is readable => accept the connection and create communication socket
        uint32_t addrlen = sizeof(_server_lst[server_order - 1]->addr);
		int comm = accept(fd, (struct sockaddr *)&_server_lst[server_order - 1]->addr, (socklen_t *)&addrlen);
		if (comm == -1)
			throw std::runtime_error ("Failed to accept. <" + std::string(strerror(errno)) + ">");
		else {
			std::cout << std::endl << GREEN << "Server acccept new client ! (fd=" << comm << ")" << RESET << std::endl;
			fcntl(comm, F_SETFL, O_NONBLOCK);
			_client_lst.insert(std::pair<int, int>(comm, server_order));
		}
    }
	else {
		ssize_t size_recv, total_size = 0;
		struct timeval begin , now;
		char	buf[BUFSIZE + 1];
		double	timediff;
		std::string *buffff = new std::string;
		FD_SET(fd, &_read_set);
		gettimeofday(&begin , NULL);
		while(true)
		{
			//std::cout << "readddddd" << std::endl;
			gettimeofday(&now , NULL);
			timediff = (now.tv_sec - begin.tv_sec) + 1e-6 * (now.tv_usec - begin.tv_usec);
			if( total_size > 0 && timediff > 4 )
			{
				break;
			}
			else if( timediff > 4*2)
			{
				break;
			}
			memset(&buf, 0, sizeof(buf));
			if ((size_recv = recv(fd, buf, sizeof(buf), 0)) < 0)
			{
				usleep(100000);
			}
			else
			{
				// std::cout << GREEN << buf << RESET;
				total_size += size_recv;
				for (ssize_t j = 0; j < size_recv ; j++)
					buffff->push_back(buf[j]);
				gettimeofday(&begin , NULL);
			}
		}
		

		std::cout << YELLOW << *buffff << RESET;

		std::cout << "total_size : " << total_size << std::endl;

		if (!buffff->empty()) //// a changer
		{
			getRequest a(*buffff);
			std::cout <<YEL << a["Content-Type"] << END << std::endl;
			delete buffff;
			// use fd to find server idx in _client_lst<int(fd), int(server idx)>,
			// and use the server idx for the function conf.get_config(idx)
			getResponse response(a);
			this->error_code();
		//std::cout << a << response.responsetosend(_err);
			send(fd, response.responsetosend(_err));
			// close and clear fd
			_iter = _client_lst.find(fd);
			if (_iter != _client_lst.end())
				_client_lst.erase(_iter);
			FD_CLR(fd, &_read_set);
			close(fd);
		}
		else

			delete buffff;
		
		// memset(&buf, 0, sizeof(buf));
		//}
		if (size_recv == 0) {
			std::cout << std::endl << GREEN << "Connection lost... (fd=" << fd << ")" << RESET << std::endl;
			_iter = _client_lst.find(fd);
			if (_iter != _client_lst.end())
				_client_lst.erase(_iter);
			FD_CLR(fd, &_read_set);
			close(fd);
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
