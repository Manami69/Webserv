#include "../includes/config.hpp"

int		main(int ac, char **av) {
	if (ac > 2) {
		std::cout << av[0] << "can take one argument <configuration file> only. "
		<< "If no argument has entered, it will launch with a default configuration file."
		<< std::endl;
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