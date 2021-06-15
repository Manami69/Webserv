#ifndef CONFIG_HPP
# define CONFIG_HPP

#include "header.hpp"

class Config
{
private:
	std::string					_filename;
	std::vector<std::string>	_tokens;
	Config( void );
	// Config(Config const &copy);
	// Config &operation=(Config const &rhs);

public:
	class WrongFilename : public std::exception {
	public:
		const char *what() const throw(); // override
	};
	Config( std::string filename );
	~Config( void );
	std::string					get_filename( void ) const;
	void						check_filename( void );
	void						scan_file( void );
	void						tokenize( std::string line );
	std::vector<std::string>	get_tokens( void ) const;
};

void	parsing(int ac, char **av);

#endif