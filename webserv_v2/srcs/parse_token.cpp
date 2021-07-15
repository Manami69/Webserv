#include "../includes/config.hpp"

Config::Config( std::string filename ) :
_filename( filename ),
_nb_server( 0 ) {
	return ;
}

Config::~Config( void ) {
	return ;
}

void	Config::scan_file( void ) {
	std::ifstream	ifs;
	std::string		line;

	ifs.open(get_filename().c_str());
	if ( !ifs.is_open() ) {
		ifs.close();
		throw ( ErrorMsg("Error : fail to open file " + get_filename() + ".") );
	}
	while ( !ifs.eof() ) {
		std::getline( ifs, line );
		this->tokenize( line );
	}
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

	/* Split semicolon, brackets, equal, tilde, carat tilde, tilde star */
	size_t found;
	for ( unsigned long i = 0; i < _tokens.size(); i++ ) 
	{
		if ((found = _tokens.at(i).find(";")) != std::string::npos && _tokens.at(i).size() != 1)
			this->split(found, i, ";", 1);
		if ((found = _tokens.at(i).find("{")) != std::string::npos && _tokens.at(i).size() != 1)
			this->split(found, i, "{", 1);
		if ((found = _tokens.at(i).find("}")) != std::string::npos && _tokens.at(i).size() != 1)
			this->split(found, i, "}", 1);
		if ((found = _tokens.at(i).find("=")) != std::string::npos && _tokens.at(i).size() != 1)
			this->split(found, i, "=", 1);
		if ((found = _tokens.at(i).find("~")) != std::string::npos && _tokens.at(i).size() != 1) {
			size_t next;
			if (found == 0 && (next = _tokens.at(i).find("/")) != std::string::npos && next == 1)
				this->split(found, i, "~", 1);
			if (found == 0 && (next = _tokens.at(i).find("*")) != std::string::npos && next == 1
			&& _tokens.at(i).size() != 2)
				this->split(found, i, "~*", 2);
			if (found == 1 && (next = _tokens.at(i).find("^")) != std::string::npos && next == 0
			&& _tokens.at(i).size() != 2)
				this->split(0, i, "^~", 2);
		}
	}
	return ;
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
		throw ( ErrorMsg("Error : configuration file has unpaired brackets.") );
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

	/* Check prefixe */
	for ( unsigned long i = 0; i < prefixe.size(); i++ ) {
		/* if there's nothing between location and open bracket */
		if ( i < prefixe.size() - 1 && !prefixe.at(i).compare("location")
			&& !prefixe.at(i + 1).compare("location"))
			throw	( ErrorMsg("Error : invalid location modifier " + prefixe.at(i) + ".") );
		if ( i == prefixe.size() - 1 && !prefixe.at(i).compare("location"))
			throw	( ErrorMsg("Error : invalid location modifier " + prefixe.at(i) + ".") );
		if ( !prefixe.at(i).compare("location") )
		{
			for ( ; prefixe.at(++i).compare("location") ;) {
				/* there's one token between location and open bracket, the token begins with a slash */
				if ( (i < prefixe.size() - 1 && !prefixe.at(i + 1).compare("location")) 
				|| (i == prefixe.size() - 1 && !prefixe.at(i - 1).compare("location")) ) {
					if ((found = prefixe.at(i).find("/")) == NOTFOUND
					|| ((found = prefixe.at(i).find("/")) != NOTFOUND && found > 0))
						throw	( ErrorMsg("Error : invalid location modifier " + prefixe.at(i) + ".") );
				}
				/* there's two tokens between location and open bracket, if the 1st token is = or ^~, the 2nd token begin with a slash */
				if ( (i < prefixe.size() - 2 && !prefixe.at(i + 2).compare("location")) 
				|| (i == prefixe.size() - 2 && !prefixe.at(i - 1).compare("location")) ) {
					if ( prefixe.at(i).size() == 1 && (found = prefixe.at(i).find("=")) != NOTFOUND) {
						if ((found = prefixe.at(i + 1).find("/")) == NOTFOUND 
						|| ((found = prefixe.at(i + 1).find("/")) != NOTFOUND && found > 0))
							throw	( ErrorMsg("Error : invalid location modifier " + prefixe.at(i) + ".") );
					}
					if ( prefixe.at(i).size() == 2 && (found = prefixe.at(i).find("^~")) != NOTFOUND) {
						if ((found = prefixe.at(i + 1).find("/")) == NOTFOUND 
						|| ((found = prefixe.at(i + 1).find("/")) != NOTFOUND && found > 0))
							throw	( ErrorMsg("Error : invalid location modifier " + prefixe.at(i) + ".") );
					}
				}
				break ;
			}
		}
	}
	
	/* Check prefixe "=", "~", "~*", "^~" */
	std::vector<std::string>	modifiers;
	modifiers.push_back ("=");
	modifiers.push_back ("~");
	modifiers.push_back ("~*");
	modifiers.push_back ("^~");

	for ( unsigned long i = 0; i < prefixe.size(); i++ )
	{
		if ( i > 0 && i < prefixe.size() - 2 && !prefixe.at(i - 1).compare("location")
		&& !prefixe.at(i + 2).compare("location") )
			if (std::find(modifiers.begin(), modifiers.end(), prefixe.at(i)) == modifiers.end())
				throw	( ErrorMsg("Error : invalid location modifier " + prefixe.at(i) + ".") );

		if	( i != 0 && i == prefixe.size() - 2 && !prefixe.at(i - 1).compare("location") )
			if (std::find(modifiers.begin(), modifiers.end(), prefixe.at(i)) == modifiers.end())
				throw	( ErrorMsg("Error : invalid location modifier " + prefixe.at(i) + ".") );
	}

	/* remove location */
	for ( unsigned long i = 0; i < prefixe.size(); i++ )
		if ( !prefixe.at(i).compare("location") )
			prefixe.erase (prefixe.begin() + i);

	/* check duplicated prefixe */
	for ( unsigned long i = 0; i < prefixe.size(); i++ ) {
		for ( unsigned long j = 1; j < prefixe.size(); j++ ) {
			if ( !prefixe.at(i).compare(prefixe.at(j)) && i != j
			&& std::find(modifiers.begin(), modifiers.end(), prefixe.at(i)) == modifiers.end()) 
			{
	 			if ( prefixe.at(j - 1).compare("~") && prefixe.at(j - 1).compare("~*") ) {
					if ( i != 0 && !prefixe.at(i - 1).compare("=") && !prefixe.at(j - 1).compare("="))
						throw	( ErrorMsg("Error : invalid location modifier " + prefixe.at(i) + ".") );
					else if ( i != 0 && !prefixe.at(i - 1).compare("^~") && !prefixe.at(j - 1).compare("^~"))
						throw	( ErrorMsg("Error : invalid location modifier " + prefixe.at(i) + ".") );
					else if ( i == 0 && std::find(modifiers.begin(), modifiers.end(), prefixe.at(j - 1)) == modifiers.end())
						throw	( ErrorMsg("Error : invalid location modifier " + prefixe.at(i) + ".") );
					else if ( i != 0 && std::find(modifiers.begin(), modifiers.end(), prefixe.at(i - 1)) == modifiers.end()
					&& std::find(modifiers.begin(), modifiers.end(), prefixe.at(j - 1)) == modifiers.end())
						throw	( ErrorMsg("Error : invalid location modifier " + prefixe.at(i) + ".") );
					else if ( i == 0 && !prefixe.at(j - 1).compare("^~"))
						throw	( ErrorMsg("Error : invalid location modifier " + prefixe.at(i) + ".") );
					else if ( i != 0 && std::find(modifiers.begin(), modifiers.end(), prefixe.at(i - 1)) == modifiers.end()
					&& !prefixe.at(j - 1).compare("^~"))
						throw	( ErrorMsg("Error : invalid location modifier " + prefixe.at(i) + ".") );
				}
			}
		}
	}
	modifiers.clear();
	prefixe.clear();
}

