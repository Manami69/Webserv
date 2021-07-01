#ifndef CONFIG_HPP
# define CONFIG_HPP

#include "header.hpp"

struct						_locations
{
	std::string					modifier;
	std::string					access;
	bool						allowm;
	bool						limitm;
	int							allow_methods[3];
	std::string					root;
	std::string					index;
	bool						autoindex;
	std::string					cgi_path;
	std::string					try_files;
	std::map<std::string, std::string>	redirect;
};


struct						Serv_config
{
	std::string					host;
	std::string					port;
	std::string					server_name;
	size_t						client_max_body_size;
	std::map<int,std::string>	error_page;
	unsigned	int				_nb_location;
	std::list<_locations>		locations;
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
	class ErrorAccess : public std::exception {
	public:
		const char *what() const throw(); // override
	};
	class ErrorMethods : public std::exception {
	public:
		const char *what() const throw(); // override
	};
	class ErrorIndex : public std::exception {
	public:
		const char *what() const throw(); // override
	};
	class ErrorAutoindex : public std::exception {
	public:
		const char *what() const throw(); // override
	};
	class ErrorReturn : public std::exception {
	public:
		const char *what() const throw(); // override
	};
	class ErrorCgiPath : public std::exception {
	public:
		const char *what() const throw(); // override
	};
	class ErrorTryFiles : public std::exception {
	public:
		const char *what() const throw(); // override
	};
	Config( std::string filename );
	~Config( void );
	void								scan_file( void );
	void								tokenize( std::string line );
	void								check_brackets( void );
	void								init_serv_config( void );
	void								init_config_location( void );
	void								parse_config( void );
	bool								is_number(const std::string& s);
	bool 								check_host(std::string host);
	size_t								parse_location( size_t i );
	//////////////////		GETTERS		//////////////////
	std::string							get_filename( void ) const;
	std::vector<std::string>			get_tokens( void ) const;
	int									get_nb_server( void ) const;
	std::list<Serv_config>::iterator	get_config( unsigned int idx );
	std::list<_locations>::iterator		get_location( std::list<Serv_config>::iterator it, unsigned int idx );
	//////////////////		SETTERS		//////////////////
	size_t								set_listen( size_t i );
	size_t								set_server_name( size_t i );
	size_t								set_client_max_body_size( size_t i );
	size_t								set_error_page( size_t i );
	size_t								set_allow_methods( size_t i, bool titre );
	size_t								set_root( size_t i );
	size_t								set_index( size_t i );
	size_t								set_autoindex( size_t i );
	size_t								set_return( size_t i );
	size_t								set_cgi_path( size_t i );
	size_t								set_try_files( size_t i );

	static const int					error_code[];
};

#endif