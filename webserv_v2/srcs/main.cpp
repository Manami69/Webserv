#include "../includes/config.hpp"
#include "../includes/server.hpp"

int		main(int ac, char **av)
{
	if (ac > 2) {
		std::cout << RED << av[0] << " can take one argument <configuration file> only. "
		<< "If no argument has been entered, it will launch with the default configuration file."
		<< RESET << std::endl;
		return (1);
	}
	try {
		/* Get server config content */
		std::string	file;
		if ( ac == 1 )
			file = "config/webserv.conf";
		else
			file = av[1];
		Config	conf( file );
		conf.scan_file();
		conf.parse_config();
		
		std::cout << GREEN << "Webserv configuration OK" << RESET << std::endl;
		/* Launch Server */
		// Server	server;
	
		// for ( int i = 0; i < conf.get_nb_server(); i++ ) {
		// 	server.setup_server_socket(conf, i);
		// 	server.set_socket_reuse();
		// 	server.binded();
		// 	server.listened();
		// 	server.add_server_lst();
		// 	}
		// server.selected(conf);
	}
	catch(const std::exception& e) {
		std::cerr << RED << e.what() << RESET << std::endl;
	}
	return (0);
}
