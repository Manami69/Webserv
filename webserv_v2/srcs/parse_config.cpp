#include "../includes/config.hpp"

void	Config::init_serv_config( void )
{
	Serv_config	serv_config;

	// serv_config.host = "0.0.0.0";
	// serv_config.port = "80";
	serv_config.server_name = "";
	serv_config.client_max_body_size = 1000000; /* nginx default upload limit 1MB */
	serv_config._nb_location = 0;
	_serv_config.push_back(serv_config);
	return ;
}

void	Config::parse_config(void)
{
	for (size_t i = 0; i < _tokens.size(); i++)
	{
		if (!_tokens.at(i).compare("server"))
		{
			_nb_server++;
			init_serv_config();
			if (_tokens.at(++i).compare("{"))
				throw	( ErrorMsg("Error : invalid element " + _tokens.at(i) + ".") );
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
		}
	}
}

/* //////////////////////////////// SETTERS ////////////////////////////////// */

size_t	Config::set_listen(size_t i)
{
	std::string		host = _serv_config.back().host; // ??
	std::string		port = _serv_config.back().port; // ??
	std::string		tmp;
	size_t 			found = 0;

	i++;
	if (!_tokens.at(i).compare(";"))
		throw ( ErrorMsg("Error : listen.") );
	while (_tokens.at(i).compare(";")) {
		if ((found = _tokens.at(i).find(":")) != NOTFOUND && _tokens.at(i).find("[") == NOTFOUND)
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
		else if (is_number(_tokens.at(i))) //check 2 times ???
		{
			if (!port.empty())
				throw ( ErrorMsg("Error : listen.") );	
			port = _tokens.at(i);
			if (!is_number(port) || std::atoi(port.c_str()) > 65535) //check 2 times ???
				throw ( ErrorMsg("Error : listen.") );
		}
		else if (!_tokens.at(i).compare("default_server")) // [::]:80 default_server
			;
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
	_serv_config.back().host = !host.compare("localhost") ? "127.0.0.1" : host;
	_serv_config.back().port = port;
	return (i);
}

size_t	Config::set_server_name( size_t i )
{
	i++;
	if (!_tokens.at(i).compare(";"))
		throw ( ErrorMsg("Error : server_name.") );
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
		throw ( ErrorMsg("Error : client_max_body_size.") );
	size_t digit = this->count_digit(_tokens.at(i));
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
			_serv_config.back().client_max_body_size *= 1000000000;
		else
			throw ( ErrorMsg("Error : client_max_body_size.") );
		if (digit != _tokens.at(i).size() - 1)
			throw ( ErrorMsg("Error : client_max_body_size.") );
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
			throw ( ErrorMsg("Error : error_page.") );
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
			throw ( ErrorMsg("Error : error_page.") );
	}
	if (!redirect.empty())
		page = redirect + " " + page; // a tester mdr
	if (page.empty())
		throw ( ErrorMsg("Error : error_page.") );
	for (std::list<int>::iterator it = num.begin(); it != num.end(); it++)
		this->_serv_config.back().error_page[*it] = page;
	return i;
}
