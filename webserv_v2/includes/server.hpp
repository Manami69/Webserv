#ifndef SERVER_HPP
# define SERVER_HPP

# include "header.hpp"
# include "config.hpp"
# include "getRequest.hpp"
# include "getResponse.hpp"

# define BUFSIZE 1024

struct 					Listen
{
	int					sockfd;
	short				port;
	std::string			host;
	std::string			server_name;
	sockaddr_in			addr;
};

class Server
{
private:
	Listen							*_listen;
	std::vector<Listen*>			_server_lst;
	int								_server_size;
	fd_set							_read_set;
	int								_max_fd;
	std::map<int, int>				_client_lst;
	// std::map<int, int>::iterator	_iter;
	std::map<int, std::string>		_status_code;

public:
	Server( void );
	Server(Server const &copy);
	Server &operator=(Server const &rhs);
	~Server( void );
	void						setup_server(Config conf, int idx);
	void						selected(Config conf);
	void 						process_socket(Config conf, int fd);
	void						send(int connection, const std::string s);

	/* ------------------------------------------ GETTERS ------------------------------------------*/
	
	std::vector<Listen*>		get_server_lst(void) const;
	int							get_server_size(void) const;
	int							get_server_sockfd(int idx) const;
	short						get_server_port(int idx) const;
	std::string					get_server_host(int idx) const;
	std::string					get_servername(int idx) const;
	int							get_client_socket_size() const;
	int							get_max_fd(void) const;

	/* ------------------------------------------ UTILS ------------------------------------------*/
	int							is_sockfd_found(int fd);
	std::map<int, std::string>	status_code(void);
	bool						check_listen_duplicated(short port, std::string host);
	void						close_client(int fd);
};

#endif