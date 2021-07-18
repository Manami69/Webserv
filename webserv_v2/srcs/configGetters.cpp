#include "../includes/config.hpp"

std::string	Config::get_filename( void ) const {
	return ( _filename );
}

std::vector<std::string>	Config::get_tokens( void ) const {
	return ( _tokens );
}

int		Config::get_nb_server( void ) const {
	return (this->_nb_server);
}

std::list<Serv_config>::iterator	Config::get_config( size_t idx ) {
	/*	if idx > nb of server, than it will get the last server in .config */
	if (idx > this->_nb_server)
		idx = this->_nb_server;

	std::list<Serv_config>::iterator it = this->_serv_config.begin();
	while (idx > 0)
	{
		idx--;
	 	it++;
	}
	return (it);
};

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