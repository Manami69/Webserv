#ifndef GETRESPONSE_H
# define GETRESPONSE_H
# include "header.hpp"
# include "getRequest.hpp"
# include "CGI.hpp"
# include "config.hpp"
#if defined (__APPLE__)
	# define st_mtim st_mtimespec
	# define ROOT "/Users/lolopez/Desktop/lolol/www"
#else
	#define ROOT "/home/lolo/Documents/Webserv/www"
#endif

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

struct t_index_file
{
	std::string			name;
	int					spaceL;
	std::string			date;
	long unsigned int	size;
	unsigned char		type;
};

class getResponse {
	public:
		~getResponse( void );
		//getResponse(); TODO constructeur qui prend uniquement la status line et renvoie directement 400 si besoin
		getResponse( getRequest const &request, Serv_config conf);
		getResponse( getResponse const & src );

		getResponse &	operator=( getResponse const & rhs );
		std::string		responsetosend(const std::map<int, std::string> err);
	private:

		// 🅤🅣🅘🅛🅢
		int	_parse_status_line( void );
		getResponse( void );
		std::string		_error_response(const std::map<int, std::string> err);
		// // GET MIMETYPE
		std::string	_get_extension( void );
		std::string _get_MIMEtype( const std::string& ext);
		// // GET HEADERS LINE
		std::string	_get_date_line( void );
		std::string _get_serv_line( void );
		// // USE PHP-CGI (if enabled)
		std::string _use_php();


		// 🅜🅔🅣🅗🅞🅓🅢
		// // GET METHOD
		std::string	_get_fill_headers( std::string response );
		std::string _method_get( void );
		// // // AUTOINDEX PAGE GENERATOR
		std::string _get_autoindex( std::string location );
		std::string _fill_index_body(std::list<t_index_file> files);

		// // POST METHOD ;TODO
		std::string	_method_post( void );

		// // DELETE METHOD
		std::string _delete_fill_header( void );
		int			_delete_file( void );
		std::string	_method_delete( void );

		// 🅥🅐🅡🅘🅐🅑🅛🅔🅢
		std::vector<std::string>			_keys;
		int									_status_code;
		getRequest							_request;
		std::string							_content;
		Serv_config							_conf;
};

//std::map<int, std::string> getResponse::error_code = getResponse::create_error();
#endif

