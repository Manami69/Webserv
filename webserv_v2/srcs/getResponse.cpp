#include "./../includes/getResponse.hpp"

/*

░█████╗░██╗░░░░░░█████╗░░██████╗░██████╗  ██╗░░░██╗████████╗██╗██╗░░░░░░██████╗
██╔══██╗██║░░░░░██╔══██╗██╔════╝██╔════╝  ██║░░░██║╚══██╔══╝██║██║░░░░░██╔════╝
██║░░╚═╝██║░░░░░███████║╚█████╗░╚█████╗░  ██║░░░██║░░░██║░░░██║██║░░░░░╚█████╗░
██║░░██╗██║░░░░░██╔══██║░╚═══██╗░╚═══██╗  ██║░░░██║░░░██║░░░██║██║░░░░░░╚═══██╗
╚█████╔╝███████╗██║░░██║██████╔╝██████╔╝  ╚██████╔╝░░░██║░░░██║███████╗██████╔╝
░╚════╝░╚══════╝╚═╝░░╚═╝╚═════╝░╚═════╝░  ░╚═════╝░░░░╚═╝░░░╚═╝╚══════╝╚═════╝░
*/

getResponse::getResponse( getRequest const &request, Serv_config conf) : _request(request), _conf(conf), isloc(false) {
	this->_status_code = _parse_status_line();// verifie la status line
	// continue checking with headers;
	if (this->_status_code == 200) {
		size_t mark;
		if ((mark = this->_request["request-target"].find("?")) == NOTFOUND) // gere les GET avec infos
		{
			_locInfos = new getLocation(_conf, this->_request["request-target"]);
		}
		else {
			std::cout << this->_request["request-target"].substr(0, mark) << " " << mark << std::endl;
			_locInfos = new getLocation(_conf, this->_request["request-target"].substr(0, mark));
		}
		isloc = true;

		if (!this->_request["body_size"].empty() && static_cast<size_t>(atoi(this->_request["body_size"].c_str())) > _conf.client_max_body_size)
		{
			this->_status_code = 413;
			return ;
		}
		if (!_locInfos->getRedirection().empty()) {
			_status_code = atoi(_locInfos->getRedirection().c_str());
			return ;
		}
		if (!this->_request["method"].compare("GET"))
			_content = _method_get();
		else if  (!this->_request["method"].compare("POST"))
			_content = _method_post();
		else if (this->_request["method"].compare("DELETE"))
			_content = _method_delete();
	}
	return ;
}

getResponse::getResponse( getResponse const & src ) {
	*this = src;
	return ;
}

getResponse::~getResponse( void ) {
	if (isloc)
		delete _locInfos;
	return ;
}

getResponse & getResponse::operator=( getResponse const & rhs ) {
	// put your equality here eg.: this->_xx = rhs._xx
	if (this != &rhs) {
		this->_request = rhs._request;
		this->_conf = rhs._conf;
		this->_keys= rhs._keys;
		this->_status_code = rhs._status_code;
	}
	return (*this);
}

void			getResponse::set_status_code(int status) {
	if (this->_status_code == 200)
		this->_status_code  = status;
}


/*
██████╗░██╗░░░██╗██████╗░██╗░░░░░██╗░█████╗░  ███████╗████████╗
██╔══██╗██║░░░██║██╔══██╗██║░░░░░██║██╔══██╗  ██╔════╝╚══██╔══╝
██████╔╝██║░░░██║██████╦╝██║░░░░░██║██║░░╚═╝  █████╗░░░░░██║░░░
██╔═══╝░██║░░░██║██╔══██╗██║░░░░░██║██║░░██╗  ██╔══╝░░░░░██║░░░
██║░░░░░╚██████╔╝██████╦╝███████╗██║╚█████╔╝  ██║░░░░░░░░██║░░░
╚═╝░░░░░░╚═════╝░╚═════╝░╚══════╝╚═╝░╚════╝░  ╚═╝░░░░░░░░╚═╝░░░
*/

