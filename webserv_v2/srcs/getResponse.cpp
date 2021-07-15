#include "./../includes/getResponse.hpp"

/*

░█████╗░██╗░░░░░░█████╗░░██████╗░██████╗  ██╗░░░██╗████████╗██╗██╗░░░░░░██████╗
██╔══██╗██║░░░░░██╔══██╗██╔════╝██╔════╝  ██║░░░██║╚══██╔══╝██║██║░░░░░██╔════╝
██║░░╚═╝██║░░░░░███████║╚█████╗░╚█████╗░  ██║░░░██║░░░██║░░░██║██║░░░░░╚█████╗░
██║░░██╗██║░░░░░██╔══██║░╚═══██╗░╚═══██╗  ██║░░░██║░░░██║░░░██║██║░░░░░░╚═══██╗
╚█████╔╝███████╗██║░░██║██████╔╝██████╔╝  ╚██████╔╝░░░██║░░░██║███████╗██████╔╝
░╚════╝░╚══════╝╚═╝░░╚═╝╚═════╝░╚═════╝░  ░╚═════╝░░░░╚═╝░░░╚═╝╚══════╝╚═════╝░
*/

getResponse::getResponse( getRequest const &request ) : _request(request) {
	this->_status_code = _parse_status_line();
	if (this->_status_code == 200) {
		if (!this->_request["method"].compare("GET"))
			_content = _method_get();
		else if  (!this->_request["method"].compare("POST"))
			_content = _method_post();
		else if (this->_request["method"].compare("DELETE"))
			_content = _method_delete();
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

getResponse & getResponse::operator=( getResponse const & rhs ) {
	// put your equality here eg.: this->_xx = rhs._xx
	if (this != &rhs) {
		this->_request = rhs._request;
		this->_keys= rhs._keys;
		this->_status_code = rhs._status_code;
	}
	return (*this);
}

void			getResponse::set_status_code(int status) {
	this->_status_code   = status;
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

	//remove(_request["body"].c_str());
	str.reserve(30);
	if (!this->_status_code)
		return this->_content;/////////////////////
	ss << this->_status_code;
	ss >> str;
	str.insert(0, "HTTP/1.1 ");
	str+= " ";
	str += err.find(this->_status_code)->second;
	str += "\r\n";
	///////////////////// a refaire
	if (this->_status_code >= 200 && this->_status_code < 300)
		str += this->_content;
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


std::string	getResponse::_error_response(const std::map<int, std::string> err) {
	std::string ret;
	size_t f = 0;
	std::stringstream ss;
	// si pas de fichier personalisé
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

#define PHP_CONTENT "./tmp/php_content"
std::string getResponse::_method_get( void )
{
	std::string location = CURRDIR + this->_request["request-target"];
	bool	isindex = false;
	std::string response_body;
	std::ifstream ifs;

	response_body.reserve(10000);
	if (*(this->_request["request-target"].end() - 1 ) == '/') {
		location += PAGE;
		isindex = true;
	}
	else if (!_get_extension().compare("php") || _request["request-target"].find(".php?") != std::string::npos)
	{
		// if cgi is on
		CGI cgi(_request, "8080", ROOT);
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
		if (isindex)
		{
			response_body += _get_autoindex(CURRDIR + this->_request["request-target"]);
			return _get_fill_headers(response_body);
		}
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


// std::string reformPHP(std::string response)
// {
// 	size_t headend = response.find("\r\n\r\n");
// 	headend = headend == std::string::npos? 0 : headend + 4;

// 	std::string body = response.substr(headend);

// }

// ptet revoir la gestion des headers via une map ???????????
std::string	getResponse::_get_fill_headers( std::string response ) {
	std::string headers;
	std::stringstream ss;
	std::string ext = _get_extension();
	if (ext.empty() || (this->_status_code < 200 && this->_status_code > 299))
		ext = "html";
	// TODO ajouter serv name
	//headers += _get_date_line();
	if (!ext.compare("php") || _request["request-target"].find(".php?") != std::string::npos /* et cgi on */) {
		headers += "Content-Length: ";
		size_t headend = response.find("\r\n\r\n");
		headend = headend == std::string::npos? 0 : headend + 4;
		//std::cout << "SIZE = " << response.size() << "HEADEND= " << headend;
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
	headers += "\nContent-Length: ";
	ss << response.size();
	headers += ss.str();
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
			ret.append((*it).name);
			ret.append("/</a>");
			ret.append((*it).spaceL - 1, ' ');
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
			ret.append((*it).name);
			ret.append("</a>");
			ret.append((*it).spaceL, ' ');
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
			std::string path = ROOT + this->_request["request-target"];
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
		std::cout << "Errrr" << std::endl; //////////////////////TODO return error 404 si page non existante ???????????????
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
	if (!ext.compare("php")) //TODO ajouter l'extension qui recupere les CGI actifs (extensions dynamiques)
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

std::string getResponse::_delete_fill_header( void ) {
	std::string ret;

	//TODO ret += header du nom du server
	ret += _get_date_line();
	return ret;
}

int			getResponse::_delete_file( void ) {
	const std::string path = ROOT + _request["request-target"];
	if ( remove(path.c_str()) )
		return 404;
	else
		return 204;
}

std::string getResponse::_method_delete( void )
{
	//TODO if delete enabled (sinon redirect vers get) return _method_get(); ???? a retester te refaire
	if ((this->_status_code = _delete_file()) == 404)
		return _method_get();
	else
		return _delete_fill_header();
}


/*
 TODO- structure par location : boucle qui cherche la location la plus proche de l'URI demandée et qui adapte la reponse selon les options de la config
 	 - GET
 TODO	** faire une boucle qui choppe le bon index avec les differents index renseignés dans la config (par defaut index.html)
 	 - POST
 TODO	** POST si vers les ext gerées par cgi
 TODO	** POST si autre que les ext gerées par un cgi
 	 - DELETE
 TODO	** Renvoie GET si non autorisée explicitement
 TODO	** supprime si explicitement autorisée dans un dossier precis
 TODO- FORBIDDEN METHOD (explicite dans la config) + toutes les methodes qui ne sont pas a faire
 TODO- constructeur par erreur 400 ( qui prend un int uniquement ) pour preparer la reponse directe en cas de premiere ligne fausse (lecture ligne par ligne via telnet)
 TODO- page d'erreur par defaut
*/

/* METHODES AUTORISEES
 - All HTTP protocol compliance checks are enabled by default except for GET with body and POST without body. 
*/
