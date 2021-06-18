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

void	Config::check_error_page(std::string conf1, std::string conf2)
{
	(void)conf1;
	if (conf1.find(";") <= conf1.size())
		throw ( ErrorPage() );
	else if (conf2.find(";") > conf2.size())
		throw ( ErrorPage() );
	else if (!is_number(conf1))
		throw ( ErrorPage() );
}