std::string getResponse::responsetosend(const std::map<int, std::string> err) {
	std::string str;
	std::stringstream ss;
	const int	error_code[] = { 100, 101, 102, 200 , 201 , 202 , 203 , 204 , 205 , 206 , 207 , 208 , 226 , 300 , 301\
, 302 , 303 , 304 , 305 , 307 , 308 , 400 , 401 , 402 , 403 , 404 , 405 , 406 , 407 , 408 , 409 , 410 , 411\
, 412 , 413 , 414 , 415 , 416 , 417 , 418 , 421 , 422 , 423 , 424 , 426 , 428 , 429 , 431 , 444 , 451 , 499\
, 500 , 501 , 502 , 503 , 504 , 505 , 506 , 507 , 508 , 510 , 511 , 599};
	std::vector<int> errcode(error_code, error_code + sizeof(error_code)/ sizeof(int));
	remove(_request["body"].c_str());
	if (!_request["body"].compare(PHP_CONTENT))
		remove(PHP_CONTENT);
	if (this->_status_code >= 300 && this->_status_code < 600 && !_conf.error_page[this->_status_code].empty())
		return _redirectError();
	str.reserve(30);
	if (!this->_status_code)
		return this->_content;/////////////////////
	ss << this->_status_code;
	ss >> str;
	str.insert(0, "HTTP/1.1 ");
	str+= " ";
	str += err.find(this->_status_code)->second;
	str += "\r\n";
	if (this->_status_code >= 200 && _status_code < 204)
		str += this->_content;
	else if (this->_status_code >= 300 && this->_status_code < 400)
		str += _returnRedir();
	else if (std::find(errcode.begin(), errcode.end(), _status_code) == errcode.end()) 
		str += _get_fill_headers(""); ///???
	else
		str += _error_response(err);
	return str;
}


/*
██╗░░░██╗████████╗██╗██╗░░░░░░██████╗
██║░░░██║╚══██╔══╝██║██║░░░░░██╔════╝
██║░░░██║░░░██║░░░██║██║░░░░░╚█████╗░
██║░░░██║░░░██║░░░██║██║░░░░░░╚═══██╗
╚██████╔╝░░░██║░░░██║███████╗██████╔╝
░╚═════╝░░░░╚═╝░░░╚═╝╚══════╝╚═════╝░
*/


/*
** Parse la premiere ligne  a la recherche d'une bonne methode / localisation / http-version
** ATTENTION EBAUCHE CAR COMPORTEMENTS ENCORE A L'ETUDE
*/

