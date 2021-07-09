#include "../includes/config.hpp"

const char *Config::ErrorListen::what() const throw() {
	return ( "Error : Wrong listen." );
}

const char *Config::ErrorServerName::what() const throw() {
	return ( "Error : Wrong server name." );
}

const char *Config::ErrorRoot::what() const throw() {
	return ( "Error : Wrong root." );
}

const char *Config::ErrorClientMaxBodySize::what() const throw() {
	return ( "Error : Wrong client max body size." );
}

const char *Config::ErrorPage::what() const throw() {
	return ( "Error : Wrong error page." );
}

const char *Config::ErrorAccess::what() const throw() {
	return ( "Error : Wrong access to location." );
}

const char *Config::ErrorMethods::what() const throw() {
	return ( "Error : Wrong methods to location." );
}

const char *Config::ErrorIndex::what() const throw() {
	return ( "Error : Wrong index." );
}

const char *Config::ErrorAutoindex::what() const throw() {
	return ( "Error : Wrong autoindex." );
}

const char *Config::ErrorReturn::what() const throw() {
	return ( "Error : Wrong return." );
}

const char *Config::ErrorCgiPath::what() const throw() {
	return ( "Error : Wrong cgi path." );
}

const char *Config::ErrorTryFiles::what() const throw() {
	return ( "Error : Wrong try_files." );
}

const int	Config::error_code[] = { 100, 101, 102, 200 , 201 , 202 , 203 , 204 , 205 , 206 , 207 , 208 , 226 , 300 , 301\
, 302 , 303 , 304 , 305 , 307 , 308 , 400 , 401 , 402 , 403 , 404 , 405 , 406 , 407 , 408 , 409 , 410 , 411\
, 412 , 413 , 414 , 415 , 416 , 417 , 418 , 421 , 422 , 423 , 424 , 426 , 428 , 429 , 431 , 444 , 451 , 499\
, 500 , 501 , 502 , 503 , 504 , 505 , 506 , 507 , 508 , 510 , 511 , 599};

// static ?
bool 	Config::check_host(std::string host) {
	int 		count = 0; 
	size_t		pos = 0;
	size_t		next = 0;
	std::string tmp;

	if (!host.compare("localhost"))
		return true;
	else
	{
		next = host.find(".");
		while (next != NOTFOUND)
		{
			count++;
			tmp = host.substr(pos, next - pos);
	 		if (!is_number(tmp) || std::atoi(tmp.c_str()) > 255)
	 			return false;
			pos = next + 1;
			next = host.find(".", next + 1);
		}
		if (count != 3)
			return false;
	}
	return true;
}

size_t	Config::set_listen(size_t i)
{
	std::string		host = _serv_config.back().host;
	std::string		port = _serv_config.back().port;
	std::string		tmp;
	size_t 			found = 0;

	i++;
	if (!_tokens.at(i).compare(";"))
		throw ( ErrorListen() );
	while (_tokens.at(i).compare(";")) {
		if ((found = _tokens.at(i).find(":")) != NOTFOUND && _tokens.at(i).find("[") == NOTFOUND)
		{
			if (!host.empty() || !port.empty())
				throw ( ErrorListen() );	
			host = _tokens.at(i).substr(0, found);
			port = _tokens.at(i).substr(found + 1);
			if (!check_host(host)){
				throw ( ErrorListen() );
			}
			if (!is_number(port) || std::atoi(port.c_str()) > 65535)
				throw ( ErrorListen() );
			found = 0;
		}
		else if (is_number(_tokens.at(i))) //check 2 times ???
		{
			if (!port.empty())
				throw ( ErrorListen() );	
			port = _tokens.at(i);
			if (!is_number(port) || std::atoi(port.c_str()) > 65535) //check 2 times ???
				throw ( ErrorListen() );
		}
		else if (!_tokens.at(i).compare("default_server")) // [::]:80 default_server
			;
		else if (!_tokens.at(i).compare("localhost") || _tokens.at(i).find(".") != NOTFOUND)
		{
			if (!host.empty())
				throw ( ErrorListen() );	
			host = _tokens.at(i);
			if (!check_host(host))
				throw ( ErrorListen() );
		}
		else
			throw ( ErrorListen() );
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
		throw ( ErrorServerName() );
	while (_tokens.at(i).compare(";")) {
		_serv_config.back().server_name += _tokens.at(i);
		_serv_config.back().server_name += " ";
		i++;
	}
	return (i);
}

// static ?
size_t count_digit( std::string str ) {
	return std::count_if( str.begin(), str.end(), static_cast<int(*)(int)>(std::isdigit ));
}

size_t	Config::set_client_max_body_size( size_t i )
{
	if (!_tokens.at(++i).compare(";"))
		throw ( ErrorClientMaxBodySize() );
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
			throw ( ErrorClientMaxBodySize() );
		if (digit != _tokens.at(i).size() - 1)
			throw ( ErrorClientMaxBodySize() );
	}
	i++;
	return (i);
}

size_t	Config::set_error_page(size_t i)
{
	std::list<int> num;
	std::string page;
	std::string redirect;
	//std::list<int> errorcode(error_code, error_code + sizeof(error_code)/sizeof(int));
	int count = 0;

	while (_tokens.at(++i).compare(";"))
	{
		if (!count && !is_number(_tokens.at(i)))
			throw ( ErrorPage() );
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
			throw ( ErrorPage() );
	}
	if (!redirect.empty())
		page = redirect + " " + page; // a tester mdr
	if (page.empty())
		throw ( ErrorPage() );
	for (std::list<int>::iterator it = num.begin(); it != num.end(); it++)
		this->_serv_config.back().error_page[*it] = page;
	return i;
}
