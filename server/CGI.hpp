#ifndef CGI_HPP
# define CGI_HPP
# include "header.hpp"

class CGI {
    public:
        CGI( std::string host, std::string port, std::string root, std::string location );
        ~CGI( void );
        static const std::string arr[];
        char **env();
		void	cgi_exec();
		class Error : public std::exception {
			public:
				virtual const char* what() const throw() {
					return ("CGI error");
				}
		};
    private:
        std::map<std::string, std::string> _SERVER;
        void    _init_map( void );
        void    _fill_map_key(std::string key, std::string value);
        void    _fill_values(std::string host, std::string port, std::string root, std::string location);
        char    **_get_env();
		char	**_get_action();
		void	_dstrfree(char ** dstr);
};


//         $_REQUEST['say']	HELLO
//         $_REQUEST['to']	POPO
//         $_POST['say']	HELLO
//         $_POST['to']	POPO
//         $_SERVER['USER']	lolopez
//         $_SERVER['HOME']	/Users/lolopez
//         $_SERVER['HTTP_CACHE_CONTROL']	max-age=0
//         $_SERVER['HTTP_UPGRADE_INSECURE_REQUESTS']	1
//         $_SERVER['HTTP_CONNECTION']	keep-alive
//         $_SERVER['HTTP_ORIGIN']	http://127.0.0.1:8080
//         $_SERVER['HTTP_CONTENT_LENGTH']	17
//         $_SERVER['HTTP_CONTENT_TYPE']	application/x-www-form-urlencoded
//         $_SERVER['HTTP_REFERER']	http://127.0.0.1:8080/php-example.html
//         $_SERVER['HTTP_ACCEPT_ENCODING']	gzip, deflate
//         $_SERVER['HTTP_ACCEPT_LANGUAGE']	fr,fr-FR;q=0.8,en-US;q=0.5,en;q=0.3
//         $_SERVER['HTTP_ACCEPT']	text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8
//         $_SERVER['HTTP_USER_AGENT']	Mozilla/5.0 (Macintosh; Intel Mac OS X 10.14; rv:86.0) Gecko/20100101 Firefox/86.0
//         $_SERVER['HTTP_HOST']	127.0.0.1:8080
//         $_SERVER['REDIRECT_STATUS']	200
//         $_SERVER['SERVER_NAME']	localhost
//         $_SERVER['SERVER_PORT']	8080
//         $_SERVER['SERVER_ADDR']	127.0.0.1
//         $_SERVER['REMOTE_PORT']	49541
//         $_SERVER['REMOTE_ADDR']	127.0.0.1
//         $_SERVER['SERVER_SOFTWARE']	nginx/1.21.0
//         $_SERVER['GATEWAY_INTERFACE']	CGI/1.1
//         $_SERVER['REQUEST_SCHEME']	http
//         $_SERVER['SERVER_PROTOCOL']	HTTP/1.1
//         $_SERVER['DOCUMENT_ROOT']	/Users/lolopez/.brew/var/www
//         $_SERVER['DOCUMENT_URI']	/php-example.php
//         $_SERVER['REQUEST_URI']	/php-example.php
//         $_SERVER['SCRIPT_NAME']	/php-example.php
//         $_SERVER['CONTENT_LENGTH']	17
//         $_SERVER['CONTENT_TYPE']	application/x-www-form-urlencoded
//         $_SERVER['REQUEST_METHOD']	POST
//         $_SERVER['QUERY_STRING']	no value
//         $_SERVER['SCRIPT_FILENAME']	/Users/lolopez/.brew/var/www/php-example.php
//         $_SERVER['PATH_TRANSLATED']	/Users/lolopez/.brew/var/www
//         $_SERVER['PATH_INFO']	no value
//         $_SERVER['FCGI_ROLE']	RESPONDER
//         $_SERVER['PHP_SELF']	/php-example.php

// 
// $_SERVER['USER']</td><td class="v">lolopez</td></tr>
// $_SERVER['HOME']</td><td class="v">/Users/lolopez</td></tr>
// $_SERVER['HTTP_HOST']</td><td class="v">localhost</td></tr>
// $_SERVER['REDIRECT_STATUS']</td><td class="v">200</td></tr>
// $_SERVER['SERVER_NAME']</td><td class="v">localhost</td></tr>
// $_SERVER['SERVER_PORT']</td><td class="v">8080</td></tr>
// $_SERVER['SERVER_ADDR']</td><td class="v">127.0.0.1</td></tr>
// $_SERVER['REMOTE_PORT']</td><td class="v">51991</td></tr>
// $_SERVER['REMOTE_ADDR']</td><td class="v">127.0.0.1</td></tr>
// $_SERVER['SERVER_SOFTWARE']</td><td class="v">nginx/1.21.0</td></tr>
// $_SERVER['GATEWAY_INTERFACE']</td><td class="v">CGI/1.1</td></tr>
// $_SERVER['REQUEST_SCHEME']</td><td class="v">http</td></tr>
// $_SERVER['SERVER_PROTOCOL']</td><td class="v">HTTP/1.1</td></tr>
// $_SERVER['DOCUMENT_ROOT']</td><td class="v">/Users/lolopez/.brew/var/www</td></tr>
// $_SERVER['DOCUMENT_URI']</td><td class="v">/index.php</td></tr>
// $_SERVER['REQUEST_URI']</td><td class="v">/index.php</td></tr>
// $_SERVER['SCRIPT_NAME']</td><td class="v">/index.php</td></tr>
// $_SERVER['CONTENT_LENGTH']</td><td class="v"><i>no value</i></td></tr>
// $_SERVER['CONTENT_TYPE']</td><td class="v"><i>no value</i></td></tr>
// $_SERVER['REQUEST_METHOD']</td><td class="v">GET</td></tr>
// $_SERVER['QUERY_STRING']</td><td class="v"><i>no value</i></td></tr>
// $_SERVER['SCRIPT_FILENAME']</td><td class="v">/Users/lolopez/.brew/var/www/index.php</td></tr>
// $_SERVER['PATH_TRANSLATED']</td><td class="v">/Users/lolopez/.brew/var/www</td></tr>
// $_SERVER['PATH_INFO']</td><td class="v"><i>no value</i></td></tr>
// $_SERVER['FCGI_ROLE']</td><td class="v">RESPONDER</td></tr>
// $_SERVER['PHP_SELF']</td><td class="v">/index.php</td></tr>
// $_SERVER['REQUEST_TIME_FLOAT']</td><td class="v">1623166005.9338</td></tr>
// $_SERVER['REQUEST_TIME']</td><td class="v">1623166005</td></tr>


#endif