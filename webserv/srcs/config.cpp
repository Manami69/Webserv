#include "../includes/config.hpp"

Config::Config( std::string filename ) :
_filename( filename ), _nb_config( 0 ) {
	return ;
}

Config::Config( Config const &copy ) {
	*this = copy;
	return ;
}

Config	&Config::operator=(Config const &rhs) {
	if (this != &rhs) {
		this->_filename = rhs._filename;
		this->_tokens = rhs._tokens;
		this->_nb_config = rhs._nb_config;
		this->_serv_config = rhs._serv_config;
	}
	return (*this);
}

Config::~Config( void ) {
	return ;
}

void	Config::scan( void ) {
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

	for ( unsigned long i = 0; i < _tokens.size(); )
		if (!_tokens.at(i).compare("server"))
			i = this->check_prefixe(++i);
	this->parse_config();
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
			if (found == 0 && (next = _tokens.at(i).find("*")) != std::string::npos && next == 1
			&& _tokens.at(i).size() != 2)
				this->split(found, i, "~*", 2);
			else if (found == 1 && (next = _tokens.at(i).find("^")) != std::string::npos && next == 0
			&& _tokens.at(i).size() != 2)
				this->split(0, i, "^~", 2);
			else if (_tokens.at(i).size() != 2)
				this->split(found, i, "~", 1);
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

size_t	Config::check_prefixe( size_t idx ) {

	std::vector<std::string> prefixe;
	size_t found;
	size_t next_server = this->found_idx("server", idx);
	next_server = (next_server > 0 ) ? next_server : _tokens.size();

	/* Get prefixe */
	for ( ; idx < next_server; idx++ ) {
		if ( !_tokens.at(idx).compare("location") ) {
			for ( ; _tokens.at(idx).compare("{") ; idx++ )
				prefixe.push_back(_tokens.at(idx));
		}
	}

	/* Check prefixe */
	for ( unsigned long i = 0; i < prefixe.size(); i++ ) {
		/* if there's nothing between location and open bracket */
		if ( i < prefixe.size() - 1 && !prefixe.at(i).compare("location")
			&& !prefixe.at(i + 1).compare("location"))
			throw ( ErrorMsg("Error : invalid location modifier " + prefixe.at(i) + ".") );
		if ( i == prefixe.size() - 1 && !prefixe.at(i).compare("location"))
			throw ( ErrorMsg("Error : invalid location modifier " + prefixe.at(i) + ".") );
		if ( !prefixe.at(i).compare("location") )
		{
			for ( ; prefixe.at(++i).compare("location") ;) {
				/* there's one token between location and open bracket, the token begins with a slash */
				if ( (i < prefixe.size() - 1 && !prefixe.at(i + 1).compare("location")) 
				|| (i == prefixe.size() - 1 && !prefixe.at(i - 1).compare("location")) ) {
					if ((found = prefixe.at(i).find("/")) == NOTFOUND
					|| ((found = prefixe.at(i).find("/")) != NOTFOUND && found > 0))
						throw ( ErrorMsg("Error : invalid location modifier " + prefixe.at(i) + ".") );
				}
				/* there's two tokens between location and open bracket, if the 1st token is = or ^~, the 2nd token begin with a slash */
				if ( (i < prefixe.size() - 2 && !prefixe.at(i + 2).compare("location")) 
				|| (i == prefixe.size() - 2 && !prefixe.at(i - 1).compare("location")) ) {
					if ( prefixe.at(i).size() == 1 && (found = prefixe.at(i).find("=")) != NOTFOUND) {
						if ((found = prefixe.at(i + 1).find("/")) == NOTFOUND 
						|| ((found = prefixe.at(i + 1).find("/")) != NOTFOUND && found > 0))
							throw ( ErrorMsg("Error : invalid location modifier " + prefixe.at(i) + ".") );
					}
					if ( prefixe.at(i).size() == 2 && (found = prefixe.at(i).find("^~")) != NOTFOUND) {
						if ((found = prefixe.at(i + 1).find("/")) == NOTFOUND 
						|| ((found = prefixe.at(i + 1).find("/")) != NOTFOUND && found > 0))
							throw ( ErrorMsg("Error : invalid location modifier " + prefixe.at(i) + ".") );
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
				throw ( ErrorMsg("Error : invalid location modifier " + prefixe.at(i) + ".") );

		if	( i != 0 && i == prefixe.size() - 2 && !prefixe.at(i - 1).compare("location") )
			if (std::find(modifiers.begin(), modifiers.end(), prefixe.at(i)) == modifiers.end())
				throw ( ErrorMsg("Error : invalid location modifier " + prefixe.at(i) + ".") );
	}

	/* remove location */
	for ( unsigned long i = 0; i < prefixe.size(); i++ )
		if ( !prefixe.at(i).compare("location") )
			prefixe.erase (prefixe.begin() + i);

	/* check duplicated URI */
	for ( unsigned long i = 0; i < prefixe.size(); i++ ) {
		for ( unsigned long j = 1; j < prefixe.size(); j++ ) {
			if ( !prefixe.at(i).compare(prefixe.at(j)) && i != j
			&& std::find(modifiers.begin(), modifiers.end(), prefixe.at(i)) == modifiers.end()) 
			{
	 			if ( prefixe.at(j - 1).compare("~") && prefixe.at(j - 1).compare("~*") ) {
					if ( i != 0 && !prefixe.at(i - 1).compare("=") && !prefixe.at(j - 1).compare("="))
						throw ( ErrorMsg("Error : invalid location modifier " + prefixe.at(i) + ".") );
					else if ( i != 0 && !prefixe.at(i - 1).compare("^~") && !prefixe.at(j - 1).compare("^~"))
						throw ( ErrorMsg("Error : invalid location modifier " + prefixe.at(i) + ".") );
					else if ( i == 0 && std::find(modifiers.begin(), modifiers.end(), prefixe.at(j - 1)) == modifiers.end())
						throw ( ErrorMsg("Error : invalid location modifier " + prefixe.at(i) + ".") );
					else if ( i != 0 && std::find(modifiers.begin(), modifiers.end(), prefixe.at(i - 1)) == modifiers.end()
					&& std::find(modifiers.begin(), modifiers.end(), prefixe.at(j - 1)) == modifiers.end())
						throw ( ErrorMsg("Error : invalid location modifier " + prefixe.at(i) + ".") );
					else if ( i == 0 && !prefixe.at(j - 1).compare("^~"))
						throw ( ErrorMsg("Error : invalid location modifier " + prefixe.at(i) + ".") );
					else if ( i != 0 && std::find(modifiers.begin(), modifiers.end(), prefixe.at(i - 1)) == modifiers.end()
					&& !prefixe.at(j - 1).compare("^~"))
						throw ( ErrorMsg("Error : invalid location modifier " + prefixe.at(i) + ".") );
				}
			}
		}
	}
	modifiers.clear();
	prefixe.clear();
	return (next_server);
}

void	Config::InitConfig( void )
{
	Serv_config	serv_config;

	serv_config.server_name = "";
	serv_config.client_max_body_size = 1000000;
	serv_config._nb_location = 0;
	_serv_config.push_back(serv_config);
	return ;
}


void		Config::InitLocation(void)
{
	_locations		location;

	location.access = "";
	location.autoindex = false;
	location.allowm = false;
	location.limitm = false;
	location.allow_methods[GET] = 0;
	location.allow_methods[POST] = 0;
	location.allow_methods[DELETE] = 0;
	location.root = DEFAULT_LOCATION;
	location.index = "";
	location.cgi_path = "";
	_serv_config.back().locations.push_back(location);
}


void	Config::parse_config(void)
{
	for (size_t i = 0; i < _tokens.size(); i++)
	{
		if (!_tokens.at(i).compare("server"))
		{
			_nb_config++;
			InitConfig();
			if (_tokens.at(++i).compare("{"))
				throw ( ErrorMsg("Error : invalid element " + _tokens.at(i) + ".") );
			while (++i < _tokens.size() && _tokens.at(i) != "}")
			{
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
					throw ( ErrorMsg("Error : unknown directive [ " + _tokens.at(i) + " ]") );
			}
			set_default_listen();	
		}	
	}
	return ;
}

void	Config::set_default_listen(void){
	if (_serv_config.back().host.empty())
		_serv_config.back().host = "0.0.0.0";
	if (_serv_config.back().port.empty())
		_serv_config.back().port = "80";
}

size_t		Config::parse_location(size_t i) {
	InitLocation();
	if ( !_tokens.at(i + 2).compare("{") )
		_serv_config.back().locations.back().access += _tokens.at(++i);
	else if ( !_tokens.at(i + 3).compare("{") )
	{
		_serv_config.back().locations.back().modifier = _tokens.at(++i);
		_serv_config.back().locations.back().access = _tokens.at(++i);
	}
	if (_tokens.at(++i).compare("{"))
		throw ( ErrorMsg("Error : invalid location element " + _tokens.at(i) + ".") );
	while (_tokens.at(++i).compare("}"))
	{
		if (!_tokens.at(i).compare("allow_methods"))
			i = set_allow_methods(i, true);
		else if (!_tokens.at(i).compare("limit_methods")) // meme fonction mais pourquoi pas avec un int pour savoir lequel
			i = set_allow_methods(i, false);
		else if (!_tokens.at(i).compare("root"))
			i = set_root(i);
		else if (!_tokens.at(i).compare("return"))
			i = set_return(i);
		else if (!_tokens.at(i).compare("autoindex"))
			i = set_autoindex(i);
		else if (!_tokens.at(i).compare("index"))
			i = set_index(i);
		else if (!_tokens.at(i).compare("cgi_path"))
			i = set_cgi_path(i);
		else
			throw ( ErrorMsg("Error : invalid location element " + _tokens.at(i) + ".") );
	}
	return ( i );
}


/* ---------------------------------------------- UTILS --------------------------------------------------------- */


void	Config::split(size_t found, int i, std::string s, int len) {
	std::string temp;

	temp = _tokens.at(i).substr(0, found);
	_tokens.insert(_tokens.begin() + i + 1, s);
	if ( _tokens.at(i).size() - 1 > found )
		_tokens.insert(_tokens.begin() + i + 2, _tokens.at(i).substr(found + len, _tokens.at(i).size() - found));
	_tokens.at(i) = temp;
}

bool	Config::is_number(const std::string& s) {

    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it))
		++it;
    return (!s.empty() && it == s.end());
}

bool 	Config::check_host(std::string host) {
	int 		count = 0; 
	size_t		pos = 0;
	size_t		next = 0;
	std::string tmp;

	if (!host.compare("localhost"))
		return (true);
	else
	{
		next = host.find(".");
		while (next != NOTFOUND)
		{
			count++;
			tmp = host.substr(pos, next - pos);
	 		if (!is_number(tmp) || std::atoi(tmp.c_str()) > 255)
	 			return (false);
			pos = next + 1;
			next = host.find(".", next + 1);
		}
		if (count != 3)
			return (false);
	}
	return (true);
}

size_t	Config::count_digit( std::string str ) {
	return std::count_if( str.begin(), str.end(), static_cast<int(*)(int)>(std::isdigit ));
}

size_t	Config::found_idx( std::string str, size_t i) {
	for (; i < _tokens.size(); i++)
		if (!_tokens.at(i).compare(str))
			return (i);
	return (0);
}

const int	Config::error_code[] = { 100, 101, 102, 200 , 201 , 202 , 203 , 204 , 205 , 206 , 207 , 208 , 226 , 300 , 301\
, 302 , 303 , 304 , 305 , 307 , 308 , 400 , 401 , 402 , 403 , 404 , 405 , 406 , 407 , 408 , 409 , 410 , 411\
, 412 , 413 , 414 , 415 , 416 , 417 , 418 , 421 , 422 , 423 , 424 , 426 , 428 , 429 , 431 , 444 , 451 , 499\
, 500 , 501 , 502 , 503 , 504 , 505 , 506 , 507 , 508 , 510 , 511 , 599};