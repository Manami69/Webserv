#ifndef SERVER_HPP
# define SERVER_HPP

# include "header.hpp"
# include "getRequest.hpp"
# include "getResponse.hpp"


struct Serv
{
	int					sockfd;
	unsigned short		port;
	std::string			host;
	sockaddr_in			addr;
};

class Server
{
private:
	Serv	*_serv_data;
	char	_buf[256];
	fd_set	_read_set;
	fd_set	_read_copy;
	std::map<int, std::string> _err;
	std::string _read_socket(int fd, ssize_t& bytesRecv);
	Server(void);

public:
	Server(int sockfd, std::string port, std::string host);
	~Server(void);
	void	binded(void);
	void	listened(void);
	void	selected(void);
	void	process_socket(int fd);
	std::map<int, std::string> error_code(void);
	void	send(int connection, const std::string s);

	enum RequestTerminator
	{
		CR = 13,
		LF = 10,	
	};
};

void	set_server_socket(int server, char **av);
void	run_server_socket(int sockfd, std::string port, std::string host);

#endif