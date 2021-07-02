#ifndef GETLOCATION_HPP
# define GETLOCATION_HPP

#include "getResponse.hpp"
#define ID unsigned int
/*
struct						_locations
{
	std::string					modifier;
	std::string					access;
	bool						allowm;
	bool						limitm;
	int							allow_methods[3];
	std::string					root;
	std::string					index;
	bool						autoindex;
	std::string					cgi_path;
	std::string					try_files;
	std::map<std::string, std::string>	redirect;
};
*/
class getLocation {
	public:
		getLocation(const Serv_config &s);
		~getLocation();
		ID					get_id( std::string requestline );
		std::string			get_location_root(ID id);
	
	private:
		std::vector<_locations> arr;

		getLocation(const getLocation& src);
		getLocation();
		getLocation &operator=(const getLocation &rhs);
};

#endif