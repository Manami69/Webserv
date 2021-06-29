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

// static or not ?
bool	is_number(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it))
		++it;
    return (!s.empty() && it == s.end());
}

// static ?
static bool _check_host(std::string host) {
	int count = 0; 
	size_t pos = 0;
	size_t next = 0;
	std::string tmp;

	if (!host.compare("localhost"))
		return true;
	else
	{
		while ((next = host.find(".", pos)) != NOTFOUND)
		{
			count++;
			tmp = host.substr(pos, next);
	 		if (!is_number(tmp) || std::atoi(tmp.c_str()) > 255)
	 			return false;
			next = pos;
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
	if (!_tokens[i].compare(";"))
		throw ( ErrorListen() );
	while (_tokens[i].compare(";")) {
		if ((found = _tokens[i].find(":")) != NOTFOUND && _tokens[i].find("[") == NOTFOUND)
		{
			if (!host.empty() || !port.empty())
				throw ( ErrorListen() );	
			host = _tokens[i].substr(0, found);
			port = _tokens[i].substr(found + 1);
			if (!_check_host(host))
				throw ( ErrorListen() );			
			if (!is_number(port) || std::atoi(port.c_str()) > 65535)
				throw ( ErrorListen() );
			found = 0;
		}
		else if (is_number(_tokens[i])) //check 2 times ???
		{
			if (!port.empty())
				throw ( ErrorListen() );	
			port = _tokens[i];
			if (!is_number(port) || std::atoi(port.c_str()) > 65535) //check 2 times ???
				throw ( ErrorListen() );
		}
		else if (!_tokens[i].compare("default_server")) // [::]:80 default_server
			;
		else if (!_tokens[i].compare("localhost") || _tokens[i].find(".") != NOTFOUND)
		{
			if (!host.empty())
				throw ( ErrorListen() );	
			host = _tokens[i];
			if (!_check_host(host))
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

size_t	Config::check_server_name( size_t i )
{
	i++;
	std::cout << i << " " <<_tokens[i] << std::endl; // hahaha
	if (!_tokens[i].compare(";"))
		throw ( ErrorServerName() );
	while (_tokens[i].compare(";")) {
		_serv_config.back().server_name += _tokens[i];
		_serv_config.back().server_name += " ";
		i++;
	}
	return (i);
}

// size_t	Config::check_root( size_t i )
// {
// 	i++;
// 	if (!_tokens[i].compare(";"))
// 		throw ( ErrorRoot() );
// 	_serv_config.back().root = _tokens[i]; // need to modify for location structure
// 	i++;
// 	if (_tokens[i].compare(";"))
// 		throw ( ErrorRoot() );
// 	return (i);
// }

// size_t	Config::check_index( size_t i )
// {
// 	i++;
// 	if (!_tokens[i].compare(";"))
// 		throw ( ErrorIndex() );
// 	while (_tokens[i].compare(";")) {
// 		_serv_config.index += _tokens[i]; // need to modify for location structure
// 		_serv_config.index += " "; // need to modify for location structure
// 	}
// 	return (i);
// }

// static ?
size_t count_digit(std::string str)
{
	return std::count_if(str.begin(), str.end(), static_cast<int(*)(int)>(std::isdigit));
}

size_t	Config::check_client_max_body_size( size_t i )
{
	i++;
	if (!_tokens[i].compare(";"))
		throw ( ErrorClientMaxBodySize() );
	size_t digit = count_digit(_tokens[i]);
	if (digit == _tokens[i].size())
		_serv_config.back().client_max_body_size = atoi(_tokens[i].c_str());
	else {
		std::locale loc;
		if (isdigit(_tokens[i][0],loc)) {
			size_t limit_size;
			std::stringstream(_tokens[i]) >> limit_size;
			_serv_config.back().client_max_body_size = limit_size;
		}
		if (_tokens[i].at(digit) == 'M')
			_serv_config.back().client_max_body_size *= 1000;
	}
	return (i);
}

size_t	Config::check_error_page(size_t i)
{
	std::list<int> num;
	std::string page;
	std::string redirect;
	int		e[] = {	100, 101, 102, 200 , 201 , 202 , 203 , 204 , 205 , 206 , 207 , 208 , 226 , 300 , 301\
	, 302 , 303 , 304 , 305 , 307 , 308 , 400 , 401 , 402 , 403 , 404 , 405 , 406 , 407 , 408 , 409 , 410 , 411\
	, 412 , 413 , 414 , 415 , 416 , 417 , 418 , 421 , 422 , 423 , 424 , 426 , 428 , 429 , 431 , 444 , 451 , 499\
	, 500 , 501 , 502 , 503 , 504 , 505 , 506 , 507 , 508 , 510 , 511 , 599};
	std::list<int> errorcode(e, e + sizeof(e)/sizeof(int));
	int count = 0;

	while (_tokens[++i].compare(";"))
	{
		if (!count && !is_number(_tokens[i]))
			throw ( ErrorPage() );
		else if (!is_number(_tokens[i]) && _tokens[i][0] != '=' && page.empty())
			page = _tokens[i];
		else if (_tokens[i][0] == '=' && is_number(_tokens[i].substr(1)) && \
		std::find(errorcode.begin(), errorcode.end(), atoi(_tokens[i].substr(1).c_str())) != errorcode.end() && redirect.empty())
			redirect = _tokens[i];
		else if (is_number(_tokens[i]) && std::find(errorcode.begin(), errorcode.end(), atoi(_tokens[i].c_str())) != errorcode.end())
		{
			count++;
			num.push_back(atoi(_tokens[i].c_str()));
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

//size_t	Config::check_allow_methods( size_t i )
//{ 
// 		throw ( ErrorMethods() );
	// else if (!check_line(conf, "GET;") && !check_line(conf, "PUT;") && !check_line(conf, "POST;"))
	// 	throw ( ErrorMethods() );
// 	return (i);
// }
