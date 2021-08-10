#include "../includes/config.hpp"

std::string	Config::get_filename( void ) const {
	return ( _filename );
}

std::vector<std::string>	Config::get_tokens( void ) const {
	return ( _tokens );
}

int		Config::get_config_size( void ) const {
	return (this->_nb_config);
}

std::list<Serv_config>::iterator	Config::get_config( size_t idx ) {
	/*	if idx > nb of server, than it will get the last server in .config */
	if (idx > this->_nb_config)
		idx = this->_nb_config;

	std::list<Serv_config>::iterator it = this->_serv_config.begin();
	while (idx > 0)
	{
		idx--;
	 	it++;
	}
	return (it);
};

Serv_config	Config::get_conf_by_name(std::string host, short port, std::string serv_name) {
	Serv_config best;
	Serv_config defau;
	size_t 		nbconf = 0;
	size_t		goals;
	size_t		found;
	for (std::list<Serv_config>::iterator it = _serv_config.begin(); it != _serv_config.end(); it++)
	{
		goals = 0;
		if (!it->host.compare("0.0.0.0") || !host.compare(it->host))
			goals++;
		if (port == atoi(it->port.c_str()))
			goals++;
		if (goals == 2 && nbconf == 0)
		{
			defau = *it;
			best = defau;
			nbconf++;
		}
		if ((found = it->server_name.find(serv_name)) != NOTFOUND)
		{
			// si found == 0 ou found - 1 == " "
			if (found == 0 || it->server_name[found - 1] == ' ')
			{
				// si found + size = total size ou si + 1 est un whitespace
				if (found + serv_name.size() > it->server_name.size())
					;
				if (found + serv_name.size() == it->server_name.size() || it->server_name[found + serv_name.size()] == ' ')
					goals++;
			}
		}
		if (goals == 3)
		{
			nbconf++;
			best = *it;
			return best;
		}
	}
	return best;
}


std::list<_locations>::iterator		Config::get_location( std::list<Serv_config>::iterator it, unsigned int idx )
{
	if (idx > it->locations.size())
		return (*it).locations.end();
	std::list<_locations>::iterator its = (*it).locations.begin();
	while (idx > 0)
	{
		idx--;
	 	its++;
	}
	return (its);
}