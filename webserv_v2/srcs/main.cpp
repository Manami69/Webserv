#include "../includes/config.hpp"

void	parsing(int ac, char **av)
{
	std::string	file;

	if ( ac == 1 )
		file = "config/webserv.conf";
	else
		file = av[1];

	Config	conf( file );
	conf.scan_file();
	conf.check_brackets();
	conf.parse_config();
	std::cout << GREEN << "Parsing Good" << RESET << std::endl;
}

int		main(int ac, char **av) {
	if (ac > 2) {
		std::cout << RED << av[0] << "can take one argument <configuration file> only. "
		<< "If no argument has been entered, it will launch with the default configuration file."
		<< RESET << std::endl;
		return (1);
	}
	try {
		parsing(ac, av);
	}
	catch(const std::exception& e) {
		std::cerr << e.what() << '\n';
	}
	return (0);
}