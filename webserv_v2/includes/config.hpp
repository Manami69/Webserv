#ifndef CONFIG_HPP
# define CONFIG_HPP

#include "header.hpp"

struct						Serv_config
{
	std::string				host;
	std::string				port;
	std::string				server_name;
	std::string				root;
	std::string				default_error_page;
	size_t					body_size_limit;
	std::list<std::string>	index;
	// std::list<t_location>	locations;
};

class Config
{
private:
	std::string					_filename;
	std::vector<std::string>	_tokens;
	int							_nb_server;
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
	Config( std::string filename );
	~Config( void );
	std::string					get_filename( void ) const;
	void						scan_file( void );
	void						tokenize( std::string line );
	std::vector<std::string>	get_tokens( void ) const;
	void						check_brackets( void );
	void						init_serv_config(void);
	void						parse_config(void);
	bool						check_line(std::string const &line, std::string const &comp);
};

#endif