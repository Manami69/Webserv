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
/////////////////////////////////////////////////////////////////////////////////
	// std::cout << conf.get_nb_server() << std::endl;
	// std::cout << conf.get_config(1)->host << std::endl;
	// std::cout << conf.get_config(2)->host << std::endl;
	// std::cout << conf.get_config(3)->host << std::endl;
	// std::cout << conf.get_config(4)->host << std::endl;
/////////////////////////////////////////////////////////////////////////////////
}

int		main(int ac, char **av)
{
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
		std::cerr << RED << e.what() << std::endl;
	}
	return (0);
}
