#include "../includes/server.hpp"

Server::Server( void ) : 
_server_size(0), _max_fd(0) {
	FD_ZERO(&_read_set);
	return ;
}

Server::Server(Server const &copy) {
	*this = copy;
	return ;
}

Server &Server::operator=(Server const &rhs) {
	if (this != &rhs) {
		this->_listen = rhs._listen;
		this->_server_lst = rhs._server_lst;
		this->_server_size = rhs._server_size;
		this->_read_set = rhs._read_set;
		this->_max_fd = rhs._max_fd;
		this->_client_lst = rhs._client_lst;
		this->_status_code = rhs._status_code;
	}
	return (*this);
}

Server::~Server( void ) {
	for (std::vector<Listen*>::iterator it = _server_lst.begin(); it != _server_lst.end(); ++it) {
		if (*it) {
			close((*it)->sockfd);
			FD_CLR((*it)->sockfd, &_read_set);
			delete *it;
			*it = NULL;
		}
	}
	return ;
}

void	Server::setup_server(Config conf, int idx) {

	int	ret;

	/* ------------------------------ TCP, IPv4 ------------------------------ */
	in_addr_t address = inet_addr(conf.get_config(idx)->host.c_str());
	if (address == INADDR_NONE)
		throw ( ErrorMsg ("Bad ip address. <" + std::string(strerror(errno)) + ">"));
	
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		close(sockfd);
		throw ( ErrorMsg ("Failed to create socket. <" + std::string(strerror(errno)) + ">"));
	}

	/* --------------- set socket to allow multiple connections -------------- */
	int opt = true;
	ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char *>(&opt), sizeof(opt));
	if (ret == -1) {
		close(sockfd);
		throw ( ErrorMsg ("Failed to set socket reuse. <" + std::string(strerror(errno)) + ">"));
	}

	/* -------------------------- Check before bind -------------------------- */
	if (this->get_server_size() > 0) {
		if (check_listen_duplicated(atoi(conf.get_config(idx)->port.c_str()), conf.get_config(idx)->host)) {
			close(sockfd);
			return ;
		}
	}

	/* --------------------------------- Bind -------------------------------- */
	_listen = new Listen();
	_listen->sockfd = sockfd;
	_listen->port = atoi(conf.get_config(idx)->port.c_str());
	_listen->host = conf.get_config(idx)->host;
	_listen->server_name = conf.get_config(idx)->server_name;
	
	memset(&_listen->addr, 0, sizeof(sockaddr_in));
	_listen->addr.sin_family = AF_INET;
	_listen->addr.sin_addr.s_addr = address;
	_listen->addr.sin_port = htons(_listen->port);
	ret = bind(_listen->sockfd, (const struct sockaddr*)&_listen->addr, sizeof(_listen->addr));
	if (ret == -1)
		throw ( ErrorMsg ("Failed to bind to port " + conf.get_config(idx)->port + "<" + std::string(strerror(errno)) + ">"));

	/* -------------------------------- Listen ------------------------------- */
	ret = listen(_listen->sockfd, 128);
	if (ret == -1)
		throw ( ErrorMsg ("Failed to listen on socket. <" + std::string(strerror(errno)) + ">"));
	_server_lst.push_back(_listen);
	if (_listen->sockfd > this->get_max_fd())
		_max_fd = _listen->sockfd;
	return ;
}

void	Server::selected(Config conf) {
	
	for (std::vector<Listen*>::iterator it = _server_lst.begin(); it != _server_lst.end(); ++it) {
		FD_SET((*it)->sockfd, &_read_set);
	}
	while (true) {
		fd_set	_read_copy = _read_set;
		int ret = select((_max_fd + 1), &_read_copy, 0, 0, 0);
		if ((ret == -1) && (errno != EINTR))
			throw std::runtime_error ("An error occurred with select. <" + std::string(strerror(errno)) + ">");
		for (int fd = 0; fd <= this->get_max_fd(); ++fd) {
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
			_client_lst.insert(std::pair<int, int>(comm, server_order));
			FD_SET(comm , &_read_set);
			if (comm > this->get_max_fd())
				_max_fd = comm;
		}
    }
	else {
		ssize_t size_recv = 0;
		ssize_t body_size = 0;
		size_t 	ret;
		char	buf[BUFSIZE + 1];
		std::string *save_buf = new std::string;
		fcntl(fd, F_SETFL, O_NONBLOCK);
		FD_SET(fd, &_read_set);
		int empty = 0;
		while(true)
		{
			if (size_recv == 0)
				empty++;
			else
				empty = 0;
			if (empty == 10000)
			{
				delete save_buf;
				std::cout << std::endl << GREEN << "Connection lost... (fd=" << fd << ")" << RESET << std::endl;
				this->close_client(fd);
				return ;
			}
			memset(&buf, 0, sizeof(buf));
			size_recv = recv(fd, buf, sizeof(buf), 0); // check return value
			for (ssize_t j = 0; j < size_recv ; j++)
				save_buf->push_back(buf[j]);
			if ((ret = save_buf->find("\r\n\r\n")) != std::string::npos)
				break ;
		}
		getRequest req(*save_buf);
		while (true) {
			memset(&buf, 0, sizeof(buf));
			size_recv = recv(fd, buf, sizeof(buf), 0);
			body_size += size_recv;
			for (ssize_t j = 0; j < size_recv ; j++)
				save_buf->push_back(buf[j]);
			if (save_buf->size() == std::atoi(req.getKeyValue("Content-Length").c_str()) + ret + 4)
				break ;
		}
		req.fill_body(*save_buf);
		getResponse response(req, conf.get_conf_by_name(get_server_host(_client_lst[fd] - 1), get_server_port(_client_lst[fd] - 1), req["Host"]));
		this->status_code();
		send(fd, response.responsetosend(_status_code));
		delete save_buf;
		std::cout << std::endl << GREEN << "Connection lost... (fd=" << fd << ")" << RESET << std::endl;
		this->close_client(fd);
	}
}

