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

	std::cout << std::endl << "Process_socket fd : " << fd << std::endl;

	int	server_order = this->is_sockfd_found(fd);

	std::cout << "server_order : " << server_order << std::endl;

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
		ssize_t size_recv;
		char	buf[BUFSIZE + 1];
		std::string *buffff = new std::string;
		FD_SET(fd, &_read_set);
		while(true)
		{
			// add a counter 
			memset(&buf, 0, sizeof(buf));
			size_recv = recv(fd, buf, sizeof(buf), 0);
			for (ssize_t j = 0; j < size_recv ; j++)
				buffff->push_back(buf[j]);
			getRequest a(*buffff);
			std::cout << YELLOW << a << RESET << std::endl;
			// if content-length found
			// body_size += size_recv
			// if body_size == content-length
			// break
			getResponse response(a, *conf.get_config(_client_lst[fd] - 1)); //move out of loop ?
			this->error_code();
			send(fd, response.responsetosend(_err));
			if (buffff->find("\r\n\r\n") != std::string::npos)
				break ;
			// if buffff->find("0\r\n")
			// int end = counter
			// if buffff->find("\r\n") && counter - end == 1
			// break;	
		}
		_iter = _client_lst.find(fd);
		if (_iter != _client_lst.end())
			_client_lst.erase(_iter);
		FD_CLR(fd, &_read_set);
		close(fd);
	}
}

// void	Server::process_socket(Config conf, int fd) {

// 	//std::cout << std::endl << "Process_socket fd : " << fd << std::endl;

// 	int	server_order = this->is_sockfd_found(fd);

// 	//std::cout << "server_order : " << server_order << std::endl;

// 	if (server_order > 0) {
//         // listener socket is readable => accept the connection and create communication socket
//         uint32_t addrlen = sizeof(_server_lst[server_order - 1]->addr);
// 		int comm = accept(fd, (struct sockaddr *)&_server_lst[server_order - 1]->addr, (socklen_t *)&addrlen);
// 		if (comm == -1)
// 			throw std::runtime_error ("Failed to accept. <" + std::string(strerror(errno)) + ">");
// 		else {
// 			std::cout << std::endl << GREEN << "Server acccept new client ! (fd=" << comm << ")" << RESET << std::endl;
// 			fcntl(comm, F_SETFL, O_NONBLOCK);
// 			_client_lst.insert(std::pair<int, int>(comm, server_order));
// 		}
//     }
// 	else {
// 		ssize_t size_recv, total_size = 0;
// 		struct timeval begin , now;
// 		char	buf[BUFSIZE + 1];
// 		double	timediff;
// 		std::string *buffff = new std::string;
// 		FD_SET(fd, &_read_set);
// 		gettimeofday(&begin , NULL);
// 		while(true)
// 		{
// 			//std::cout << "readddddd" << std::endl;
// 			gettimeofday(&now , NULL);
// 			timediff = (now.tv_sec - begin.tv_sec) + 1e-6 * (now.tv_usec - begin.tv_usec);
// 			if( total_size > 0 && timediff > 4 )
// 			{
// 				break;
// 			}
// 			else if( timediff > 4*2)
// 			{
// 				break;
// 			}
// 			memset(&buf, 0, sizeof(buf));
// 			if ((size_recv = recv(fd, buf, sizeof(buf), 0)) < 0)
// 			{
// 				usleep(100000);
// 			}
// 			else
// 			{
// 				// std::cout << GREEN << buf << RESET;
// 				total_size += size_recv;
// 				for (ssize_t j = 0; j < size_recv ; j++)
// 					buffff->push_back(buf[j]);
// 				if (buffff->find("\r\n\r\n") != std::string::npos)
// 					break ;
// 				gettimeofday(&begin , NULL);
// 			}
// 		}
		

// 		std::cout << YELLOW << *buffff << RESET;

// 		std::cout << "total_size : " << total_size << std::endl;

