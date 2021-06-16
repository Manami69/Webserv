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

	//this->check_filename();
	ifs.open(get_filename());
	if ( !ifs.is_open() ) {
		ifs.close();
		throw ( FailedToOpenFile() );
	}
	while ( !ifs.eof() ) {
		std::getline( ifs, line );
		this->tokenize( line );
	}
	ifs.close();
	////////////////////////////////////////////////////////////////////////////
	for ( unsigned long i = 0; i < _tokens.size(); i++ ) {
		std::cout << YELLOW << "token : |" << _tokens[i] << "|" << RESET << std::endl;
	}
	////////////////////////////////////////////////////////////////////////////
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
		if ( _tokens[i].empty() )
			_tokens.erase( _tokens.begin() + i );
	}
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

void	Config::init_serv_config( void )
{
	Serv_config	serv_config;

	serv_config.host = "0.0.0.0";
	serv_config.port = "80";
	serv_config.server_name = "Default";
	serv_config.root = "";
	serv_config.default_error_page = "";
	serv_config.body_size_limit = 1;
	_serv_config.push_back(serv_config);
	return ;
}

void	Config::parse_config(void)
{
	for (unsigned long i = 0; i < this->_tokens.size(); i++)
	{
		if (check_line(this->_tokens[i], "server"))
		{
			this->_nb_server++;
			this->init_serv_config();
			while (i < this->_tokens.size() && this->_tokens[i] != "}")
			{
				if (check_line(this->_tokens[i], "listen"))
					this->_serv_config.back().host = this->_tokens[++i];
				else if (check_line(this->_tokens[i], "server_name"))
					this->_serv_config.back().server_name = this->_tokens[++i];
				else if (check_line(this->_tokens[i], "root"))
					this->_serv_config.back().root = this->_tokens[++i];
				else if (check_line(this->_tokens[i], "body_size_limit"))
					this->_serv_config.back().body_size_limit = std::atoi(this->_tokens[++i].c_str());
				else if (check_line(this->_tokens[i], "default_error_page"))
					this->_serv_config.back().default_error_page = this->_tokens[++i];
				else if (check_line(this->_tokens[i], "location"))
					while (i < this->_tokens.size() && this->_tokens[i] != "}")
						i++;
				i++;
			}
		}
	}
	std::cout << BLUE << this->_serv_config.back().host << RESET << std::endl;
	std::cout << BLUE << this->_serv_config.back().port << RESET << std::endl;
	std::cout << BLUE << this->_serv_config.back().server_name << RESET << std::endl;
	std::cout << BLUE << this->_serv_config.back().root << RESET << std::endl;
	std::cout << BLUE << this->_serv_config.back().default_error_page << RESET << std::endl;
	std::cout << BLUE << this->_serv_config.back().body_size_limit << RESET << std::endl;
}

bool	Config::check_line(std::string const &line, std::string const &comp)
{
	unsigned int	start = 0;
	unsigned int	end = comp.size();

	if (line.size() >= start && line.substr(start, end) == comp)
		return true;
	return false;
}

int		Config::get_nb_server(void)
{
	return (this->_nb_server);
}

std::list<Serv_config>::iterator	Config::get_config(int i)
{
	if (i > this->_nb_server)
		i = this->_nb_server;
	std::list<Serv_config>::iterator it = this->_serv_config.begin();
	while (--i > 0)
		*it++;
	return (it);
};
