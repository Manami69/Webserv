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
		std::string						getRoot();
		bool							isAllowedMethod(int method);
		std::string						getRedirection();
		bool							getAutoindex();
		std::string						getIndex();
		std::string						getCGIPath();

	
	private:
		//std::vector<_locations> arr;
		_locations			_infos;
		std::string			_req;
		ID					get_id(std::vector<_locations> & arr);
		getLocation(const getLocation& src);
		getLocation();
		getLocation &operator=(const getLocation &rhs);
		bool _tildeisfound(std::string loc, bool isCaseSensitive);
		std::string	strtoupper(std::string str);

};

#endif