#include "./../includes/getLocation.hpp"

getLocation::getLocation(const Serv_config &s) : arr(s.locations) {}
getLocation::~getLocation() {}

// get the best location id for this config so you can check all that you need in the configuration
ID getLocation::get_id( std::string requestline ) {
	std::vector<_locations>::iterator it = arr.begin();
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
}

/*
 - 
 - 
 - 
 - 
*/