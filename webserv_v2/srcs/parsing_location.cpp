#include "../includes/config.hpp"

size_t		Config::parse_location(size_t i) {
	init_config_location();
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
		else if (!_tokens.at(i).compare("limit_methods"))
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
		else
			throw ( ErrorMsg("Error : invalid location element " + _tokens.at(i) + ".") );
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
	location.allow_methods[GET] = 0;
	location.allow_methods[POST] = 0;
	location.allow_methods[DELETE] = 0;
	location.root = DEFAULT_LOCATION;
	location.index = "";
	location.cgi_path = "";
	_serv_config.back().locations.push_back(location);
}

size_t	Config::set_root( size_t i ) {

	if ( !_tokens.at(++i).compare(";") )
		throw	( ErrorMsg("Error : location root around .") );
	_serv_config.back().locations.back().root = _tokens.at(i);
	if ( _tokens.at(++i).compare(";") )
		throw	( ErrorMsg("Error : location root around .") );
	return ( i );
}

size_t	Config::set_index( size_t i ) {
	if ( !_tokens.at(++i).compare(";") )
		throw	( ErrorMsg("Error : location index.") );
	while (_tokens.at(i).compare(";")) {
		_serv_config.back().locations.back().index += _tokens.at(i);
		_serv_config.back().locations.back().index += " ";
		i++;
	}
	return ( i );
}

size_t	Config::set_autoindex( size_t i ) {

	if ( !_tokens.at(++i).compare(";") )
		throw	( ErrorMsg("Error : location autoindex.") );
	if ( !_tokens.at(i).compare("off") || !_tokens.at(i).compare("OFF") || !_tokens.at(i).compare("Off") )
		_serv_config.back().locations.back().autoindex = false;
	else if ( !_tokens.at(i).compare("on") || !_tokens.at(i).compare("ON") || !_tokens.at(i).compare("On") )
		_serv_config.back().locations.back().autoindex = true;
	else
		throw	( ErrorMsg("Error : location autoindex.") );
	if ( _tokens.at(++i).compare(";") )
		throw	( ErrorMsg("Error : location autoindex.") );
	return ( i );
}

size_t	Config::set_return( size_t i ) {
	if ( !_tokens.at(++i).compare(";") )
		throw	( ErrorMsg("Error : location return.") );
	if ( !_tokens.at(i + 1).compare(";") )
		throw	( ErrorMsg("Error : location return.") );
	if (is_number(_tokens.at(i)) && _tokens.at(i + 1).compare(";")) {
		int code = std::atoi(_tokens.at(i).c_str());
		if (code < 0 || code > 999)
			throw	( ErrorMsg("Error : location return.") );
		_serv_config.back().locations.back().redirect.insert(std::pair<std::string, std::string>(_tokens.at(i), _tokens.at(i + 1)));
	}
	if ( _tokens.at(i + 2).compare(";") )
		throw	( ErrorMsg("Error : location return.") );
	i += 2;
	return ( i );
}

size_t	Config::set_cgi_path( size_t i ) {
	if ( !_tokens.at(++i).compare(";") )
		throw	( ErrorMsg("Error : location cig_path.") );
	_serv_config.back().locations.back().cgi_path = _tokens.at(i); // check if the cgi path end by php-cgi ?
	if ( _tokens.at(++i).compare(";") )
		throw	( ErrorMsg("Error : location cig_path.") );
	return ( i );
}

size_t	Config::set_allow_methods( size_t i, bool titre ) {
	bool isalready[] = { false, false, false };
	if ( !_tokens.at(i + 1).compare(";") )
		throw	( ErrorMsg("Error : location allow_methods.") );
	if (!titre) {
		if (_serv_config.back().locations.back().limitm)
			throw	( ErrorMsg("Error : location allow_methods.") );
		_serv_config.back().locations.back().limitm = true;
	}
	else {
		if (_serv_config.back().locations.back().allowm)
			throw	( ErrorMsg("Error : location allow_methods.") );
		_serv_config.back().locations.back().allowm = true;
	}
	while (_tokens.at(++i).compare(";")) {
		if (!isalready[GET] && !_tokens.at(i).compare("GET"))
		{
			isalready[GET] = true;
			if (_serv_config.back().locations.back().allow_methods[GET] >= 0)
				_serv_config.back().locations.back().allow_methods[GET] = titre ? 0 : -1;
		}
		else if (!isalready[POST] && !_tokens.at(i).compare("POST"))
		{
			isalready[POST] = true;
			if (_serv_config.back().locations.back().allow_methods[POST] >= 0)
				_serv_config.back().locations.back().allow_methods[POST] = titre ? 0 : -1;
		}
		else if (!isalready[DELETE] && !_tokens.at(i).compare("DELETE"))
		{
			isalready[DELETE] = true;
			if (_serv_config.back().locations.back().allow_methods[DELETE] >= 0)
				_serv_config.back().locations.back().allow_methods[DELETE] = titre ? 1 : -1;
		}
		else
			throw ( ErrorMsg("Error : location allow_methods.") );
	}
	++i;
	return ( i );
}