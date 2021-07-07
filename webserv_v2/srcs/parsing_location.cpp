#include "../includes/config.hpp"

size_t		Config::parse_location(size_t i) {
	if (!_tokens.at(i + 3).compare("{")) //  prefix modifier case
	{
		i++;
		std::string c[] = {"=", "~", "~*", "^~"};
		std::vector<std::string> cas(c, c + sizeof(c) / sizeof(std::string)); //  POURQUOI CA M;ENGUEULE? ca doit etre c++ 11 par defaut ?
		if (std::find(cas.begin(), cas.end(), _tokens.at(i)) == cas.end()) 
			throw ErrorLocationPrefix();
		else
			_serv_config.back().locations.back().modifier = _tokens[i];
		_serv_config.back().locations.back().access = _tokens.at(++i);
	}
	else if (!_tokens.at(i + 2).compare("{"))
		_serv_config.back().locations.back().access = _tokens.at(++i);
	else
		throw ErrorLocationPrefix();
	init_config_location();
	if (_tokens.at(++i).compare("{"))
		throw	( ErrorLocationPrefix() );
	while (_tokens.at(++i).compare("}"))
	{
		if (!_tokens.at(i).compare("allow_methods"))
			i = set_allow_methods(i, true);
		else if (!_tokens.at(i).compare("limit_methods")) // meme fonction mais pourquoi pas avec un int pour savoir lequel
			i = set_allow_methods(i, false);
		if (!_tokens.at(i).compare("root"))
			i = set_root(i);
		else if (!_tokens.at(i).compare("return"))
			i = set_return(i);
		else if (!_tokens.at(i).compare("autoindex"))
			i = set_autoindex(i);
		else if (!_tokens.at(i).compare("index"))
			i = set_index(i);
		else if (!_tokens.at(i).compare("cgi_path"))
			i = set_cgi_path(i);
		else if (!_tokens.at(i).compare("try_files"))
			i = set_try_files(i);
	}
	return ( i );
}

void		Config::init_config_location(void)
{
	_locations		location;

	location.access = "";
	location.autoindex = false;
	location.allowm = false;
	location.limitm = false;
	location.allow_methods[0] = 0;
	location.allow_methods[1] = 0;
	location.allow_methods[2] = 0;
	location.root = "";
	location.index = "";
	location.cgi_path = "";
	location.try_files = "";
	_serv_config.back().locations.push_back(location);
}

size_t	Config::set_root( size_t i ) {

	if ( !_tokens.at(++i).compare(";") )
		throw ( ErrorRoot() );
	_serv_config.back().locations.back().root = _tokens.at(i);
	if ( _tokens.at(++i).compare(";") )
		throw ( ErrorRoot() );
	return ( i );
}

size_t	Config::set_index( size_t i ) {
	if ( !_tokens.at(++i).compare(";") )
		throw ( ErrorIndex() );
	while (_tokens.at(i).compare(";")) {
		_serv_config.back().locations.back().index += _tokens.at(i);
		_serv_config.back().locations.back().index += " ";
		i++;
	}
	return ( i );
}

size_t	Config::set_autoindex( size_t i ) {

	if ( !_tokens.at(++i).compare(";") )
		throw ( ErrorAutoindex() );
	if ( !_tokens.at(i).compare("off") || !_tokens.at(i).compare("OFF") || !_tokens.at(i).compare("Off") )
		_serv_config.back().locations.back().autoindex = false;
	else if ( !_tokens.at(i).compare("on") || !_tokens.at(i).compare("ON") || !_tokens.at(i).compare("On") )
		_serv_config.back().locations.back().autoindex = true;
	else
		throw ( ErrorAutoindex() );
	if ( _tokens.at(++i).compare(";") )
		throw ( ErrorAutoindex() );
	return ( i );
}

size_t	Config::set_return( size_t i ) {
	if ( !_tokens.at(++i).compare(";") )
		throw ( ErrorReturn() );
	if ( !_tokens.at(i + 1).compare(";") )
		throw ( ErrorReturn() );
	if (is_number(_tokens.at(i)) && _tokens.at(i + 1).compare(";"))
		_serv_config.back().locations.back().redirect.insert(std::pair<std::string, std::string>(_tokens.at(i), _tokens.at(i + 1)));
	if ( _tokens.at(i + 2).compare(";") )
		throw ( ErrorReturn() );
	i += 2;
	return ( i );
}

size_t	Config::set_cgi_path( size_t i ) {
	if ( !_tokens.at(++i).compare(";") )
		throw ( ErrorCgiPath() );
	_serv_config.back().locations.back().cgi_path = _tokens.at(i); // check if the cgi path end by php-cgi ?
	if ( _tokens.at(++i).compare(";") )
		throw ( ErrorCgiPath() );
	return ( i );
}

size_t	Config::set_try_files( size_t i ) {
	if ( !_tokens.at(++i).compare(";") )
		throw ( ErrorTryFiles() );
	while ( _tokens.at(i).compare(";") ) {
		_serv_config.back().locations.back().try_files += _tokens.at(i);
		_serv_config.back().locations.back().try_files += " ";
		i++;
	}
	return ( i );
}

size_t	Config::set_allow_methods( size_t i, bool titre ) {
	bool isalready[] = { false, false, false };
	if ( !_tokens.at(i + 1).compare(";") )
		throw ( ErrorMethods() ); // je sais pas quelle erreur
	if (!titre) {
		if (_serv_config.back().locations.back().limitm)
			throw ( ErrorMethods() );
		_serv_config.back().locations.back().limitm = true;
	}
	else {
		if (_serv_config.back().locations.back().allowm)
			throw ( ErrorMethods() );
		_serv_config.back().locations.back().allowm = true;
	}
	while (_tokens.at(++i).compare(";")) {
		if (!isalready[0] && !_tokens.at(i).compare("GET"))
		{
			isalready[0] = true;
			if (_serv_config.back().locations.back().allow_methods[0] >= 0)
				_serv_config.back().locations.back().allow_methods[0] = titre ? 0 : -1;
		}
		else if (!isalready[1] && !_tokens.at(i).compare("POST"))
		{
			isalready[1] = true;
			if (_serv_config.back().locations.back().allow_methods[1] >= 0)
				_serv_config.back().locations.back().allow_methods[1] = titre ? 0 : -1;
		}
		else if (!isalready[2] && !_tokens.at(i).compare("DELETE"))
		{
			isalready[2] = true;
			if (_serv_config.back().locations.back().allow_methods[2] >= 0)
				_serv_config.back().locations.back().allow_methods[2] = titre ? 1 : -1;
		}
		else
			throw ErrorMethods();
	}
	return ( i );
}