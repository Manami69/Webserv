#ifndef SERVER_HPP
# define SERVER_HPP

# include "header.hpp"
# include "getRequest.hpp"
# include "getResponse.hpp"

# define BUFSIZE 2048
struct 					Listen
{
	int					sockfd;
	short				port;
	std::string			host;
	sockaddr_in			addr;
};

class Server
{
private:
	int							_sockfd;
	short						_port;
	Listen						*_listen;
	std::vector<Listen*>		_server_lst;
	int							_server_nbr;
	fd_set						_read_set;
	fd_set						_read_copy;
	int							_max_fd;
	std::vector<int>			_client_lst;
	std::vector<int>::iterator	_iter;
	Server(Server const &copy);
	Server &operator=(Server const &rhs);
	std::map<int, std::string>	_err;
	//std::string 				_read_socket(int fd, ssize_t& bytesRecv);

public:
	Server(void);
	~Server(void);
	void					setup_server_socket(std::string const &port);
	void					get_master_socket_fd(void);
	void					set_socket_reuse(void);
	void					binded(void);
	void					listened(void);
	void					add_server_lst(void);
	int						get_server_nbr(void) const;
	std::vector<Listen*>	get_server_lst(void) const;
	void					selected(void);
	void 					process_socket(int fd);
	int						is_sockfd_found(int fd);
	int						get_client_socket_size() const;
	std::map<int, std::string> 	error_code(void);
	void					send(int connection, const std::string s);

	enum RequestTerminator
	{
		CR = 13,
		LF = 10,	
	};
};

#endif