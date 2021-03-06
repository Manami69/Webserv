#include "./../includes/getLocation.hpp"

getLocation::getLocation(const Serv_config &s, const std::string &r) : _req(r) {

	std::vector<_locations> arr;
	for (std::list<_locations>::const_iterator it = s.locations.begin(); it != s.locations.end(); it++) // first loop, get = modifiers
	{
		if (!it->modifier.compare("=")) {
			arr.push_back(*it);
		}
	}
	for (std::list<_locations>::const_iterator it = s.locations.begin(); it != s.locations.end(); it++) // get modifier ^~
	{
		if (!it->modifier.compare("^~")) {
			arr.push_back(*it);
		}
	}
	for (std::list<_locations>::const_iterator it = s.locations.begin(); it != s.locations.end(); it++) // get modifier ~ and ~*
	{
		if (!it->modifier.compare("~*") || !it->modifier.compare("~")) {
			arr.push_back(*it);
		}
	}
	for (std::list<_locations>::const_iterator it = s.locations.begin(); it != s.locations.end(); it++) // get nomod
	{
		if (it->modifier.empty()) {
			arr.push_back(*it);
		}
	}
	// for (std::vector<_locations>::const_iterator it = arr.begin(); it != arr.end(); it++) // get nomod
	// {
	// 	std::cout << it->modifier << " " << it->access << std::endl;
	// }
	int id = get_id(arr);
	if (id < 0)
		_infos = _returnDefault();
	else
		_infos = arr.at(id);
	// std::cout << _infos.access << " ID = " << id << std::endl;
}

getLocation::~getLocation() {}


// get the best location id for this config so you can check all that you need in the configuration
int getLocation::get_id(std::vector<_locations> & arr) {
	int i = 0;
	if (arr.empty())
		return -1;
// cherche  les locations avec prefixe et egalite parfaite
	for (std::vector<_locations>::iterator it = arr.begin(); it != arr.end(); it++)
	{
		if (!it->modifier.compare("=") && !it->access.compare(_req))
			return i;
		else if (!it->modifier.compare("^~") && !it->access.compare(_req))
			return i;
		else if (!it->modifier.compare("~") && _tildeisfound(it->access, true)){
			return i;
		}
		else if (!it->modifier.compare("~*") && _tildeisfound(it->access, false))
			return i;
		else if (it->modifier.empty() && !it->access.compare(_req))
			return i;
		std::cout << "HEY " << i << " " << it->access << std::endl; 
		i++;
	}
	std::cout << "fin de boucle" << std::endl;
	// si pas de super choix, choisir le - pire
	size_t best = 0;
	int iBest = -1;
	i = 0;
	for (std::vector<_locations>::iterator it = arr.begin(); it != arr.end(); it++)
	{
		if ((!it->modifier.compare("^~") || it->modifier.empty()) && _req.find(it->access) == 0)
		{
		std::cout << i << " " << it->access << " " << it->access.size() << " " <<  best <<std::endl;
			if (it->access.size() > best)
			{
				best = it->access.size();
				iBest = i;
			}
		}
		i++;
	}
	std::cout << iBest << " " << arr[iBest].access << " " << arr[iBest].index<< std::endl;
	return (iBest);
}

bool getLocation::_tildeisfound(std::string loc, bool isCaseSensitive)
{
	int findFrom = 0;
	if (loc[loc.size() - 1] == '$')
	{
		loc.erase(loc.size() - 1);
		findFrom = _req.size() - loc.size();
	}
	if (isCaseSensitive) {
		return (_req.find(loc, findFrom) != NOTFOUND);
	}
	else {
		return (strtoupper(_req).find(strtoupper(loc), findFrom) != NOTFOUND);
	}
}

std::string	getLocation::strtoupper(std::string str) {
	std::string ret;
	for (size_t i = 0; i < str.size(); i++)
		ret += static_cast<char>(toupper(str[i]));
	return ret;
}

std::string getLocation::getRoot() {
		return _infos.root;
}

bool	getLocation::isAllowedMethod(int method) {
	try {
		if (method == DELETE) {
			return _infos.allow_methods[DELETE] == 1 ? true : false;
		}
		else {
			return _infos.allow_methods[method] != -1 ? true : false;
		}
	} catch (std::exception &e) {
		return method == DELETE ? false : true;
	}
}

std::string		getLocation::getRedirection() {

	if (_infos.redirect.empty())
		return "";
	return _infos.redirect.begin()->first + " " + _infos.redirect.begin()->second;
}

bool							getLocation::getAutoindex() {
	return (_infos.autoindex);
}

std::string						getLocation::getIndex() {
	return (_infos.index);
}

std::string						getLocation::getCGIPath() {
	return (_infos.cgi_path);
}

_locations getLocation::_returnDefault() {
	_locations loc;
	loc.access = "/";
	loc.allow_methods[GET] = 0;
	loc.allow_methods[POST] = 0;
	loc.allow_methods[DELETE] = 0;
	loc.root = DEFAULT_LOCATION;
	loc.autoindex = false;
	return loc;
}