int getResponse::_parse_status_line( void )
{
	const std::string method[] = { "GET", "POST", "DELETE", "HEAD", "PUT", "CONNECT", "OPTIONS", "TRACE" };
	std::vector<std::string> array(method, method + sizeof(method)/ sizeof(std::string));
	std::string http = this->_request["http-version"];
	// check method
	if (std::find(array.begin(), array.end(), this->_request["method"]) == array.end())
		return 400;
	if (std::find(array.begin(), array.begin() + 3, this->_request["method"]) == array.begin() + 3)
		return 405;
	if (!this->_request["method"].compare("GET") && this->_request["http-version"].empty())
		return 0; // ne renvoie que le contenu de la page donnee par localisation (404 si elle n'existe pas)
	/////////////////////////////////////////////////////////////////////////
	if (this->_request["request-target"][0] != '/')
		return (400);
	// check http-version
	if (http.compare(0, 5, "HTTP/") || http.size() <= 5)
		return 400; // si different alors 400
	// if (this->_request.getKeyValue("Host").empty())
	// 	return 400;
	if (!http.compare(5, 2, "1.") && atoi(http.substr(7).c_str()) < 1000)
		return 200; // si HTTP/1.x avec atoi(x) < 1000 alors c'est bon
	else if (atof(http.substr(5).c_str()) < 2)
		return 400;
	else if (atof(http.substr(5).c_str()) >= 2)
		return 505;
	else
		return 400;
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

std::string getResponse::_get_MIMEtype(const std::string &ext) {
	std::ifstream is;
	std::string ret;
	is.open("MIMEtypes");
	char buf[300];
	std::string buffer;
	while (buffer.find(ext) == std::string::npos && buffer.compare("END"))
	{
		is.getline(buf, 300);
		buffer = buf;
	}
	if (!buffer.compare("END")) {
		ret = "text/plain";
	}
	else
	{
		ret.append(buffer, buffer.find(" ") + 1, buffer.size() - buffer.find(" ") + 1);
	}
	is.close();
	return ret;
}

std::string	getResponse::_get_date_line( void ) {
	std::string ret = "Date: ";
	struct timeval time;
	gettimeofday(&time, NULL);
	// Date: Tue, 15 Jun 2021 13:35:03 GMT

	char strNow[32];
	strftime(strNow, 32, "%a, %d %b %Y %X GMT\r\n", gmtime( &time.tv_sec ));
	ret += strNow;
	return (ret);
}

std::string getResponse::_get_serv_line( void ) {
	return "Server: Webserv/v1.0\r\n";
}

std::string	getResponse::_redirectError() {
	std::string reloc;
	std::string resp;
	resp.reserve(500);
	size_t pos = 0;
	if (_conf.error_page[this->_status_code][0] == '=' && (pos = _conf.error_page[this->_status_code].find(" ")) != NOTFOUND)
			reloc = _conf.error_page[this->_status_code].substr(pos + 1);
	else
		reloc = _conf.error_page[this->_status_code];
	resp += "HTTP/1.1 302 Moved Temporarily\r\n";
	resp += _get_serv_line();
	resp += _get_date_line();
	resp += "Connection: keep-alive\r\nLocation: ";
	resp += reloc + CRLF + CRLF;
	return resp;
}

std::string	getResponse::_returnRedir() {
	std::string reloc;
	std::string resp;
	resp.reserve(500);
	size_t pos = 0;
	if ((pos = _locInfos->getRedirection().find(" ")) == NOTFOUND || pos == _locInfos->getRedirection().size() - 1) // si egal a "301 "
		reloc = "";
	else
		reloc =  _locInfos->getRedirection().substr(pos + 1);
	resp += _get_serv_line();
	resp += _get_date_line();
	resp += "Connection: keep-alive\r\nLocation: ";
	resp += reloc + CRLF + CRLF;
	return resp;
}

std::string	getResponse::_error_response(const std::map<int, std::string> err) {
	std::string ret;
	size_t f = 0;
	std::stringstream ss;

	ret.reserve(200);
	ret += "<html>\n<head><title>xx yy</title></head>\n<body>\n\
<center><h1>xx yy</h1></center>\n<hr><center>Webserv/1.0</center>\n\
</body>\n</html>";
	ss << this->_status_code;
	while ((f = ret.find("xx")) != std::string::npos)
		ret.replace(f, 2, ss.str());
	while ((f = ret.find("yy")) != std::string::npos)
		ret.replace(f, 2, err.find(this->_status_code)->second);
	return _get_fill_headers(ret);
}

/*
░██████╗░███████╗████████╗  ███╗░░░███╗███████╗████████╗██╗░░██╗░█████╗░██████╗░
██╔════╝░██╔════╝╚══██╔══╝  ████╗░████║██╔════╝╚══██╔══╝██║░░██║██╔══██╗██╔══██╗
██║░░██╗░█████╗░░░░░██║░░░  ██╔████╔██║█████╗░░░░░██║░░░███████║██║░░██║██║░░██║
██║░░╚██╗██╔══╝░░░░░██║░░░  ██║╚██╔╝██║██╔══╝░░░░░██║░░░██╔══██║██║░░██║██║░░██║
╚██████╔╝███████╗░░░██║░░░  ██║░╚═╝░██║███████╗░░░██║░░░██║░░██║╚█████╔╝██████╔╝
░╚═════╝░╚══════╝░░░╚═╝░░░  ╚═╝░░░░░╚═╝╚══════╝░░░╚═╝░░░╚═╝░░╚═╝░╚════╝░╚═════╝░
*/

std::string		getResponse::_findIndex() {
	std::string test;
	size_t i = 0;
	size_t last = 0;
	std::cout << _locInfos->getIndex() << std::endl;
	if (_locInfos->getIndex().empty())
		return "";
	while ((i = _locInfos->getIndex().find(" ", last)) != NOTFOUND)
	{
		test = _locInfos->getIndex().substr(last, i);
		if (test[0] != '/')
			test.insert(0, "/");
		std::cout << _locInfos->getRoot() + test << std::endl;
		if (test.compare("/") != 0 && _fileExists(_locInfos->getRoot() + test))
			return test;
		last = i + 1;
	}
	test = _locInfos->getIndex().substr(last);
	if (test[0] != '/')
		test.insert(0, "/");
	if (test.compare("/") != 0 && _fileExists(_locInfos->getRoot() + test))
		return test;
	return "";
}

bool		getResponse::_fileExists(std::string fileStr)
{
	std::fstream fs;

	fs.open(fileStr.c_str(), std::fstream::in);
	if (fs.is_open())
	{
		fs.close();
		return true;
	}
	else
		return false;
}


std::string getResponse::_method_get( void )
{
	std::string location = _locInfos->getRoot() + this->_request["request-target"];
	std::string	index;
	std::string response_body;
	std::ifstream ifs;

	response_body.reserve(10000);
	if (*(this->_request["request-target"].end() - 1 ) == '/') {
		std::cout << "OKOK<<" << std::endl;
		if ((index = _findIndex()).empty())
		{	
			std::cout << index + " isindex "  <<_locInfos->getAutoindex() << std::endl;
			if (_locInfos->getAutoindex()) {
				response_body += _get_autoindex(_locInfos->getRoot() + this->_request["request-target"]);
				std::cout << "HEYHEY  " << response_body << std::endl ;
				return _get_fill_headers(response_body);
			}
			else {
				_status_code = 404;
				return "";
			}
		}
		location += index;
	}
	if (!_locInfos->getCGIPath().empty())
	{
		CGI cgi(_request, _conf.port, _locInfos->getRoot(), _locInfos->getCGIPath());
		try {
			cgi.cgi_exec();
		}
		catch (std::exception &e)
		{
			this->_status_code = 503;
			return "";
		}
		location = PHP_CONTENT;
	}
	ifs.open(location.c_str(), std::ifstream::in);
	if (ifs.fail()) {
		ifs.clear();
		this->_status_code = 404;
		return "";
	}
	char c;
	while (ifs.get(c))          // loop getting single characters
    	response_body += c;
	ifs.close();
	std::cout << response_body << std::endl;
	return _get_fill_headers(response_body);
}


std::string	getResponse::_get_fill_headers( std::string response ) {
	std::string headers;
	std::stringstream ss;
	std::string ext = _get_extension();
	if (ext.empty() || (this->_status_code < 200 || this->_status_code > 299))
		ext = "html";
	headers += _get_serv_line();
	headers += _get_date_line();
	if ((this->_status_code <= 200 && this->_status_code < 300) && !_locInfos->getCGIPath().empty()) {
		std::cout << "pouet" << std::endl;
		headers += "Content-Length: ";
		size_t headend = response.find("\r\n\r\n");
		headend = headend == std::string::npos? 0 : headend + 4;
		ss << response.size() - headend;
		headers += ss.str();
		if (response.find("\r\n\r\n") != response.npos)
			headers += "\r\n";
		else
			headers += "\r\n\r\n";
		headers += response;
		std::cout << RED << headers << END << std::endl;

		return headers;
	}
	headers += "Content-Type: ";
	headers += _get_MIMEtype(ext);
	if (!response.empty()) {
		headers += "\nContent-Length: ";
		ss << response.size();
		headers += ss.str();
	}
	if (response.find("\r\n\r\n") != response.npos)
		headers += "\r\n";
	else
		headers += "\r\n\r\n";
	headers += response;
	return headers;
}


#define INDEX_BODY "<html>\n\
<head><title>Index of "
#define INDEX2 "</title></head>\n\
<body bgcolor=\"white\">\n\
<h1>Index of "
#define INDEX3 "</h1><hr><pre><a href=\"../\">../</a>\n"
#define INDEXEND "</pre><hr></body>\n\
</html>"
#define LOC "/"

std::string getResponse::_fill_index_body(std::list<t_index_file> files) // TODO Si + grand que 50 char
{
	std::string ret;
	ret.reserve(1024);
	ret.append(INDEX_BODY);
	ret.append(this->_request["request-target"]);
	ret.append(INDEX2);
	ret.append(this->_request["request-target"]);
	ret.append(INDEX3);
	for (std::list<t_index_file>::iterator it = files.begin(); it != files.end(); it++) {
		if ((*it).type == static_cast<unsigned char>(4) && (*it).name.compare(".") != 0 && (*it).name.compare("..") != 0)
		{
			std::stringstream ss;
			ret.append("<a href=\"");
			ret.append((*it).name);
			ret.append("/\">");
			if ((*it).name.size() < 50) {
				ret.append((*it).name);
				ret.append("/</a>");
				ret.append((*it).spaceL - 1, ' ');
			}
			else if ((*it).name.size() == 50) {
				ret.append((*it).name);
				ret.append("</a> ");
			}
			else {
				ret.append(it->name.substr(0, 47));
				ret.append("..></a> ");
			}
			ret.append((*it).date);
			ss << std::setfill(' ') << std::setw(20);
			ss << "-\n";
			ret.append(ss.str());
		}
	}
	for (std::list<t_index_file>::iterator it = files.begin(); it != files.end(); it++) {
		if ((*it).type == static_cast<unsigned char>(8))
		{
			std::stringstream ss;
			ret.append("<a href=\"");
			ret.append((*it).name);
			ret.append("\">");
			if ((*it).name.size() < 51) {
				ret.append((*it).name);
				ret.append("</a>");
				ret.append((*it).spaceL, ' ');
			}
			else {
				ret.append(it->name.substr(0, 47));
				ret.append("..></a> ");
			}
			ret.append((*it).date);
			ss << std::setfill(' ') << std::setw(20);
			ss << (*it).size << "\n";
			ret.append(ss.str());
		}
	}
	ret.append(INDEXEND);
	return ret;
}

std::string getResponse::_get_autoindex( std::string location ) {
	DIR *dir;
	t_index_file cur;
	std::list<t_index_file> files;
	struct dirent	*ent;
	struct stat		st;

	if ((dir = opendir(location.c_str())) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			memset(&st, 0, sizeof(st));
			std::string path = _locInfos->getRoot() + this->_request["request-target"];
			path += ent->d_name;
			stat(path.c_str(), &st);
			char strNow[ 19 ];
    		strftime(strNow, 19, "%d-%b-%Y %H:%M", gmtime( &st.st_mtim.tv_sec ));
			cur.name = ent->d_name;
			cur.spaceL = 51 - cur.name.size();
			cur.date = strNow;
			cur.size = st.st_size;
			cur.type = ent->d_type;
			files.push_back(cur);
  		}
  		closedir (dir);
	}
	else {
		_status_code = 500;
		return "";
	}
	return _fill_index_body(files);
}

