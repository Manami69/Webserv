#include "../includes/config.hpp"

const char *Config::ErrorListen::what() const throw() {
	return ( "Error : Wrong listen." );
}

const char *Config::ErrorServerName::what() const throw() {
	return ( "Error : Wrong server name." );
}

const char *Config::ErrorRoot::what() const throw() {
	return ( "Error : Wrong root." );
}

const char *Config::ErrorClientMaxBodySize::what() const throw() {
	return ( "Error : Wrong client max body size." );
}

const char *Config::ErrorPage::what() const throw() {
	return ( "Error : Wrong error page." );
}

const char *Config::ErrorAccess::what() const throw() {
	return ( "Error : Wrong access to location." );
}

const char *Config::ErrorMethods::what() const throw() {
	return ( "Error : Wrong methods to location." );
}

bool	is_number(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

void	Config::check_listen(std::string conf)
{
	std::string		host;
	std::string		port;
	std::string		tmp;
	int				i = 0;
	int				check = 0;
	int				find = conf.find(":");

	if (find == -1 || conf.find(";") > conf.size())
		throw ( ErrorListen() );
	host = conf.substr(0, find);
	port = conf.substr(find + 1, conf.size() - (find + 2));
	if (host != "localhost")
	{
		while (host.find(".") < host.size())
		{
			check++;
			i = host.find(".");
			tmp = host.substr(0, i);
			if (!is_number(tmp) || std::atoi(tmp.c_str()) > 255)
				throw ( ErrorListen() );
			host = host.substr(i + 1, host.size());
		}
		if (check != 3)
			throw ( ErrorListen() );
		if (!is_number(host) || std::atoi(host.c_str()) > 255)
				throw ( ErrorListen() );
	}
	if (!is_number(port) || std::atoi(port.c_str()) > 65535)
		throw ( ErrorListen() );
}

void	Config::check_server_name(std::string conf)
{
	if (conf.find(";") > conf.size())
		throw ( ErrorServerName() );
}

void	Config::check_root(std::string conf)
{
	if (conf.find(";") > conf.size())
		throw ( ErrorRoot() );
}

void	Config::check_client_max_body_size(std::string conf)
{
	std::string		tmp;

	if (conf.find(";") > conf.size())
		throw ( ErrorClientMaxBodySize() );
	tmp = conf.substr(0, conf.size() - 1);
	if (!is_number(tmp) || std::atoi(tmp.c_str()) > 100 || std::atoi(tmp.c_str()) == 0)
		throw ( ErrorClientMaxBodySize() );
}

void	Config::check_error_page(size_t i)
{
	if (!is_number(_tokens[i]))
		throw ( ErrorPage() );
	while (is_number(_tokens[i]))
		i++;
	if (_tokens[i].find(";") == std::string::npos)
		throw ( ErrorPage() );
}

void	Config::check_allow_methods(std::string conf)
{
	if (conf.find(";") > conf.size())
		throw ( ErrorMethods() );
	else if (!check_line(conf, "GET;") && !check_line(conf, "PUT;") && !check_line(conf, "POST;"))
		throw ( ErrorMethods() );
}
