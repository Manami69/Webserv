#include "./../includes/getLocation.hpp"

getLocation::getLocation(const Serv_config &s, const std::string &r) : _req(r) {
	std::vector<_locations>::const_iterator it = s.locations.begin();
	for (; it != s.locations.end(); it++) // first loop, get = modifiers
	{
		if (!it->modifier.compare("="))
			arr.push_back(*it);
	}
	for (; it != s.locations.end(); it++) // get modifier ^~
	{
		if (!it->modifier.compare("^~"))
			arr.push_back(*it);
	}
	for (; it != s.locations.end(); it++) // get modifier ~ and ~*
	{
		if (!it->modifier.compare("~*") || !it->modifier.compare("~"))
			arr.push_back(*it);
	}
	for (; it != s.locations.end(); it++) // get nomod
	{
		if (it->modifier.empty())
			arr.push_back(*it);
	}
}
getLocation::~getLocation() {}

std::vector<_locations> 		getLocation::get_locations() const { return arr; }
// get the best location id for this config so you can check all that you need in the configuration
/*
ID getLocation::get_id( std::string requestline ) {
	std::vector<_locations>::iterator it = arr.begin();
	std::
// cherche  les locations avec prefixe
	for (; it != arr.end(); it++)
	{}
}

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
