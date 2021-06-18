#include "../includes/config.hpp"
#include "../includes/server.hpp"

void	display_help(Config conf) // temporary
{
	for (int i = 1; i <= conf.get_nb_server(); i++)
	{
		std::cout << MAGENTA << "SERVER : " << i << std::endl;

		std::cout << BLUE << "host : " << RESET;
		std::cout << conf.get_config(i)->host << std::endl;

		std::cout << BLUE << "port : " << RESET;
		std::cout << conf.get_config(i)->port << std::endl;

		std::cout << BLUE << "server_name : " << RESET;
		std::cout << conf.get_config(i)->server_name << std::endl;

		std::cout << BLUE << "root : " << RESET;
		std::cout << conf.get_config(i)->root << std::endl;

		std::cout << BLUE << "client_max_body_size : " << RESET;
		std::cout << conf.get_config(i)->client_max_body_size << std::endl;

		std::cout << BLUE << "error_page : " << RESET;
		std::map<int,std::string>::iterator is = conf.get_config(i)->error_page.begin();
		while (is != conf.get_config(i)->error_page.end())
		{
			std::cout << is->first << CYAN << " => " << RESET << is->second << YELLOW << " | " << RESET;
			is++;
		}
		// std::cout << conf.get_config(i)->error_page[404] << std::endl;

		std::cout << BLUE << "\nindex : " << RESET;
		std::list<std::string>::iterator	it = conf.get_config(i)->index.begin();
		while (it != conf.get_config(i)->index.end())
			std::cout << *it++ << YELLOW << " | " << RESET;
		std::cout << std::endl;
	}
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
		/* Get server config content */
		std::string	file;
		if ( ac == 1 )
			file = "config/webserv.conf";
		else
			file = av[1];
		Config	conf( file );
		conf.scan_file();
		conf.check_brackets();
		conf.parse_config();
		std::cout << GREEN << "Server config Good" << RESET << std::endl;
		
		/* Launch Server */
		Server	server;
	
		for ( int i = 1; i <= conf.get_nb_server(); i++ ) {
			server.setup_server_socket(conf, i);
			server.set_socket_reuse();
			server.binded();
			server.listened();
			server.add_server_lst();
		}
		server.selected();
	}
	catch(const std::exception& e) {
		std::cerr << RED << e.what() << RESET << std::endl;
	}
	return (0);
}
