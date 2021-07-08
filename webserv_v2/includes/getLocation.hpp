#ifndef GETLOCATION_HPP
# define GETLOCATION_HPP

#include "getResponse.hpp"
#define ID unsigned int
/*
struct						_locations
{
	std::string					modifier; // gere ici
	std::string					access; // ici
	bool						allowm; // pour les erreurs de conf
	bool						limitm; // conf
	int							allow_methods[3]; // OK lu ici et recup par requete
	std::string					root; //OK a recuperer par requete
	std::string					index; // a recuperer par requete si lecation finit par /
	bool						autoindex; // bool a recuperer
	std::string					cgi_path; // string a recuperer par requete
	std::string					try_files; // OSEF
	std::map<std::string, std::string>	redirect; //OK recuperer le premier
};
*/
class getLocation {
	public:
		getLocation(const Serv_config &s, const std::string& r);
		~getLocation();
		ID					get_id();
		std::vector<_locations> 		get_locations() const;
		std::string						getRoot(ID id);
		bool							isAllowedMethod(ID id, int method);
		std::string						getRedirection(ID id);
		bool							getAutoindex(ID id);
		std::string						getIndex(ID id);
		std::string						getCGIPath(ID id);

	
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