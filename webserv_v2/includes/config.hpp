#ifndef CONFIG_HPP
# define CONFIG_HPP

#include "header.hpp"

struct						Serv_config
{
	std::string					host;
	std::string					port;
	std::string					server_name;
	std::string					root;
	size_t						client_max_body_size;
	std::map<int,std::string>	error_page;
	std::list<std::string>		index;
	// std::list<t_location>	locations;
};

class Config
{
private:
	std::string					_filename;
	std::vector<std::string>	_tokens;
	unsigned	int				_nb_server;
	std::list<Serv_config>		_serv_config;
	Config( void );
	// Config(Config const &copy);
	// Config &operation=(Config const &rhs);

public:
	class FailedToOpenFile : public std::exception {
	public:
		const char *what() const throw(); // override
	};
	class WrongConfig : public std::exception {
	public:
		const char *what() const throw(); // override
	};
	class ErrorListen : public std::exception {
	public:
		const char *what() const throw(); // override
	};
	class ErrorServerName : public std::exception {
	public:
		const char *what() const throw(); // override
	};
	class ErrorRoot : public std::exception {
	public:
		const char *what() const throw(); // override
	};
	class ErrorClientMaxBodySize : public std::exception {
	public:
		const char *what() const throw(); // override
	};
	class ErrorPage : public std::exception {
	public:
		const char *what() const throw(); // override
	};
	Config( std::string filename );
	~Config( void );
	void						scan_file( void );
	void						tokenize( std::string line );
	void						check_brackets( void );
	void						init_serv_config(void);
	void						parse_config(void);
	bool						check_line(std::string const &line, std::string const &comp);
	bool						check_semicolon(unsigned long i);
	/////////		GETTERS		/////////
	std::string							get_filename( void ) const;
	std::vector<std::string>			get_tokens( void ) const;
	int									get_nb_server(void) const;
	std::list<Serv_config>::iterator	get_config( unsigned int idx );
	/////////		CHECKS		/////////
	void						check_listen(std::string conf);
	void						check_server_name(std::string conf);
	void						check_root(std::string conf);
	void						check_client_max_body_size(std::string conf);
	void						check_error_page(std::string conf1, std::string conf2);
};

#endif