#ifndef CONFIG_HPP
# define CONFIG_HPP

#include "header.hpp"
#include "error_msg.hpp"

struct										_locations
{
	std::string								modifier;
	std::string								access;
	bool									allowm;
	bool									limitm;
	int										allow_methods[3];
	std::string								root;
	std::string								index;
	bool									autoindex;
	std::string								cgi_path;
	std::map<std::string, std::string>		redirect;
};


struct										Serv_config
{
	std::string								host;
	std::string								port;
	std::string								server_name;
	size_t									client_max_body_size;
	std::map<int,std::string>				error_page;
	unsigned	int							_nb_location;
	std::list<_locations>					locations;
};

class Config
{
private:
	std::string								_filename;
	std::vector<std::string>				_tokens;
	unsigned	int							_nb_config;
	std::list<Serv_config>					_serv_config;
	Config( void );

public:
	Config( std::string filename );
	Config( Config const &copy );
	Config &operator=(Config const &rhs);
	~Config( void );
	void								scan( void );
	void								tokenize( std::string line );
	void								check_brackets( void );
	size_t								check_prefixe( size_t idx );
	void								InitConfig( void );
	void								InitLocation( void );
	void								parse_config( void );
	size_t								parse_location( size_t i );

	/* ------------------------------------------- UTILS -------------------------------------------*/
	void								split( size_t found, int i, std::string s, int len );
	bool								is_number( const std::string& s );
	bool 								check_host( std::string host );
	size_t								count_digit( std::string str );
	size_t								found_idx( std::string, size_t i);
	static const int					error_code[];

	/* ------------------------------------------ GETTERS ------------------------------------------*/
	std::string							get_filename( void ) const;
	std::vector<std::string>			get_tokens( void ) const;
	int									get_config_size( void ) const;
	std::list<Serv_config>::iterator	get_config( size_t idx );
	Serv_config							get_conf_by_name(std::string host, short port, std::string serv_name);
	std::list<_locations>::iterator		get_location( std::list<Serv_config>::iterator it, unsigned int idx );
	
	/* ------------------------------------------ SETTERS ------------------------------------------*/
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
};

#endif