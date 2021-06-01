#include "getResponse.hpp"


getResponse::getResponse( getRequest const &request ) : _request(request) {
	this->_status_code = _parse_status_line();
	if (this->_status_code == 200) {
		if (!this->_request["method"].compare("GET"))
			_content = _method_get();
		else
			_content = "nono";
	}
	// continue checking with headers;
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
		return "pouet";
	ss << this->_status_code;
	ss >> str;
	str.insert(0, "HTTP/1.1 ");
	str+= " ";
	str += err.find(this->_status_code)->second;;
	str += CRLF;
	///////////////////// a refaire
	if (this->_status_code == 200 || this->_status_code == 404)
		str += this->_content;
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
	if (this->_request["request-target"][0] != '/')
		return (400);
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


std::string getResponse::_method_get( void )
{
	std::string location = CURRDIR + this->_request["request-target"];
	std::string response_body;
	std::ifstream ifs;

	response_body.reserve(10000);
	if (this->_request["request-target"].size() == 1)
		location += PAGE;
	ifs.open(location.c_str(), std::ifstream::in);
	if (ifs.fail()) {
		ifs.clear();
		ifs.open(ERROR404, std::ifstream::in);
		this->_status_code = 404;
	}
	char c;
	while (ifs.get(c))          // loop getting single characters
    	response_body += c;
	return _get_fill_headers(response_body);
}

std::string	getResponse::_get_fill_headers( std::string response ) {
	std::string headers;
	std::stringstream ss;
	std::string ext = _get_extension();
	if (ext.empty() || this->_status_code == 404)
		ext = "html";
	// ajouter date et autre
	// CONTENT-TYPE https://www.iana.org/assignments/media-types/media-types.xhtml ;
	headers += "Content-Type: ";
	std::ifstream is;
	is.open("MIMEtypes");
	char buf[300];
	std::string buffer;
	while (buffer.find(ext) == std::string::npos && buffer.compare("END"))
	{
		is.getline(buf, 300);
		buffer = buf;
	}
	if (!buffer.compare("END")) {
		headers += "text/plain";
	}
	else
	{
		headers.append(buffer, buffer.find(" ") + 1, buffer.size() - buffer.find(" ") + 1);
	}
	is.close();
	headers += "\nContent-Length: ";
	ss << response.size();
	headers += ss.str();
	headers += "\n\n";
	std::cout << YEL << headers << END << std::endl;
	headers += response;
	return headers;
}

std::string getResponse::_get_extension( void ) {
	std::string loc = this->_request["request-target"];
	size_t i = loc.size() - 1;
	std::string ext;
	if (loc[i] == '/')
		return "";
	while (loc[i] != '/' && loc[i] != '.')
		i--;
	if (loc[i] == '/')
		return "";
	else
	{	
		i++;
		for (; i != loc.size(); ++i)
			ext += tolower(loc[i]);
	}
	return ext;
}