/*
██████╗░░█████╗░░██████╗████████╗  ███╗░░░███╗███████╗████████╗██╗░░██╗░█████╗░██████╗░
██╔══██╗██╔══██╗██╔════╝╚══██╔══╝  ████╗░████║██╔════╝╚══██╔══╝██║░░██║██╔══██╗██╔══██╗
██████╔╝██║░░██║╚█████╗░░░░██║░░░  ██╔████╔██║█████╗░░░░░██║░░░███████║██║░░██║██║░░██║
██╔═══╝░██║░░██║░╚═══██╗░░░██║░░░  ██║╚██╔╝██║██╔══╝░░░░░██║░░░██╔══██║██║░░██║██║░░██║
██║░░░░░╚█████╔╝██████╔╝░░░██║░░░  ██║░╚═╝░██║███████╗░░░██║░░░██║░░██║╚█████╔╝██████╔╝
╚═╝░░░░░░╚════╝░╚═════╝░░░░╚═╝░░░  ╚═╝░░░░░╚═╝╚══════╝░░░╚═╝░░░╚═╝░░╚═╝░╚════╝░╚═════╝░
*/

std::string	getResponse::_method_post( void ) {
	// si activé mais pas vers une page statique >> vers get avec un corps
	const std::string ext = _get_extension();
	if (!_locInfos->getCGIPath().empty() && _locInfos->isAllowedMethod(POST)) //TODO ajouter l'extension qui recupere les CGI actifs (extensions dynamiques)
	{
		return _method_get();
	}
	else {
		this->_status_code = 405;
		return "";
	}
	return ""; //
}

