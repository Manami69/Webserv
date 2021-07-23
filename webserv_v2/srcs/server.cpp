#include "../includes/server.hpp"

Server::Server( void ) : 
_server_size(0), _max_fd(0) {
	FD_ZERO(&_read_set);
    FD_ZERO(&_read_copy);
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
		this->_read_copy = rhs._read_copy;
		this->_max_fd = rhs._max_fd;
		this->_client_lst = rhs._client_lst;
		this->_iter = rhs._iter;
		this->_err = rhs._err;
	}
	return (*this);
}

Server::~Server( void ) { //close fd and delete
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
	if (_listen->sockfd > _max_fd)
		_max_fd = _listen->sockfd;
	return ;
}

bool	Server::check_listen_duplicated( short port, std::string host ) {
	for (int i = 0; i < this->get_server_size(); i++) {
		if (get_server_lst().at(i)->port == port && !get_server_lst().at(i)->host.compare(host))
			return (true);
	}
	return (false);
}

void	Server::selected(Config conf) {
			
	for (std::vector<Listen*>::iterator it = _server_lst.begin(); it != _server_lst.end(); ++it) {
		FD_SET((*it)->sockfd, &_read_set);
	}
	
	while (true) {
		_read_copy = _read_set;
		int ret = select((_max_fd + 1), &_read_copy, 0, 0, 0);
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
			_client_lst.insert(std::pair<int, int>(comm, server_order));
			FD_SET(comm , &_read_set);
			if (comm > _max_fd)
				_max_fd = comm;
		}
    }
	else {
		ssize_t size_recv, body_size = 0;
		size_t ret;
		char	buf[BUFSIZE + 1];
		std::string *save_buf = new std::string;
		fcntl(fd, F_SETFL, O_NONBLOCK);
		FD_SET(fd, &_read_set);
		while(true)
		{
			memset(&buf, 0, sizeof(buf));
			size_recv = recv(fd, buf, sizeof(buf), 0); // check return value
			for (ssize_t j = 0; j < size_recv ; j++)
				save_buf->push_back(buf[j]);
			if ((ret = save_buf->find("\r\n\r\n")) != std::string::npos)
				break ;
		}
		getRequest req(*save_buf);
		while (true) {
			std::cout << *save_buf << std::endl;
			memset(&buf, 0, sizeof(buf));
			size_recv = recv(fd, buf, sizeof(buf), 0);
			body_size += size_recv;
			for (ssize_t j = 0; j < size_recv ; j++)
				save_buf->push_back(buf[j]);
			if (save_buf->size() == std::atoi(req.getKeyValue("Content-Length").c_str()) + ret + 4)
				break ;
		}
		req.fill_body(*save_buf);
		//std::cout << *save_buf << std::endl;

		getResponse response(req, conf.get_conf_by_name(get_server_host(_client_lst[fd] - 1), get_server_port(_client_lst[fd] - 1), req["Host"]));
		this->error_code();
		send(fd, response.responsetosend(_err)); // check return value
		delete save_buf;
		std::cout << std::endl << GREEN << "Connection lost... (fd=" << fd << ")" << RESET << std::endl;
		_iter = _client_lst.find(fd);
		if (_iter != _client_lst.end())
			_client_lst.erase(_iter);
		FD_CLR(fd, &_read_set);
		close(fd);
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
