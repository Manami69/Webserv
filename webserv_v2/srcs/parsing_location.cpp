#include "../includes/config.hpp"

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