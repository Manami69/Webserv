#include "../includes/config.hpp"

size_t		Config::parse_location(size_t i) {
	if (!_tokens.at(i + 3).compare("}")) //  prefix modifier case
	{
		i++;
		std::string c[] = {"=", "~", "~*", "^~"};
		std::vector<std::string> cas(c, c + sizeof(c) / sizeof(std::string)); //  POURQUOI CA M;ENGUEULE? ca doit etre c++ 11 par defaut ?
		if (std::find(cas.begin(), cas.end(), _tokens.at(i)) == cas.end()) // faire une erreur speciale au lieu de Wrong Config
			throw WrongConfig(); 
		else
			_serv_config.back().locations.back().modifier = _tokens[i];
		_serv_config.back().locations.back().access = _tokens.at(++i);
	}
	else if (!_tokens.at(i + 2).compare("}"))
	{
		_serv_config.back().locations.back().access = _tokens.at(++i);
	}
	else
		throw WrongConfig();
	i++;
	while (!_tokens.at(++i).compare("}"))
	{
		if (!_tokens.at(i).compare("allow_methods"))
			;
		else if (!_tokens.at(i).compare("limit_methods")) // meme fonction mais pourquoi pas avec un int pour savoir lequel
			;
		else if (!_tokens.at(i).compare("root"))
			;
		else if (!_tokens.at(i).compare("return"))
			;
		else if (!_tokens.at(i).compare("autoindex"))
			;
		else if (!_tokens.at(i).compare("index"))
			;
		else if (!_tokens.at(i).compare("CGI_path"))
			;
	}
	return i;
}
// void		Config::parse_location(unsigned long i)
// {
// 	this->init_config_location();
// 	if (this->_tokens[i].find("~") == 0)
// 	{
// 		this->_serv_config.back().locations.back().cgi = true;
// 		this->_serv_config.back().locations.back().access = this->_tokens[++i];
// 	}
// 	else if (this->_tokens[i].find("/") != 0)
// 		throw ( ErrorAccess() );
// 	else
// 		this->_serv_config.back().locations.back().access = this->_tokens[i];
// 	if (!check_line(this->_tokens[i + 1], "{"))
// 		throw ( WrongConfig() );
// 	while (i < this->_tokens.size() && this->_tokens[i] != "}")
// 	{
// 		if (check_line(this->_tokens[i], "root") && check_semicolon(++i))
// 		{
// 			check_root(this->_tokens[i]);
// 			this->_serv_config.back().locations.back().root = this->_tokens[i].substr(0, this->_tokens[i].size() - 1);
// 		}
// 		else if (check_line(this->_tokens[i], "index") && check_semicolon(++i))
// 		{
// 			this->_serv_config.back().locations.back().index.clear();
// 			while (this->_tokens[i].find(";") > this->_tokens[i].size())
// 				this->_serv_config.back().locations.back().index.push_back(_tokens[i++]);
// 			this->_serv_config.back().locations.back().index.push_back(this->_tokens[i].substr(0, this->_tokens[i].size() - 1));
// 		}
// 		else if (check_line(this->_tokens[i], "try_files") && check_semicolon(++i))
// 		{
// 			this->_serv_config.back().locations.back().try_files.clear();
// 			while (this->_tokens[i].find(";") > this->_tokens[i].size())
// 				this->_serv_config.back().locations.back().try_files.push_back(_tokens[i++]);
// 			this->_serv_config.back().locations.back().try_files.push_back(this->_tokens[i].substr(0, this->_tokens[i].size() - 1));
// 		}
// 		else if (check_line(this->_tokens[i], "client_max_body_size") && check_semicolon(++i))
// 		{
// 			check_client_max_body_size(this->_tokens[i]);
// 			this->_serv_config.back().locations.back().client_max_body_size = std::atoi(this->_tokens[i].substr(0, this->_tokens[i].size() - 1).c_str());
// 		}
// 		else if (check_line(this->_tokens[i], "allow_methods") && check_semicolon(++i))
// 		{
// 			check_allow_methods(this->_tokens[i]);
// 			this->_serv_config.back().locations.back().allow_methods = this->_tokens[i].substr(0, this->_tokens[i].size() - 1);
// 		}
// 		else if (check_line(this->_tokens[i], "cgi_path") && check_semicolon(++i))
// 		{
// 			this->_serv_config.back().locations.back().cgi_path = this->_tokens[i].substr(0, this->_tokens[i].size() - 1);
// 		}
// 		i++;
// 	}
// }

// void		Config::init_config_location(void)
// {
// 	_locations		location;

// 	location.access = "";
// 	location.cgi = false;
// 	location.index = this->_serv_config.back().index;
// 	location.root = this->_serv_config.back().root;
// 	location.client_max_body_size = this->_serv_config.back().client_max_body_size;
// 	_serv_config.back().locations.push_back(location);
// }

// size_t	Config::set_root( size_t i ) {

// 	if ( !_tokens.at(++i).compare(";") )
// 		throw ( ErrorRoot() );
// 	_serv_config.back().locations.back().root = _tokens.at(i);
// 	if ( _tokens.at(++i).compare(";") )
// 		throw ( ErrorRoot() );
// 	// NOT COMPLETE YET
// 	return ( i );
// }

// size_t	Config::set_index( size_t i ) {

// 	if ( !_tokens.at(++i).compare(";") )
// 		throw ( ErrorIndex() );
// 	while ( _tokens.at(i).compare(";") ) {
// 		_serv_config.back().locations.back().index += _tokens.at(i);
// 		_serv_config.back().locations.back().index += " ";
// 	}
// 	return ( i );
// }

// size_t	Config::set_autoindex( size_t i ) {

// 	if ( !_tokens.at(++i).compare(";") )
// 		throw ( ErrorAutoindex() );
// 	if ( !_tokens.at(i).compare("off") || !_tokens.at(i).compare("OFF"))
// 		_serv_config.back().locations.back().autoindex = false;
// 	else if ( !_tokens.at(i).compare("on") || !_tokens.at(i).compare("ON") )
// 		_serv_config.back().locations.back().autoindex = true;
// 	else
// 		throw ( ErrorRoot() );
// 	if ( _tokens.at(++i).compare(";") )
// 		throw ( ErrorRoot() );
// 	return ( i );
// }

// size_t	Config::set_allow_methods( size_t i ) {

// }