/*
██████╗░███████╗██╗░░░░░███████╗████████╗███████╗
██╔══██╗██╔════╝██║░░░░░██╔════╝╚══██╔══╝██╔════╝
██║░░██║█████╗░░██║░░░░░█████╗░░░░░██║░░░█████╗░░
██║░░██║██╔══╝░░██║░░░░░██╔══╝░░░░░██║░░░██╔══╝░░
██████╔╝███████╗███████╗███████╗░░░██║░░░███████╗
╚═════╝░╚══════╝╚══════╝╚══════╝░░░╚═╝░░░╚══════╝

███╗░░░███╗███████╗████████╗██╗░░██╗░█████╗░██████╗░
████╗░████║██╔════╝╚══██╔══╝██║░░██║██╔══██╗██╔══██╗
██╔████╔██║█████╗░░░░░██║░░░███████║██║░░██║██║░░██║
██║╚██╔╝██║██╔══╝░░░░░██║░░░██╔══██║██║░░██║██║░░██║
██║░╚═╝░██║███████╗░░░██║░░░██║░░██║╚█████╔╝██████╔╝
╚═╝░░░░░╚═╝╚══════╝░░░╚═╝░░░╚═╝░░╚═╝░╚════╝░╚═════╝░
*/

// valide seulement si explicitement ecrit dans le location : sinon simple get