// 		if (!buffff->empty()) //// a changer
// 		{
// 			getRequest a(*buffff);
// 			//std::cout << "GOGOGOGO" << a.get_content_length() << std::endl;
// 			delete buffff;
// 			//use fd to find server idx in _client_lst<int(fd), int(server idx)>,
// 			// and use the server idx for the function conf.get_config(idx)
// 			(void)conf;
// 			std::cout << _client_lst[fd] - 1 << conf.get_config(_client_lst[fd] - 1)->host << std::endl;
// 			getResponse response(a, *conf.get_config(_client_lst[fd] - 1));
// 			std::cout << "HELLO" << std::endl;
// 			this->error_code();
// 			std::cout << a << response.responsetosend(_err);
// 			send(fd, response.responsetosend(_err));
// 			// close and clear fd
// 			_iter = _client_lst.find(fd);
// 			if (_iter != _client_lst.end())
// 				_client_lst.erase(_iter);
// 			FD_CLR(fd, &_read_set);
// 			close(fd);
// 		}
// 		else

// 			delete buffff;
		
// 		// memset(&buf, 0, sizeof(buf));
// 		//}
// 		if (size_recv == 0) {
// 			std::cout << std::endl << GREEN << "Connection lost... (fd=" << fd << ")" << RESET << std::endl;
// 			_iter = _client_lst.find(fd);
// 			if (_iter != _client_lst.end())
// 				_client_lst.erase(_iter);
// 			FD_CLR(fd, &_read_set);
// 			close(fd);
// 		}
// 	}
// }

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
	_err[201] = "Created";
	_err[202] = "Accepted";
	_err[203] = "Non-Authoritative Information";
	_err[204] = "No Content";
	_err[205] = "Reset Content";
	_err[206] = "Partial Content";
	_err[207] = "Multi-Status";
	_err[208] = "Already Reported";
	_err[226] = "IM Used";
	_err[300] = "Multiple Choices";
	_err[301] = "Moved Permanently";
	_err[302] = "Found";
	_err[303] = "See Other";
	_err[304] = "Not Modified";
	_err[305] = "Use Proxy";
	_err[306] = "Switch Proxy";
	_err[307] = "Temporary Redirect";
	_err[308] = "Permanent Redirect";
	_err[400] = "Bad Request";
	_err[401] = "Unauthorized";
	_err[402] = "Payment Required";
	_err[403] = "Forbidden";
	_err[404] = "Not Found";
	_err[405] = "Method Not Allowed";
	_err[406] = "Not Acceptable";
	_err[407] = "Proxy Authentication Required";
	_err[408] = "Request Timeout";
	_err[409] = "Conflict";
	_err[410] = "Gone";
	_err[411] = "Length Required";
	_err[412] = "Precondition Failed";
	_err[413] = "Payload Too Large";
	_err[414] = "URI Too Long";
	_err[415] = "Unsupported Media Type";
	_err[416] = "Range Not Satisfiable";
	_err[417] = "Expectation Failed";
	_err[418] = "I'm a teapot";
	_err[421] = "Misdirected Reques";
	_err[422] = "Unprocessable Entity";
	_err[423] = "Locked";
	_err[424] = "Failed Dependency";
	_err[425] = "Too Early";
	_err[426] = "Upgrade Required";
	_err[428] = "Precondition Required";
	_err[429] = "Too Many Requests";
	_err[431] = "Request Header Fields Too Large";
	_err[451] = "Unavailable For Legal Reasons";
	_err[500] = "Internal Server Error";
	_err[501] = "Not Implemented";
	_err[502] = "Bad Gateway";
	_err[503] = "Service Unavailable";
	_err[504] = "Gateway Timeout";
	_err[505] = "HTTP Version Not Supported";
	_err[506] = "Variant Also Negotiates";
	_err[507] = "Insufficient Storage";
	_err[508] = "Loop Detected";
	_err[510] = "Not Extended";
	_err[511] = "Network Authentication Required";
	return _err;
}

void	Server::send(int connection, const std::string s)
{
	::send(connection, s.c_str(), s.size(), 0);
	return ;
}
