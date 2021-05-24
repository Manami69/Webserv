#include "getResponse.hpp"


getResponse::getResponse( getRequest const &request ) : _request(request) {
	this->_status_code = _parse_status_line();
	if (this->_status_code == 200)
		;// continue checking with headers;
	return ;
}

getResponse::getResponse( getResponse const & src ) {
	*this = src;
	return ;
}

getResponse::~getResponse( void ) {
	return ;
}

std::string getResponse::responsetosend(const std::map<int, std::string> err) {
	std::string str;
	std::stringstream ss;

	if (!this->_status_code)
		return CONTENT;
	ss << this->_status_code;
	ss >> str;
	str.insert(0, "HTTP/1.1 ");
	str+= " ";
	str += err.find(this->_status_code)->second;;
	str += CRLF;
	///////////////////// a refaire
	if (this->_status_code == 200)
		str += CONTENT;
	else
	{
		str += ERR + ss.str() + "\n";
	}
	return str;
}

getResponse & getResponse::operator=( getResponse const & rhs ) {
	// put your equality here eg.: this->_xx = rhs._xx
	if (this != &rhs) {
		this->_request = rhs._request;
		this->_keys= rhs._keys;
		this->_status_code = rhs._status_code;
	}
	return (*this);
}

/*
** Parse la premiere ligne  a la recherche d'une bonne methode / localisation / http-version
** ATTENTION EBAUCHE CAR COMPORTEMENTS ENCORE A L'ETUDE
*/

int getResponse::_parse_status_line( void )
{
	const std::string method[] = { "GET","HEAD", "POST", "PUT", "DELETE","CONNECT", "OPTIONS", "TRACE" };
	std::vector<std::string> array(method, method + sizeof(method)/ sizeof(std::string));
	std::string http = this->_request["http-version"];
	// check method
	if (std::find(array.begin(), array.end(), this->_request["method"]) == array.end())
		return 400;
	if (!this->_request["method"].compare("GET") && this->_request["http-version"].empty())
		return 0; // ne renvoie que le contenu de la page donnee par localisation (404 si elle n'existe pas)
	// check localisation (y a t'il un truc a checker ?)
	/////////////////////////////////////////////////////////////////////////
	// check http-version
	if (http.compare(0, 5, "HTTP/") || http.size() <= 5)
		return 400; // si different alors 400
	if (!http.compare(5, 2, "1.") && atoi(http.substr(7).c_str()) < 1000)
		return 200; // si HTTP/1.x avec atoi(x) < 1000 alors c'est bon
	else if (atof(http.substr(5).c_str()) < 2)
		return 400;
	else if (atof(http.substr(5).c_str()) >= 2)
		return 505;
	else
		return 400;
}