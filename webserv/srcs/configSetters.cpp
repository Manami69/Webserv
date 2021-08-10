#include "../includes/config.hpp"

size_t	Config::set_listen(size_t i)
{
	std::string		host;
	std::string		port;
	std::string		tmp;
	size_t 			found = 0;

	i++;
	if (!_tokens.at(i).compare(";"))
		throw	( ErrorMsg("Error : listen.") );
	while (_tokens.at(i).compare(";")) {
		if ((found = _tokens.at(i).find(":")) != NOTFOUND)
		{
			if (!host.empty() || !port.empty())
				throw ( ErrorMsg("Error : listen.") );
			host = _tokens.at(i).substr(0, found);
			port = _tokens.at(i).substr(found + 1);
			if (!check_host(host)){
				throw ( ErrorMsg("Error : listen.") );
			}
			if (!is_number(port) || std::atoi(port.c_str()) > 65535)
				throw ( ErrorMsg("Error : listen.") );
			found = 0;
		}
		else if (is_number(_tokens.at(i)))
		{
			if (!port.empty())
				throw ( ErrorMsg("Error : listen.") );	
			port = _tokens.at(i);
			if (std::atoi(port.c_str()) > 65535)
				throw ( ErrorMsg("Error : listen.") );
		}
		else if (!_tokens.at(i).compare("localhost") || _tokens.at(i).find(".") != NOTFOUND)
		{
			if (!host.empty())
				throw ( ErrorMsg("Error : listen.") );	
			host = _tokens.at(i);
			if (!check_host(host))
				throw ( ErrorMsg("Error : listen.") );
		}
		else
			throw ( ErrorMsg("Error : listen.") );
		i++;
	}
	if (!_serv_config.back().host.empty() || !_serv_config.back().port.empty())
		throw ErrorMsg ("Error : listen directive is duplicate.");
	if (!host.empty())
		_serv_config.back().host = !host.compare("localhost") ? "127.0.0.1" : host;
	if  (!port.empty())
		_serv_config.back().port = port;
	return (i);
}

size_t	Config::set_server_name( size_t i )
{
	i++;
	if (!_tokens.at(i).compare(";"))
		throw	( ErrorMsg("Error : server_name.") );
	while (_tokens.at(i).compare(";")) {
		_serv_config.back().server_name += _tokens.at(i);
		_serv_config.back().server_name += " ";
		i++;
	}
	return (i);
}

size_t	Config::set_client_max_body_size( size_t i )
{
	if (!_tokens.at(++i).compare(";"))
		throw	( ErrorMsg("Error : client_max_body_size.") );
	size_t digit = count_digit(_tokens.at(i));
	if (digit == _tokens.at(i).size())
		_serv_config.back().client_max_body_size = atoi(_tokens.at(i).c_str());
	else {
		std::locale loc;
		if (isdigit(_tokens.at(i)[0],loc)) {
			size_t limit_size;
			std::stringstream(_tokens.at(i)) >> limit_size;
			_serv_config.back().client_max_body_size = limit_size;
		}
		if (_tokens.at(i).at(digit) == 'K' || _tokens.at(i).at(digit) == 'k')
			_serv_config.back().client_max_body_size *= 1000;
		else if (_tokens.at(i).at(digit) == 'M' || _tokens.at(i).at(digit) == 'm')
			_serv_config.back().client_max_body_size *= 1000000;
		else if (_tokens.at(i).at(digit) == 'G' || _tokens.at(i).at(digit) == 'g')
			_serv_config.back().client_max_body_size *= 1000000000; // do we need ?
		else
			throw	( ErrorMsg("Error : client_max_body_size.") );
		if (digit != _tokens.at(i).size() - 1)
			throw	( ErrorMsg("Error : client_max_body_size.") );
	}
	i++;
	return (i);
}

