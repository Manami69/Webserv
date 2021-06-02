#ifndef GETRESPONSE_H
# define GETRESPONSE_H
# include <iostream>
# include <string>
# include <map>
# include <vector>
# include <sstream>
# include <fstream>
# include <algorithm>
# include <dirent.h>
# include "getRequest.hpp"

# define CRLF "\r\n"
# define SP " "
# define CURRDIR "./../www"
# define PAGE "index.html"
# define ERROR404 "./../www/404.html"
#define ERR "Content-Type: text/html\n\
content returned: <!DOCTYPE html>\n\n\
ERROR "

/*
** RFC 7230 3.1.2.
** The first line of a response message is the status-line:
** status-line = HTTP-version SP status-code SP reason-phrase CRLF
** status-code = 3DIGIT
	""" The status-code element is a 3-digit integer code describing the
	result of the server's attempt to understand and satisfy the client's
	corresponding request.  The rest of the response message is to be
	interpreted in light of the semantics defined for that status code.
	See Section 6 of [RFC7231] """
** STATUS CODE TAB https://datatracker.ietf.org/doc/html/rfc7231#section-6
** reason-phrase  = *( HTAB / SP / VCHAR / obs-text )
	""" The reason-phrase element exists for the sole purpose of providing a
	textual description associated with the numeric status code, mostly
	out of deference to earlier Internet application protocols that were
	more frequently used with interactive text clients.  A client SHOULD
	ignore the reason-phrase content. """
**
*/


class getResponse {
	public:
		~getResponse( void );
		getResponse( getRequest const &request);
		getResponse( getResponse const & src );

		getResponse &	operator=( getResponse const & rhs );
		std::string		responsetosend(const std::map<int, std::string> err);
		// static std::map<int, std::string>	error_code; // ptet a mettre dans une autre classe unique
		// static std::map<int, std::string> create_error() {
		// 	std::map<int, std::string> err;
		// 	err[200] = "OK";
		// 	err[400] = "Bad Request";
		// 	err[404] = "Not Found";
		// 	err[505] = "HTTP Version Not Supported";
		// 	return err;
		// }
	private:
		int	_parse_status_line( void );
		getResponse( void );
		// GET METHOD
		std::string	_get_fill_headers( std::string response );
		std::string _method_get( void );
		// GET MIMETYPE
		std::string	_get_extension( void );
		std::string _get_MIMEtype( const std::string& ext);
		std::vector<std::string>			_keys;
		int									_status_code;
		getRequest							_request;
		std::string							_content;
};

//std::map<int, std::string> getResponse::error_code = getResponse::create_error();
#endif
