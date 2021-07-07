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
		getLocation(const Serv_config &s, const std::string& r);
		~getLocation();
		ID					get_id( std::string requestline );
		std::vector<_locations> 		get_locations() const;
		std::string			get_location_root(ID id);
	
	private:
		std::vector<_locations> arr;
		std::string				_req;
		getLocation(const getLocation& src);
		getLocation();
		getLocation &operator=(const getLocation &rhs);
		bool _tildeisfound(std::string loc, bool isCaseSensitive);
		std::string	strtoupper(std::string str);

};

std::ostream &operator<<(std::ostream & o, const getLocation& src);
#endif