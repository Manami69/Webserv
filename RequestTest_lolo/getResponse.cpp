#include "getResponse.hpp"

getResponse::getResponse( getRequest const &request ) : _request(request) {
	_create_keys_map();
	return ;
}

getResponse::getResponse( getResponse const & src ) {
	*this = src;
	return ;
}

getResponse::~getResponse( void ) {
	return ;
}

getResponse & getResponse::operator=( getResponse const & rhs ) {
	// put your equality here eg.: this->_xx = rhs._xx
	if (this != &rhs) {
		this->_request = rhs._request;
		this->_res_token = rhs._res_token;
		this->_keys= rhs._keys;
	
	}
	return (*this);
}

void  getResponse::_create_keys_map( void ) {
	const std::string array[] = {
		"http-version", "status-code", "reason-phrase"
	};
	this->_keys = std::vector<std::string>(array, array + sizeof(array)/ sizeof(std::string));
	for (size_t i = 0; i < sizeof(array)/sizeof(std::string) ; i++)
		this->_res_token[array[i]]= "";
}

/*
** Parse la premiere ligne  a la recherche d'une bonne methode / localisation / http-version
** ATTENTION EBAUCHE CAR COMPORTEMENTS ENCORE A L'ETUDE
*/

bool getResponse::_parse_status_line( void )
{
	const std::string method[] = { "GET","HEAD", "POST", "PUT", "DELETE","CONNECT", "OPTIONS", "TRACE" };
	std::vector<std::string> array(method, method + sizeof(method)/ sizeof(std::string));

	if (std::find(array.begin(), array.end(), this->_request["method"]) == array.end())
		return false;
	// localisation (y a t'il un truc a checker ?)
	if (this->_request["http-version"] != "HTTP/1.1")
		return false; // parfois avec telnet la status line n'apparait pas // a etudier au regard du testeur et de la RFC	
}