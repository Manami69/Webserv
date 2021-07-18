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
	
	try {
		std::cout << WHITE << "Webserv configuration preparing ... " << RESET << std::endl;
		Config	conf( config_path );
		remove(PHP_CONTENT);

		std::cout << WHITE << "Webserv server preparing ... " << RESET << std::endl;
		Server	server;
	
		for ( int i = 0; i < conf.get_nb_server(); i++ ) {
			server.setup_server_socket(conf, i);
			server.set_socket_reuse();
			server.binded();
			server.listened();
			server.add_server_lst();
		}
		server.selected(conf);
	}
	catch(const std::exception& e) {
		std::cerr << RED << e.what() << RESET << std::endl;
	}
	return (0);
}
