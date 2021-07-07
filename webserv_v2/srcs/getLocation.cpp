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
// ID getLocation::get_id( std::string requestline ) {
// 	std::vector<_locations>::iterator it = arr.begin();
// 	ID i = 0;
// // cherche  les locations avec prefixe
// 	for (; it != arr.end(); it++)
// 	{
// 		if (!it->modifier.compare("=") && !it->access.compare(_req))
// 			return i;
// 		else if (!it->modifier.compare("^~") && !it->access.compare(_req)) // pas vrai si chemin plus long trouve
// 			return i;
// 		else if (!it->modifier.compare("~") && _tildeisfound(it->access, true))
// 			return i;
// 		else if (!it->modifier.compare("~*") && _tildeisfound(it->access, false))
// 			return i;
// 		i++;
// 	}
// }

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
/*

std::string getLocation::get_location_root(ID id) {
	if (arr.size() <= id)
	{
		return "";
	}
	else
		return arr.at(id).root;
}*/

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
