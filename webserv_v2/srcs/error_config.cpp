#include "../includes/config.hpp"

const char *Config::ErrorListen::what() const throw() {
	return ( "Error : Wrong listen." );
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

	if (find == -1)
		throw ( ErrorListen() );
	host = conf.substr(0, find);
	port = conf.substr(find + 1, conf.size() - (find + 1));
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
