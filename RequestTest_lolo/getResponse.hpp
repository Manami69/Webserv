#ifndef GETRESPONSE_H
# define GETRESPONSE_H
# include <iostream>
# include <string>
# include <map>
# include <vector>
# include <algorithm>
# include "getRequest.hpp"

# define CRLF "\r\n"
# define SP " "

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

	private:
		void	_create_keys_map( void );
		bool	_parse_status_line( void );
		getResponse( void );
		std::vector<std::string>			_keys;
		std::map<std::string, std::string>	_res_token;
		getRequest							_request;
};

#endif
