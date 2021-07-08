#include "./../includes/getLocation.hpp"

getLocation::getLocation(const Serv_config &s, const std::string &r) : _req(r) {
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
}

getLocation::~getLocation() {}

std::vector<_locations> 		getLocation::get_locations() const { return arr; }

// get the best location id for this config so you can check all that you need in the configuration
ID getLocation::get_id() {
	ID i = 0;
// cherche  les locations avec prefixe et egalite parfaite
	for (std::vector<_locations>::iterator it = arr.begin(); it != arr.end(); it++)
	{
		if (!it->modifier.compare("=") && !it->access.compare(_req))
			return i;
		else if (!it->modifier.compare("^~") && !it->access.compare(_req))
			return i;
		else if (!it->modifier.compare("~") && _tildeisfound(it->access, true))
			return i;
		else if (!it->modifier.compare("~*") && _tildeisfound(it->access, false))
			return i;
		else if (it->modifier.empty() && !it->access.compare(_req))
			return i;
		i++;
	}
	// si pas de super choix, choisir le - pire
	size_t best = 0;
	size_t iBest = 0;
	i = 0;
	for (std::vector<_locations>::iterator it = arr.begin(); it != arr.end(); it++)
	{
		if ((!it->modifier.compare("^~") || it->modifier.empty()) && it->access.find(_req) == 0)
		{
			if (_req.size() > best)
			{
				best = _req.size();
				iBest = i;
			}
		}
		i++;
	}
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
	if (isCaseSensitive)
		return (_req.find(loc, findFrom) != NOTFOUND);
	else
		return (strtoupper(_req).find(strtoupper(loc), findFrom) != NOTFOUND);
}

std::string	getLocation::strtoupper(std::string str) {
	std::string ret;
	for (size_t i = 0; i < str.size(); i++)
		ret += static_cast<char>(toupper(str[i]));
	return ret;
}

std::string getLocation::getRoot(ID id) {
	if (arr.size() <= id)
	{
		return ""; // DEFAULT_LOCATION
	}
	else
		return arr.at(id).root;
}

bool	getLocation::isAllowedMethod(ID id, int method) {
	try {
		if (method == DELETE) {
			return arr.at(id).allow_methods[DELETE] == 1 ? true : false;
		}
		else {
			return arr.at(id).allow_methods[method] != -1 ? true : false;
		}
	} catch (std::exception &e) {
		return method == DELETE ? false : true;
	}
}


std::string		getLocation::getRedirection(ID id) {
	try {
		if (arr.at(id).redirect.empty())
			return "";
		return arr.at(id).redirect.begin()->first + " " + arr.at(id).redirect.begin()->second;
	} catch (std::exception &e) {
		return "";
	}
}

bool							getLocation::getAutoindex(ID id) {
	try {
		return (arr.at(id).autoindex);
	} catch (std::exception &e) {
		return false;
	}
}
std::string						getLocation::getIndex(ID id) {
	try {
		return (arr.at(id).index);
	} catch (std::exception &e) {
		return "";
	}
}
std::string						getLocation::getCGIPath(ID id) {
	try {
		return (arr.at(id).cgi_path);
	} catch (std::exception &e) {
		return "";
	}
}
/*
 - 
 - 
 - 
 - 
*/

std::ostream &operator<<(std::ostream & o, const getLocation& src) {
	std::vector<_locations> array = src.get_locations();

	for (std::vector<_locations>::iterator it = array.begin(); it != array.end(); it++)
		o << it->modifier << " " << it->access << std::endl;
	return o;
}