/* ///////////////////////////////// GETTERS ///////////////////////////////// */
std::string	Config::get_filename( void ) const {
	return ( _filename );
}

std::vector<std::string>	Config::get_tokens( void ) const {
	return ( _tokens );
}

int		Config::get_nb_server( void ) const {
	return ( this->_nb_server );
}

std::list<Serv_config>::iterator	Config::get_config( size_t idx ) {
	/*	if idx > nb of server, than it will get the last server in .config */
	if (idx > this->_nb_server)
		idx = this->_nb_server;
	std::list<Serv_config>::iterator it = this->_serv_config.begin();
	while (idx > 0) {
		idx--;
	 	it++;
	}
	return ( it );
};

std::list<_locations>::iterator		Config::get_location( std::list<Serv_config>::iterator it, unsigned int idx ) {
	if (idx > it->locations.size())
		return (*it).locations.end();
	std::list<_locations>::iterator its = (*it).locations.begin();
	while (idx > 0) {
		idx--;
	 	its++;
	}
	return ( its );
}

/* ///////////////////////////////// UTILS /////////////////////////////////// */
void	Config::split( size_t found, int i, std::string s, int len ) {
	std::string temp;

	temp = _tokens.at(i).substr(0, found);
	_tokens.insert(_tokens.begin() + i + 1, s);
	if ( _tokens.at(i).size() - 1 > found )
		_tokens.insert(_tokens.begin() + i + 2, _tokens.at(i).substr(found + len, _tokens.at(i).size() - found));
	_tokens.at(i) = temp;
}

bool	Config::is_number( const std::string& s ) {
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it))
		++it;
    return (!s.empty() && it == s.end());
}

bool 	Config::check_host( std::string host ) {
	int 		count = 0; 
	size_t		pos = 0;
	size_t		next = 0;
	std::string tmp;

	if (!host.compare("localhost"))
		return ( true );
	else {
		next = host.find(".");
		while (next != NOTFOUND) {
			count++;
			tmp = host.substr(pos, next - pos);
	 		if (!is_number(tmp) || std::atoi(tmp.c_str()) > 255)
	 			return ( false );
			pos = next + 1;
			next = host.find(".", next + 1);
		}
		if (count != 3)
			return ( false );
	}
	return ( true );
}

size_t Config::count_digit( std::string str ) {
	return std::count_if( str.begin(), str.end(), static_cast<int(*)(int)>(std::isdigit ));
}

const int	Config::error_code[] = { 100, 101, 102, 200 , 201 , 202 , 203 , 204 , 205 , 206 , 207 , 208 , 226 , 300 , 301\
, 302 , 303 , 304 , 305 , 307 , 308 , 400 , 401 , 402 , 403 , 404 , 405 , 406 , 407 , 408 , 409 , 410 , 411\
, 412 , 413 , 414 , 415 , 416 , 417 , 418 , 421 , 422 , 423 , 424 , 426 , 428 , 429 , 431 , 444 , 451 , 499\
, 500 , 501 , 502 , 503 , 504 , 505 , 506 , 507 , 508 , 510 , 511 , 599};