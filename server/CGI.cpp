#include "CGI.hpp"
#include "getResponse.hpp"


const std::string CGI::arr[] = {"HTTP_CACHE_CONTROL", "HTTP_UPGRADE_INSECURE_REQUESTS", \
    "HTTP_CONNECTION", "HTTP_ORIGIN", "HTTP_CONTENT_LENGTH", "HTTP_CONTENT_TYPE" , "HTTP_REFERER", "HTTP_ACCEPT_ENCODING",\
    "HTTP_ACCEPT_LANGUAGE", "HTTP_ACCEPT", "HTTP_USER_AGENT", "HTTP_HOST", "REDIRECT_STATUS", "SERVER_NAME", "SERVER_PORT",\
    "SERVER_ADDR", "GATEWAY_INTERFACE", "REQUEST_SCHEME", "SERVER_PROTOCOL", "DOCUMENT_ROOT",\
    "DOCUMENT_URI", "REQUEST_URI", "SCRIPT_NAME", "CONTENT_LENGTH", "CONTENT_TYPE", "REQUEST_METHOD", "QUERY_STRING",\
    "SCRIPT_FILENAME", "PATH_TRANSLATED", "PATH_INFO", "FCGI_ROLE", "PHP_SELF"};

CGI::CGI( std::string host, std::string port, std::string root, std::string location ) {
    _init_map();
    _fill_values(host, port, root, location);
}

CGI::~CGI() {}

void CGI::_init_map() {
    for (size_t i = 0; i < sizeof(arr) / sizeof(std::string); i++)
        this->_SERVER[arr[i]] = "";
}

void CGI::_fill_map_key( std::string key , std::string value) {
    std::vector<std::string> vect(arr, arr + sizeof(arr) / sizeof(std::string));

    if (std::find(vect.begin(), vect.end(), key) != vect.end())
        this->_SERVER[key] = value;
}

#ifndef STATIC_VALUES
# define STATIC_VALUES
    # define HOME "/Users/lolopez"
    # define HTTP_CACHE_CONTROL ""
    # define HTTP_UPGRADE_INSECURE_REQUESTS ""
    # define HTTP_CONNECTION ""
    # define HTTP_ORIGIN "http://127.0.0.1:7000"
    # define HTTP_CONTENT_LENGTH ""
    # define HTTP_CONTENT_TYPE ""
    # define HTTP_REFERER "http://127.0.0.1:7000/index.php"
    # define HTTP_ACCEPT_ENCODING ""
    # define HTTP_ACCEPT_LANGUAGE ""
    # define HTTP_ACCEPT ""
    # define HTTP_USER_AGENT ""
    # define HTTP_HOST "localhost"
    # define REDIRECT_STATUS "200"
    # define SERVER_NAME "localhost"
    # define SERVER_PORT "7000"
    # define SERVER_ADDR "127.0.0.1"
    # define GATEWAY_INTERFACE "CGI/1.1"
    # define REQUEST_SCHEME "http"
    # define SERVER_PROTOCOL "HTTP/1.1"
    # define DOCUMENT_ROOT "/Users/lolopez/Documents/Webserv/www"
    # define DOCUMENT_URI "/index.php"
    # define REQUEST_URI DOCUMENT_URI
    # define SCRIPT_NAME DOCUMENT_URI
    # define CONTENT_LENGTH ""
    # define CONTENT_TYPE ""
    # define REQUEST_METHOD "GET"
    # define QUERY_STRING ""
    # define SCRIPT_FILENAME "/Users/lolopez/Documents/Webserv/www/index.php"
    # define PATH_TRANSLATED DOCUMENT_ROOT
    # define PATH_INFO ""
    # define FCGI_ROLE ""
    # define PHP_SELF DOCUMENT_URI
	#if defined (__APPLE__)
		#define PHP_CGI HOME"/.brew/bin/php-cgi"
	#else
		#define PHP_CGI ""
	#endif
# endif


