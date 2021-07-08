#include "./../includes/CGI.hpp"


const std::string CGI::arr[] = {"HTTP_CACHE_CONTROL", "HTTP_UPGRADE_INSECURE_REQUESTS", \
    "HTTP_CONNECTION", "HTTP_ORIGIN", "HTTP_CONTENT_LENGTH", "HTTP_CONTENT_TYPE" , "HTTP_REFERER", "HTTP_ACCEPT_ENCODING",\
    "HTTP_ACCEPT_LANGUAGE", "HTTP_ACCEPT", "HTTP_USER_AGENT", "HTTP_HOST", "USER", "HOME" ,"REDIRECT_STATUS", "SERVER_NAME", "SERVER_PORT",\
    "SERVER_ADDR", "GATEWAY_INTERFACE", "REQUEST_SCHEME", "SERVER_PROTOCOL", "DOCUMENT_ROOT",\
    "DOCUMENT_URI", "REQUEST_URI", "SCRIPT_NAME", "CONTENT_LENGTH", "CONTENT_TYPE", "REQUEST_METHOD", "QUERY_STRING",\
    "SCRIPT_FILENAME", "PATH_TRANSLATED", "PATH_INFO", "FCGI_ROLE", "PHP_SELF"};

CGI::CGI( getRequest req, std::string port, std::string root, std::string path_cgi) : _req(req), _CGIPath(path_cgi)  {
    _init_map();
    _fill_values( port, root);
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
    # define HTTP_CACHE_CONTROL ""
    # define HTTP_UPGRADE_INSECURE_REQUESTS ""
    # define HTTP_CONNECTION ""
    # define REDIRECT_STATUS "200"
    # define SERVER_NAME "webserv/v1.0"
    # define GATEWAY_INTERFACE "CGI/1.1"
    # define REQUEST_SCHEME "http"
	# define PATH_INFO ""
    # define SERVER_PROTOCOL "HTTP/1.1"
# endif


void    CGI::_fill_values( std::string port, std::string root) {
    ////// A REFAIRE AVEC LA CONF
	_fill_map_key("USER", getenv("USER"));
	_fill_map_key("HOME", getenv("HOME"));
	_fill_map_key("HTTP_CACHE_CONTROL", HTTP_CACHE_CONTROL);
	_fill_map_key("HTTP_UPGRADE_INSECURE_REQUESTS", HTTP_UPGRADE_INSECURE_REQUESTS);
	_fill_map_key("HTTP_CONNECTION", HTTP_CONNECTION);
	_fill_map_key("HTTP_ORIGIN", "http://" + _req["Host"]);
	_fill_map_key("HTTP_CONTENT_LENGTH", _req["Content-Length"]);
	_fill_map_key("HTTP_CONTENT_TYPE" , _req["Content-Type"]);
	_fill_map_key("HTTP_REFERER", _req["Referer"]);
	_fill_map_key("HTTP_ACCEPT_ENCODING", _req["Accept-Encoding"]);
	_fill_map_key("HTTP_ACCEPT_LANGUAGE", _req["Accept-Language"]);
	_fill_map_key("HTTP_ACCEPT", _req["Accept"]);
	_fill_map_key("HTTP_USER_AGENT", _req["User-Agent"]);
	_fill_map_key("HTTP_HOST", _req["Host"]);
	_fill_map_key("REDIRECT_STATUS", REDIRECT_STATUS); // a changer pour le retour de getResponse mais globalement c'est 200 a ce stade
	_fill_map_key("SERVER_NAME", SERVER_NAME); // a changer selon la conf
	_fill_map_key("SERVER_PORT", port);
	_fill_map_key("SERVER_ADDR", _req["Host"]);
	_fill_map_key("GATEWAY_INTERFACE", GATEWAY_INTERFACE); // ne changera pas
	_fill_map_key("REQUEST_SCHEME", REQUEST_SCHEME); // ne changera pas
	_fill_map_key("SERVER_PROTOCOL", SERVER_PROTOCOL); // ne changera pas
	_fill_map_key("DOCUMENT_ROOT", root);
	size_t cut;
	if ((cut = _req["request-target"].find("?")) == std::string::npos)
		cut = 0;
	_fill_map_key("DOCUMENT_URI", (cut ? _req["request-target"].substr(0, cut) : _req["request-target"]));
	_fill_map_key("REQUEST_URI", _req["request-target"]);
	_fill_map_key("SCRIPT_NAME", (cut ? _req["request-target"].substr(0, cut) : _req["request-target"]));
	_fill_map_key("CONTENT_LENGTH", _req["Content-Length"]); // a changer pour method POST
	_fill_map_key("CONTENT_TYPE", _req["Content-Type"]);	// a remplir pour POST CONTENT_TYPE=application/x-www-form-urlencoded
	_fill_map_key("REQUEST_METHOD", _req["method"]); // GET POUR L'instant a changer (getRequest)
	_fill_map_key("QUERY_STRING", (cut ? _req["request-target"].substr(cut + 1) : "")); // ?
	_fill_map_key("SCRIPT_FILENAME", root + (cut ? _req["request-target"].substr(0, cut + 4) : _req["request-target"]));
	_fill_map_key("PATH_TRANSLATED", root);
	_fill_map_key("PATH_INFO", PATH_INFO);
	_fill_map_key("PHP_SELF", (cut ? _req["request-target"].substr(0, cut) : _req["request-target"]));
}

static char	*ft_strjoin(char const *s1, char const *s2)
{
	unsigned long	i;
	unsigned long	len;
	char			*str;

	if (!s1 && !s2)
		return (0);
	len = (strlen(s1) + strlen(s2));
	i = strlen(s1);
	if (!(str = (char *)malloc((len + 1) * sizeof(char))))
		return (NULL);
	memcpy(str, s1, strlen(s1));
	memcpy(&str[i], s2, strlen(s2));
	str[len] = '\0';
	return (str);
}

char    **CGI::_get_env() {
    char **env = (char **) malloc((sizeof(arr) / sizeof(std::string) + 1) * sizeof(char*));
    std::string next;
	if (!env)
    	return NULL;// MALLOC ERROR, return une erreur a envoyer au serv;
    for (size_t i = 0; i < sizeof(arr) / sizeof(std::string); i++)
    {
		next = "";
		next.append(arr[i]);
		next += "=";
		//next.append(_SERVER[arr[i]]);
        env[i] = ft_strjoin((next).c_str(), _SERVER[arr[i]].c_str());
        if (!env[i])
        {
			_dstrfree(env);
			return NULL;
		}
		std::cout << env[i] << std::endl;
		// MALLOC ERROR, return une erreur a envoyer au serv;
    }
	env[sizeof(arr) / sizeof(std::string)] = NULL;
	return env;
}

char **CGI::env() {
    return _get_env();
}

char **CGI::_get_action(bool act) {
	std::string ret[2];// = (act ? {"/bin/cat", _req["body"]} : {_CGIPath, _SERVER["SCRIPT_FILENAME"]});
	ret[0] = act ? "/bin/cat" : _CGIPath;
	ret[1] = act ? _req["body"]: _SERVER["SCRIPT_FILENAME"];
	size_t		len = 2;
	char **action = (char **)malloc((len + 1) * sizeof(char *));
	if (!action)
		return NULL;// MALLOC ERROR, return une erreur a envoyer au serv;
	for (size_t i = 0; i < len; i++)
	{
		action[i] = strdup(ret[i].c_str());
		if (!(action[i]))
		{
			_dstrfree(action);
			return NULL;
		}
	}
	action[len] = NULL;
	return action;
}

void	CGI::_exec_body( void ) {
	int pipefd[2];
	pid_t pid;
	pid_t pod;
	char **env = _get_env();
	if (!env)
		throw CGI::Error();	
	char **action = _get_action(true);
	if (!action)
	{
		_dstrfree(env);
		throw CGI::Error();
	}
	pipe(pipefd);
	if ((pod = fork()) == 0)
	{
		dup2(pipefd[1], 1);
		// close(pipefd[1]);
		close(pipefd[0]);
		execve(action[0], action, env);
		exit(0);
	}
	else {
		int fd = open("./tmp/php_content", O_RDWR | O_TRUNC | O_CREAT | O_NONBLOCK, 0777);
		//pid = 0;man
		char **actionman = _get_action(false);
		if (!actionman)
		{	
			_dstrfree(action);
			_dstrfree(env);
			throw CGI::Error();
		}
 		pid = fork();
		if ((pid) == 0)
		{
			dup2(pipefd[0], 0);
			dup2(fd, 1);
			// close(pipefd[0]);
			close(pipefd[1]);
			//close(fd);
			execve(actionman[0], actionman, env);
			exit(0);
		}
		else {
			waitpid(pod, NULL, 0);
			waitpid(pid, NULL, 0);
			close(pipefd[1]);
			close(pipefd[0]);
			close(fd);
			_dstrfree(action);
			_dstrfree(actionman);
			_dstrfree(env);
		}
	}
}

void	CGI::_exec_nobody( void ) {
	int fd = open("./tmp/php_content", O_RDWR | O_TRUNC | O_CREAT | O_NONBLOCK);
	pid_t pid;
	char **env = _get_env();
	if (!env)
		throw CGI::Error();
	std::string ret[] = {_CGIPath};
	char **action = _get_action(false);
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

// C type shell
void	CGI::cgi_exec()
{
	if (!_req["body"].empty() && !_req["method"].compare("POST"))
		_exec_body();
	else
		_exec_nobody();
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