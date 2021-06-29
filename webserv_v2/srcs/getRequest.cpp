#include "./../includes/getRequest.hpp"

/**
 * Les 3 premiers elements remplis correspondent a la premiere ligne de la requete et sont nommés comme
 * dans la RFC 7230 3.1.1 --- [method] [ ] [request-target] [ ] [http-version] [CRLF] ---
 * Les autres sont les headers imposés par le sujet, qu'on retrouvera ou non dans la requete
**/

const std::string getRequest::headers[] = {"method", "request-target", "http-version", \
	"Accept", "Accept-Encoding", "Accept-Charsets", "Accept-Language", "Allow", "Authorization", "Content-Language", "Content-Length", "Content-Location", \
	"Content-Type", "Date", "Host", "Last-Modified", "Location", "Referer", "Retry-After", "Server", "Transfer-Encoding", "User-Agent", "WWW-Authenticate", "body"};


getRequest::getRequest( void ) {
	_construct_array();
	_construct_empty_map();
}

getRequest::getRequest( std::string buffer ) {
	_construct_array();
	_construct_empty_map();
	//fill with informations
	fillRequest(buffer);
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

// const std::string&	getRequest::operator[] ( const std::string& key ) const {
// 	return this->getKeyValue(key);
// }
// TODO verifier si les headers sont case sensitive et agir en fonction

void			getRequest::fillRequest( std::string request ) {
	//voir et lister les conditions d'une requete valide avant meme de la tokeniser (au moins une premiere ligne)
	size_t start = 0;
	size_t space;
	std::string token;
	std::string	key;

	if ((space = request.find(" ")) == std::string::npos)
	{
		this->_request_tokens["method"] = request;
		return ;
	}
	token = request.substr(start, space - start);
	this->_request_tokens["method"] = token;
	start = space + 1;
	if ((space = request.find(" H", start)) == std::string::npos)
	{
		this->_request_tokens["request-target"] = request.substr(start);
		return ;
	}
	token = request.substr(start, space - start);
	this->_request_tokens["request-target"] = token;
	start = space + 1;
	if ((space = request.find(CRLF, start)) == std::string::npos)
	{
		this->_request_tokens["http-version"] = request.substr(start);
		return ;
	}
	token = request.substr(start, space - start);
	this->_request_tokens["http-version"] = token;
	start = space + 2;
	while ((space = request.find(": ", start)) != std::string::npos)
	{
		key = request.substr(start, space - start);
		start = space + 2;
		if ((space = request.find("\n", start)) == std::string::npos) //
		{
			this->_setKeyValueOnce(key, request.substr(start, request.size() - start));
			return ;
		}
		token = request.substr(start, space - start - 1);
		this->_setKeyValueOnce(key, token);
		start = space + 1;
	}
}

void			getRequest::_setKeyValueOnce( std::string key, std::string val ) {
	if (_is_used_key(key))
	{
		if (this->_request_tokens[key].empty())
			this->_request_tokens[key] = val;
	}
}

void			getRequest::setKeyValue( std::string key, std::string val ) {
	if (_is_used_key(key))
	{
		this->_request_tokens[key] = val;
	}
}

std::string		getRequest::getKeyValue( std::string key ) const {
	
	if (_is_used_key(key))
		return this->_request_tokens.find(key)->second;
	return std::string();
}

std::map<std::string, std::string>	getRequest::getMap( void ) const {
	return this->_request_tokens;
}

void	getRequest::fill_body(std::string request) {
	if (!this->_request_tokens["Content-Type"].empty() || !this->_request_tokens["Content-Length"].empty())
		_fill_body(request);
}
int		getRequest::get_content_length( void ) {
	if (getKeyValue("Transfer-Encoding").find("chunked") != std::string::npos)
		return -1;
	else if (!getKeyValue("Content-Length").empty())
	{
		std::stringstream ss;
		int ret = 0;
		ss << getKeyValue("Content-Length");
		ss >> ret;
		return ret;
	}
	else
		return 0;
}

void	getRequest::_construct_array( void ) {
	std::vector<std::string> array(this->headers, this->headers + sizeof(this->headers)/ sizeof(std::string));
	this->_array = array;
}

void	getRequest::_construct_empty_map( void ) {
	for (size_t i = 0; i < sizeof(headers)/sizeof(std::string); i++)
		this->_request_tokens[this->headers[i]] = "";
}

void	getRequest::_fill_body(std::string buffer) {
	size_t start;
	std::string filename;
	std::fstream fs;
	if ((start = buffer.find("\r\n\r\n")) != std::string::npos)
	{
		start += 4;
		try {
			this->_request_tokens["body"] = buffer.substr(start);
		}
		catch (const std::out_of_range& oor)
		{
			this->_request_tokens["body"] = "";
			return;
		}
		filename = _new_file();
		fs.open(filename.c_str(), std::fstream::out);
		fs << this->_request_tokens["body"];
		fs.close();
		this->_request_tokens["body"] = filename;
	}
}

std::string getRequest::_new_file()
{
	std::fstream fs;
	int		num = 0;
	std::stringstream ss;
	bool t = true;
	while (t)
	{
		ss.str("");
		ss << num;
		fs.open(("./tmp/body" + ss.str()).c_str(), std::fstream::in);
		t = fs.is_open();
		fs.close();
		num++;
	}
	return "./tmp/body" + ss.str();
}

bool	getRequest::_is_used_key(std::string key) const {
	return (std::find(this->_array.begin(), this->_array.end(), key) != this->_array.end());
}


std::ostream & operator<<( std::ostream & o, getRequest const & rhs ) {
	o << CYN <<rhs.getKeyValue("method") << " " << rhs.getKeyValue("request-target") << " " << rhs.getKeyValue("http-version") << std::endl;
	for (size_t i = 3; i < sizeof(rhs.headers) / sizeof(std::string); i++)
	{
		if (!rhs.getKeyValue(rhs.headers[i]).empty())
			o << rhs.headers[i] << ": " << rhs.getKeyValue(rhs.headers[i]) << std::endl;
	}
	o << END << std::endl;;
	return o;
}