void    CGI::_fill_values(std::string host, std::string port, std::string root, std::string location) {
    ////// A REFAIRE AVEC LA CONF

   _fill_map_key("HTTP_CACHE_CONTROL", HTTP_CACHE_CONTROL);
   _fill_map_key("HTTP_UPGRADE_INSECURE_REQUESTS", HTTP_UPGRADE_INSECURE_REQUESTS);
   _fill_map_key("HTTP_CONNECTION", HTTP_CONNECTION);
   _fill_map_key("HTTP_ORIGIN", "http://" + host + ":" + port);
   _fill_map_key("HTTP_CONTENT_LENGTH", HTTP_CONTENT_LENGTH);
   _fill_map_key("HTTP_CONTENT_TYPE" , HTTP_CONTENT_TYPE);
   _fill_map_key("HTTP_REFERER", "http://" + host + ":" + port + location);
   _fill_map_key("HTTP_ACCEPT_ENCODING", HTTP_ACCEPT_ENCODING);
   _fill_map_key("HTTP_ACCEPT_LANGUAGE", HTTP_ACCEPT_LANGUAGE);
   _fill_map_key("HTTP_ACCEPT", HTTP_ACCEPT);
   _fill_map_key("HTTP_USER_AGENT", HTTP_USER_AGENT);
   _fill_map_key("HTTP_HOST", host); // a changer pour le resultat du requestHeader "host"
   _fill_map_key("REDIRECT_STATUS", REDIRECT_STATUS); // a changer pour le retour de getResponse mais globalement c'est 200 a ce stade
   _fill_map_key("SERVER_NAME", SERVER_NAME); // a changer selon la conf
   _fill_map_key("SERVER_PORT", port);
   _fill_map_key("SERVER_ADDR", host);
   _fill_map_key("GATEWAY_INTERFACE", GATEWAY_INTERFACE); // ne changera pas
   _fill_map_key("REQUEST_SCHEME", REQUEST_SCHEME); // ne changera pas
   _fill_map_key("SERVER_PROTOCOL", SERVER_PROTOCOL); // ne changera pas
   _fill_map_key("DOCUMENT_ROOT", root);
   _fill_map_key("DOCUMENT_URI", location);
   _fill_map_key("REQUEST_URI", location);
   _fill_map_key("SCRIPT_NAME", location);
   _fill_map_key("CONTENT_LENGTH", CONTENT_LENGTH); // a changer pour method POST
   _fill_map_key("CONTENT_TYPE", CONTENT_TYPE);	// a remplir pour POST CONTENT_TYPE=application/x-www-form-urlencoded
   _fill_map_key("REQUEST_METHOD", REQUEST_METHOD); // GET POUR L'instant a changer (getRequest)
   _fill_map_key("QUERY_STRING", QUERY_STRING); // ?
   _fill_map_key("SCRIPT_FILENAME", root + location);
   _fill_map_key("PATH_TRANSLATED", PATH_TRANSLATED);
   _fill_map_key("PATH_INFO", PATH_INFO);
   _fill_map_key("FCGI_ROLE", FCGI_ROLE);
   _fill_map_key("PHP_SELF", location);
}

char    **CGI::_get_env() {
    char **env = (char **) malloc((sizeof(arr) / sizeof(std::string) + 1) * sizeof(char*));
    if (!env)
    	return NULL;// MALLOC ERROR, return une erreur a envoyer au serv;
    for (size_t i = 0; i < sizeof(arr) / sizeof(std::string); i++)
    {
        env[i] = strdup((arr[i] + "=" + _SERVER[arr[i]]).c_str());
        if (!env[i])
        {
			_dstrfree(env);
			return NULL;
		}
		// MALLOC ERROR, return une erreur a envoyer au serv;
        std::cout << env[i] << std::endl;
    }
	env[sizeof(arr) / sizeof(std::string)] = NULL;
	return env;
}

char **CGI::env() {
    return _get_env();
}

char **CGI::_get_action() {
	std::string ret[] = {PHP_CGI};
	char **action = (char **)malloc((sizeof(ret) / sizeof(std::string) + 1) * sizeof(char *));
	if (!action)
		return NULL;// MALLOC ERROR, return une erreur a envoyer au serv;
	for (size_t i = 0; i < sizeof(ret) / sizeof(std::string); i++)
	{
		action[i] = strdup(ret[i].c_str());
		if (!(action[i]))
		{
			_dstrfree(action);
			return NULL;
		}
	}
	action[sizeof(ret) / sizeof(std::string)] = NULL;
	return action;
}
// C type shell
void	CGI::cgi_exec()
{
	int fd = open("./srcs/php_content", O_RDWR | O_TRUNC | O_CREAT);
	pid_t pid;
	char **env = _get_env();
	if (!env)
		throw CGI::Error();
	char **action = _get_action();
	if (!action)
	{
		_dstrfree(env);
		throw CGI::Error();
	}
	if ((pid = fork()) == 0)
	{
		dup2(fd, 1);
		close(fd);
		execve(action[0], action, env);
		exit(0);
	}
	else {
		waitpid(pid, NULL, 0);
		close(fd);
		_dstrfree(env);
		_dstrfree(action);
	}
}

void	CGI::_dstrfree(char ** dstr) {
	int i = -1;

	while (dstr[++i])
	{
		free(dstr[i]);
		dstr[i] = NULL;
	}
	free(dstr);
}

/*
TO DO:
    - Test avec le moins de HEADER possible pour voir si ca marche
*/
