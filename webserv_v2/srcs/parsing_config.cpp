#include "../includes/config.hpp"

const char *Config::FailedToOpenFile::what() const throw() {
	return ( "Error : Impossible to open config file." );
}

const char *Config::WrongConfig::what() const throw() {
	return ( "Error : Wrong configuration." );
}

Config::Config( std::string filename ) :
_filename( filename ),
_nb_server( 0 ) {
	return ;
}

Config::~Config( void ) {
	return ;
}

std::string	Config::get_filename( void ) const {
	return ( _filename );
}

void	Config::scan_file( void ) {
	std::ifstream	ifs;
	std::string		line;

	ifs.open(get_filename().c_str());
	if ( !ifs.is_open() ) {
		ifs.close();
		throw ( FailedToOpenFile() );
	}
	while ( !ifs.eof() ) {
		std::getline( ifs, line );
		this->tokenize( line );
	}
	//////////////////////////////// delete later //////////////////////////////
	for ( unsigned long i = 0; i < _tokens.size(); i++ ) {
		std::cout << BLUE << "token : " << _tokens.at(i) << RESET << std::endl;
	}
	////////////////////////////////////////////////////////////////////////////
	ifs.close();
	this->check_brackets();
	this->check_location();
	return ;
}

void	Config::tokenize( std::string line ) {
	/* Replace all space into tab */
	std::replace( line.begin(), line.end(), ' ', '\t' );
	
	/* Split by tab */
	std::stringstream			str( line );
	std::string					intermediate;

	while ( std::getline(str, intermediate, '\t') ) {
		_tokens.push_back( intermediate );
	}

	/* Remove empty vector */
	for ( unsigned long i = 0; i < _tokens.size(); i++ ) {
		if ( _tokens.at(i).empty() ) {
			_tokens.erase( _tokens.begin() + i );
			i = 0;
		}
	}

	/* Seperate semicolon */
	size_t found;
	std::string temp;
	for ( unsigned long i = 0; i < _tokens.size(); i++ ) 
	{
		if ((found = _tokens.at(i).find(";")) != std::string::npos && _tokens.at(i).size() != 1) {
		 	temp = _tokens.at(i).substr(0, found);
		 	_tokens.insert(_tokens.begin() + i + 1, ";");
			if ( _tokens.at(i).size() - 1 > found )
				_tokens.insert(_tokens.begin() + i + 2, _tokens.at(i).substr(found + 1, _tokens.at(i).size() - found));
			_tokens.at(i) = temp;
			temp = "";
		}
	}

	/* Seperate open bracket */
	for ( unsigned long i = 0; i < _tokens.size(); i++ ) 
	{
		if ((found = _tokens.at(i).find("{")) != std::string::npos && _tokens.at(i).size() != 1) {
		 	temp = _tokens.at(i).substr(0, found);
		 	_tokens.insert(_tokens.begin() + i + 1, "{");
			if ( _tokens.at(i).size() - 1 > found )
				_tokens.insert(_tokens.begin() + i + 2, _tokens.at(i).substr(found + 1, _tokens.at(i).size() - found));
			_tokens.at(i) = temp;
			temp = "";
		}
	}

	/* Seperate close bracket */
	for ( unsigned long i = 0; i < _tokens.size(); i++ ) 
	{
		if ((found = _tokens.at(i).find("}")) != std::string::npos && _tokens.at(i).size() != 1) {
		 	temp = _tokens.at(i).substr(0, found);
		 	_tokens.insert(_tokens.begin() + i + 1, "}");
			if ( _tokens.at(i).size() - 1 > found )
				_tokens.insert(_tokens.begin() + i + 2, _tokens.at(i).substr(found + 1, _tokens.at(i).size() - found));
			_tokens.at(i) = temp;
			temp = "";
		}
	}
	return ;
}

std::vector<std::string>	Config::get_tokens( void ) const {
	return ( _tokens );
}

void	Config::check_brackets( void ) {
	std::vector<std::string>::iterator it;
	int open_bracket = 0;
	int	close_bracket = 0;

	for( it = _tokens.begin(); it != _tokens.end(); it++ ) {
		if ( *it == "{" )
			open_bracket += 1;
		else if ( *it == "}" )
			close_bracket += 1;
	}
	if (open_bracket != close_bracket)
		throw ( WrongConfig() );
}

