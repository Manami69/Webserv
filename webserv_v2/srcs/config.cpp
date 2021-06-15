#include "../includes/config.hpp"

const char *Config::WrongFilename::what() const throw() {
	return ("Error : Wrong filename.");
}


Config::Config( std::string filename ) : _filename( filename ) {
	return ;
}

Config::~Config( void ) {
	return ;
}

std::string	Config::get_filename( void ) const {
	return (_filename);
}

void	Config::check_filename( void ) {
	if (_filename.empty()) {
		throw (WrongFilename());
	}
	return ;
}

void	Config::scan_file( void ) {
	std::ifstream	ifs;
	std::string		line;

	this->check_filename();
	ifs.open(get_filename());
	if ( !ifs.is_open() )
		throw (WrongFilename());
	while ( !ifs.eof() ) {
		std::getline( ifs, line );
		this->tokenize( line );
	}
	for (unsigned long i = 0; i < _tokens.size(); i++) {
		std::cout << "token : |" << _tokens[i] << "|" << std::endl;
	}
	std::cout << "token size: |" << _tokens.size() << "|" << std::endl;
	return ;
}

void	Config::tokenize( std::string line ) {
	/* Replace all space into tab */
	std::replace(line.begin(), line.end(), ' ', '\t');
	
	/* Split by tab */
	std::stringstream			str(line);
	std::string					intermediate;

	while (std::getline(str, intermediate, '\t')) {
		_tokens.push_back(intermediate);
	}

	/* Remove empty vector */
	for (unsigned long i = 0; i < _tokens.size(); i++) {
		if (_tokens[i].empty())
			_tokens.erase(_tokens.begin() + i);
	}
}

std::vector<std::string>	Config::get_tokens( void ) const {
	return (_tokens);
}

void	parsing(int ac, char **av)
{
	std::string	file;
	if (ac == 1)
		file = "config/webserv.conf";
	else
		file = av[1];
	Config	conf(file);
	conf.scan_file();
}
