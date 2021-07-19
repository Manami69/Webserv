#include "../includes/config.hpp"
#include "../includes/server.hpp"

int		main(int ac, char **av) {
	
	std::string	config_path ("config/webserv.conf");
	
	if (ac > 2) {
		std::cout << RED << av[0] << " can take one argument <configuration file> only. "
		<< "If no argument has been entered, it will launch with the default configuration file."
		<< RESET << std::endl;
		return (1);
	}
	
	config_path = (ac == 1) ? config_path : av[1];
	Config	conf( config_path );
	Server	server;
	try {
		std::cout << WHITE << "Loading configuration file ... " << RESET;
		conf.scan();
		std::cout << GREEN << "SUCCESS" << RESET << std::endl;
		
		remove(PHP_CONTENT);

		std::cout << WHITE << "Setup server ... " << RESET;
		for ( int i = 0; i < conf.get_nb_server(); i++ )
			server.setup_server(conf, i);
		std::cout << GREEN << "READY" << RESET << std::endl;
	}
	catch(const std::exception& e) {
		std::cerr << RED << e.what() << RESET << std::endl;
	}
	//std::cout << server.get_server_lst().size() << std::endl;
	server.selected(conf);
	return (0);
}
