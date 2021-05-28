#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include "all.hpp"

// typedef struct 				s_location //Config routes
// {
	//std::string				http_methods;
// }							t_location;

//	Struct parse .conf
typedef struct				s_config
{
	std::string				listen;
	std::string				server_name;
	std::string				root;
	std::string				default_error_page;
	size_t					body_size_limit;
	std::list<std::string>	index;
	// std::list<t_location>	locations;
}							t_config;

void					parse_config(std::string path, std::list<t_config> &config);
bool					check_line(std::string const &line, const std::string &comp);
bool					read_config(int fd, std::string *line);
std::string				get_config(std::string line, const std::string &after);
bool					read_location(int fd, std::string *line);
std::list<std::string>	get_index(std::string line, const std::string &after);

#endif