/*
> DELETE /chose.txt HTTP/1.1
> Host: localhost:8080
> User-Agent: curl/7.54.0
> Accept: *\/\*
> 
< HTTP/1.1 204 No Content
< Server: nginx/1.21.0
< Date: Tue, 15 Jun 2021 15:07:00 GMT
< Connection: keep-alive
*/


int			getResponse::_delete_file( void ) {
	const std::string path = _locInfos->getRoot() + _request["request-target"];
	if ( remove(path.c_str()) )
		return 404;
	else
		return 204;
}

std::string getResponse::_method_delete( void )
{
	//TODO if delete enabled (sinon redirect vers get) return _method_get(); ???? a retester te refaire
	if (!_locInfos->isAllowedMethod(DELETE))
	{
		_status_code = 405;
		return "";
	}
	if ((this->_status_code = _delete_file()) == 404)
		return _method_get();
	else
		return "";
}


/*
 	 - GET
 	 - POST
 	 - DELETE
 TODO- FORBIDDEN METHOD (explicite dans la config) + toutes les methodes qui ne sont pas a faire
 TODO- constructeur par erreur 400 ( qui prend un int uniquement ) pour preparer la reponse directe en cas de premiere ligne fausse (lecture ligne par ligne via telnet)
*/

/* METHODES AUTORISEES
 - All HTTP protocol compliance checks are enabled by default except for GET with body and POST without body. 
*/
