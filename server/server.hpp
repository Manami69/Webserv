#ifndef SERVER_HPP
# define SERVER_HPP

# include "header.hpp"

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
	int						_sockfd;
	short					_port;
	Listen					*_listen;
	std::vector<Listen*>	_server_lst;
	int						_server_nbr;
	Server(Server const &copy);
	Server &operator=(Server const &rhs);

public:
	Server(void);
	~Server(void);
	void	setup_server_socket(std::string const &port);
	void	get_master_socket_fd(void);
	void	set_socket_reuse(void);
	void	binded(void);
	void	listened(void);
	void	add_server_lst(void);
	int		get_server_nbr(void) const;
};

#endif