size_t	Config::set_error_page(size_t i)
{
	std::list<int> num;
	std::string page;
	std::string redirect;
	int count = 0;

	while (_tokens.at(++i).compare(";"))
	{
		if (!count && !is_number(_tokens.at(i)))
			throw	( ErrorMsg("Error : error_page.") );
		else if (!is_number(_tokens.at(i)) && _tokens.at(i)[0] != '=' && page.empty())
			page = _tokens.at(i);
		else if (_tokens.at(i)[0] == '=' && is_number(_tokens.at(i).substr(1)) && \
		atoi(_tokens.at(i).substr(1).c_str()) > 0 && atoi(_tokens.at(i).substr(1).c_str()) < 1000 && redirect.empty())
			redirect = _tokens.at(i);
		else if (is_number(_tokens.at(i)) && atoi(_tokens.at(i).c_str()) > 299 && atoi(_tokens.at(i).c_str()) < 600) // 300 and 599
		{
			count++;
			num.push_back(atoi(_tokens.at(i).c_str()));
		}
		else
			throw	( ErrorMsg("Error : error_page.") );
	}
	if (!redirect.empty())
		page = redirect + " " + page; // a tester mdr
	if (page.empty())
		throw	( ErrorMsg("Error : error_page.") );
	for (std::list<int>::iterator it = num.begin(); it != num.end(); it++)
		this->_serv_config.back().error_page[*it] = page;
	return (i);
}

size_t	Config::set_allow_methods( size_t i, bool titre ) {
	bool isalready[] = { false, false, false };
	std::string only;
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
		if (!titre)
			only += _tokens.at(i);
		if (!isalready[GET] && !_tokens.at(i).compare("GET"))
		{
			isalready[GET] = titre ? false : true;
			if (_serv_config.back().locations.back().allow_methods[GET] != -1)
				_serv_config.back().locations.back().allow_methods[GET] = titre ? 0 : 0;
		}
		else if (!isalready[POST] && !_tokens.at(i).compare("POST"))
		{
			isalready[POST] = titre ? false : true;
			if (_serv_config.back().locations.back().allow_methods[POST] != -1)
				_serv_config.back().locations.back().allow_methods[POST] = titre ? 0 : 0;
		}
		else if (!_tokens.at(i).compare("DELETE"))
		{
			if (!titre && isalready[DELETE])
				;
			else {
				isalready[DELETE] = titre ? false : true;
				if (_serv_config.back().locations.back().allow_methods[DELETE] != -1)
					_serv_config.back().locations.back().allow_methods[DELETE] = titre ? 1 : 0;
			}
		}
		else
			throw	( ErrorMsg("Error : location allow_methods.") );
	}
	if (!titre) {
		if (!isalready[GET] && only.find("GET") == NOTFOUND)
			_serv_config.back().locations.back().allow_methods[GET] = -1;
		if (!isalready[POST] && only.find("POST") == NOTFOUND)
			_serv_config.back().locations.back().allow_methods[POST] = -1;
		if (!isalready[DELETE] && only.find("DELETE") == NOTFOUND)
			_serv_config.back().locations.back().allow_methods[DELETE] = -1;
	}
	return ( i );
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
	if (is_number(_tokens.at(i)) && _tokens.at(i + 1).compare(";")) {
		int code = std::atoi(_tokens.at(i).c_str());
		if (code < 0 || code > 999)
			throw	( ErrorMsg("Error : location return.") );
		if ( _tokens.at(i + 2).compare(";") )
			throw	( ErrorMsg("Error : location return.") );
		_serv_config.back().locations.back().redirect.insert(std::pair<std::string, std::string>(_tokens.at(i), _tokens.at(i + 1)));
		return (i + 2);
	}
	else if (is_number(_tokens.at(i)) && !_tokens.at(i + 1).compare(";")){
		int code = std::atoi(_tokens.at(i).c_str());
		if (code < 0 || code > 999)
			throw	( ErrorMsg("Error : location return.") );
		_serv_config.back().locations.back().redirect.insert(std::pair<std::string, std::string>(_tokens.at(i), ""));
		return (i + 1);
	}
	throw	( ErrorMsg("Error : location return.") );
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