void	Config::check_location( void ) {
	std::vector<std::string> prefixe;
	size_t found;

	/* Get prefixe */
	for ( unsigned long i = 0; i < _tokens.size(); i++ ) {
		if ( !_tokens.at(i).compare("location") ) {
			for ( ; _tokens.at(i).compare("{") ; i++ )
				prefixe.push_back(_tokens.at(i));
		}
	}

	/* Check if prefixe exists */
	for ( unsigned long i = 0; i < prefixe.size(); i++ ) {
		if ( i != prefixe.size() - 1 && !prefixe.at(i).compare("location")
			&& !prefixe.at(i + 1).compare("location"))
			throw ( WrongConfig() );
		if ( i == prefixe.size() - 1 && !prefixe.at(i).compare("location"))
			throw ( WrongConfig() );
		if ( !prefixe.at(i).compare("location") )
		{
			for ( ; prefixe.at(++i).compare("location") ;) {
				if ( i != prefixe.size() - 1 && (found = prefixe.at(i).find("/")) == NOTFOUND
				&& !prefixe.at(i + 1).compare("location") )
					throw ( WrongConfig() );
				if (i == prefixe.size() - 1 && (found = prefixe.at(i).find("/")) == NOTFOUND)
					throw ( WrongConfig() );
				if (i != prefixe.size() - 2 && (found = prefixe.at(i).find("/")) == NOTFOUND
				&& (found = prefixe.at(i + 1).find("/")) == NOTFOUND
				&& !prefixe.at(i + 2).compare("location"))
					throw ( WrongConfig() );
				if (i == prefixe.size() - 2 && (found = prefixe.at(i).find("/")) == NOTFOUND
				&& (found = prefixe.at(i + 1).find("/")) == NOTFOUND)
					throw ( WrongConfig() );
				break ;
			}
		}
	}

	/* Check if prefix begins with a slash */
	for ( unsigned long i = 0; i < prefixe.size(); i++ ) {
		if ( (found = prefixe.at(i).find("/")) != NOTFOUND && found > 0)
			throw ( WrongConfig() );
	}

	//////////////////////////////// delete later //////////////////////////////
	for ( unsigned long i = 0; i < prefixe.size(); i++ )
		std::cout << MAGENTA << "prefix : " << prefixe.at(i) << RESET << std::endl;
	////////////////////////////////////////////////////////////////////////////
	prefixe.clear();
}

void	Config::init_serv_config( void )
{
	Serv_config	serv_config;

	// serv_config.host = "0.0.0.0";
	// serv_config.port = "80";
	serv_config.server_name = "";
	serv_config.client_max_body_size = 1000000; //nginx default upload limit 1MB
	serv_config._nb_location = 0;
	_serv_config.push_back(serv_config);
	return ;
}

void	Config::parse_config(void)
{
	for (size_t i = 0; i < _tokens.size(); i++)
	{
		std::cout << GREEN << i << " " << _tokens.at(i) << RESET << std::endl;

		if (!_tokens.at(i).compare("server"))
		{
			_nb_server++;
			init_serv_config();
			if (_tokens.at(++i).compare("{"))
				throw ( WrongConfig() );
			while (++i < _tokens.size() && _tokens.at(i) != "}")
			{
				std::cout << i << " tokens " << _tokens.at(i) << std::endl;
				if (!_tokens.at(i).compare("listen"))
					i = set_listen(i);
				else if (!_tokens.at(i).compare("server_name"))
					i = set_server_name(i);
				else if (!_tokens.at(i).compare("client_max_body_size"))
					i = set_client_max_body_size(i);
				else if (this->_tokens.at(i) == "error_page")
					i = set_error_page(i);
				else if (this->_tokens.at(i) == "location")
					i = parse_location(i);
				else
					throw ( WrongConfig() );
			}
			//checker si host et port sont set et si ils ne le sont pas les mettre a default
		}
	}
}

bool	Config::is_number(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it))
		++it;
    return (!s.empty() && it == s.end());
}

int		Config::get_nb_server( void ) const {
	return (this->_nb_server);
}

std::list<Serv_config>::iterator	Config::get_config( unsigned int idx ) {
	/*	if idx > nb of server, than it will get the last server in .config */
	if (idx > this->_nb_server)
		idx = this->_nb_server;

	std::list<Serv_config>::iterator it = this->_serv_config.begin();
	while (--idx > 0)
		*it++;
	return (it);
};

std::vector<_locations>::iterator		Config::get_location( std::list<Serv_config>::iterator it, unsigned int idx )
{
	if (idx > it->locations.size())
		return it->locations.end();
	return it->locations.begin() + idx;
}
