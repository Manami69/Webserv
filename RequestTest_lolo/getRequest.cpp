#include "getRequest.hpp"
/**
 * Les 3 premiers elements remplis correspondent a la premiere ligne de la requete et sont nommés comme
 * dans la RFC 7230 3.1.1 --- [method] [ ] [request-target] [ ] [http-version] [CRLF] ---
 * Les autres sont les headers imposés par le sujet, qu'on retrouvera ou non dans la requete
**/

const std::string getRequest::headers[] = {"method", "request-target", "http-version", \
	"Accept-Charsets", "Accept-Language", "Allow", "Authorization", "Content-Language", "Content-Length", "Content-Location", \
	"Content-Type", "Date", "Host", "Last-Modified", "Location", "Referer", "Retry-After", "Server", "Transfer-Encoding", "User-Agent", "WWW-Authenticate"};


getRequest::getRequest( void ) {
	_construct_empty_map();
}

getRequest::getRequest( std::string buffer ) {
	_construct_empty_map();
	//fill with informations
}

getRequest::getRequest( getRequest const & src ) {
	*this = src;
	return ;
}

getRequest::~getRequest( void ) {
	return ;
}

getRequest & getRequest::operator=( getRequest const & rhs ) {
	// put your equality here eg.: this->_xx = rhs._xx
	if (this != &rhs)
		this->_request_tokens = rhs._request_tokens;
	return (*this);
}


std::string&	getRequest::operator[] ( const std::string& key ) {
	return this->_request_tokens[key];
}

void			getRequest::fillRequest( std::string buffer ) {
	//voir et lister les conditions d'une requete valide avant meme de la tokeniser (au moins une premiere ligne)
	size_t i = 0;
	size_t last =
}

void			getRequest::setKeyValue( std::string key, std::string val ) {
	if (_is_used_key(key))
		this->_request_tokens[key] = val;
}

std::string		getRequest::getKeyValue( std::string key ) {
	if (_is_used_key(key))
		return this->_request_tokens[key];
	return "";
}

std::map<std::string, std::string>	getRequest::getMap( void ) const {
	return this->_request_tokens;
}


void	getRequest::_construct_empty_map( void ) {
	for (size_t i = 0; i < sizeof(headers)/sizeof(std::string); i++)
		this->_request_tokens[this->headers[i]] = "";
}

bool	getRequest::_is_used_key(std::string key) {
	std::vector<std::string> array(this->headers, this->headers + sizeof(this->headers)/ sizeof(std::string));
	return std::find(array.begin(), array.end(), key) != array.end();
}

bool	getRequest::_check_request(std::string request) {
	const std::string method[] = { "GET","HEAD", "POST", "PUT", "DELETE","CONNECT", "OPTIONS", "TRACE" };
	std::vector<std::string> array(method, method + sizeof(method)/ sizeof(std::string));
	size_t start = 0;
	size_t space;
	std::string token;

	if ((space = request.find(" ")) == std::string::npos)
		throw getRequest::BadRequestException();
	token = request.substr(start, space - 1 - start);
	
	
	
}

std::ostream & operator<<( std::ostream & o, getRequest const & rhs ) {
}