void	Server::send(int connection, const std::string s)
{
	int ret;
	if ((ret = ::send(connection, s.c_str(), s.size(), 0)) < 0)
		throw ( ErrorMsg ("sent failed. <" + std::string(strerror(errno))));
	return ;
}


/* ------------------------------------------ GETTERS ------------------------------------------*/


std::vector<Listen*>	Server::get_server_lst(void) const {
	return (_server_lst);
}

int		Server::get_server_size(void) const {
	return (this->get_server_lst().size());	
}

int		Server::get_server_sockfd(int idx) const {
	return (get_server_lst().at(idx)->sockfd);
}

short	Server::get_server_port(int idx) const {
	return (get_server_lst().at(idx)->port);
}

std::string	Server::get_server_host(int idx) const {
	return (get_server_lst().at(idx)->host);
}

std::string	Server::get_servername(int idx) const {
	return (get_server_lst().at(idx)->server_name);
}

int		Server::get_client_socket_size() const {
	return (_client_lst.size());
}

int		Server::get_max_fd(void) const {
	return (_max_fd);
}


/* ----------------------------------------- UTILS --------------------------------------------- */


int		Server::is_sockfd_found(int fd) {
	int i = 1;
	std::vector<Listen*>::iterator it;
	for (it = _server_lst.begin(); it != _server_lst.end(); ++it) {
		if (fd == (*it)->sockfd)
			return (i);
		i++;
	}
	return (false);
}

std::map<int, std::string> Server::status_code(void) {
	_status_code[200] = "OK";
	_status_code[201] = "Created";
	_status_code[202] = "Accepted";
	_status_code[203] = "Non-Authoritative Information";
	_status_code[204] = "No Content";
	_status_code[205] = "Reset Content";
	_status_code[206] = "Partial Content";
	_status_code[207] = "Multi-Status";
	_status_code[208] = "Already Reported";
	_status_code[226] = "IM Used";
	_status_code[300] = "Multiple Choices";
	_status_code[301] = "Moved Permanently";
	_status_code[302] = "Found";
	_status_code[303] = "See Other";
	_status_code[304] = "Not Modified";
	_status_code[305] = "Use Proxy";
	_status_code[306] = "Switch Proxy";
	_status_code[307] = "Temporary Redirect";
	_status_code[308] = "Permanent Redirect";
	_status_code[400] = "Bad Request";
	_status_code[401] = "Unauthorized";
	_status_code[402] = "Payment Required";
	_status_code[403] = "Forbidden";
	_status_code[404] = "Not Found";
	_status_code[405] = "Method Not Allowed";
	_status_code[406] = "Not Acceptable";
	_status_code[407] = "Proxy Authentication Required";
	_status_code[408] = "Request Timeout";
	_status_code[409] = "Conflict";
	_status_code[410] = "Gone";
	_status_code[411] = "Length Required";
	_status_code[412] = "Precondition Failed";
	_status_code[413] = "Payload Too Large";
	_status_code[414] = "URI Too Long";
	_status_code[415] = "Unsupported Media Type";
	_status_code[416] = "Range Not Satisfiable";
	_status_code[417] = "Expectation Failed";
	_status_code[418] = "I'm a teapot";
	_status_code[421] = "Misdirected Reques";
	_status_code[422] = "Unprocessable Entity";
	_status_code[423] = "Locked";
	_status_code[424] = "Failed Dependency";
	_status_code[425] = "Too Early";
	_status_code[426] = "Upgrade Required";
	_status_code[428] = "Precondition Required";
	_status_code[429] = "Too Many Requests";
	_status_code[431] = "Request Header Fields Too Large";
	_status_code[451] = "Unavailable For Legal Reasons";
	_status_code[500] = "Internal Server Error";
	_status_code[501] = "Not Implemented";
	_status_code[502] = "Bad Gateway";
	_status_code[503] = "Service Unavailable";
	_status_code[504] = "Gateway Timeout";
	_status_code[505] = "HTTP Version Not Supported";
	_status_code[506] = "Variant Also Negotiates";
	_status_code[507] = "Insufficient Storage";
	_status_code[508] = "Loop Detected";
	_status_code[510] = "Not Extended";
	_status_code[511] = "Network Authentication Required";
	return _status_code;
}

bool	Server::check_listen_duplicated( short port, std::string host ) {
	for (int i = 0; i < this->get_server_size(); i++) {
		if (get_server_lst().at(i)->port == port && !get_server_lst().at(i)->host.compare(host))
			return (true);
	}
	return (false);
}

void	Server::close_client(int fd) {
	std::map<int, int>::iterator	_iter;
	
	_iter = _client_lst.find(fd);
	if (_iter != _client_lst.end()) {
		FD_CLR((*_iter).first, &_read_set);
		_client_lst.erase(_iter);
	}
	close(fd);
}