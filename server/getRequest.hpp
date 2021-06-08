#ifndef GETREQUEST_H
# define GETREQUEST_H
# include "header.hpp"
# define BLK "\e[0;30m"
# define GRN "\e[0;32m"
# define YEL "\e[0;33m"
# define BLU "\e[0;34m"
# define MAG "\e[0;35m"
# define CYN "\e[0;36m"
# define WHT "\e[0;37m"
# define END "\e[0m"
# define CRLF "\r\n"

class getRequest {
	public:
		//typedef typename std::map<std::string, std::string>::iterator iterator;

		getRequest( void );
		getRequest( std::string buffer );
		~getRequest( void );
		getRequest( getRequest const & src );

		getRequest &	operator=( getRequest const & rhs );
		std::string&	operator[] ( const std::string& key );
		const std::string&	operator[] ( const std::string& key ) const;

		// Setters
		void			fillRequest( std::string buffer );
		void			setKeyValue( std::string key, std::string val );
		// Getters
		std::string		getKeyValue( std::string key ) const;
		std::map<std::string, std::string>	getMap( void ) const;
		static const std::string headers[];
		class BadRequestException : public std::exception {
			public:
				virtual const char* what() const throw() {
					return ("Error: Bad Request");
				}
		};
	private:
		std::map<std::string, std::string>	_request_tokens;
		std::vector<std::string>			_array;
		void	_construct_array( void );
		void	_construct_empty_map( void );
		bool	_is_used_key(std::string key) const;
		bool	_check_request(std::string request);
};

std::ostream & operator<<( std::ostream & o, getRequest const & rhs );

#endif
