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

		std::cout << BLUE << "index : " << RESET;
		std::list<std::string>::iterator	it = conf.get_config(i)->index.begin();
		while (it != conf.get_config(i)->index.end())
			std::cout << *it++ << YELLOW << " | " << RESET;

		std::cout << BLUE << "\nerror_page : " << RESET;
		std::map<int,std::string>::iterator is = conf.get_config(i)->error_page.begin();
		while (is != conf.get_config(i)->error_page.end())
		{
			std::cout << is->first << CYAN << " => " << RESET << is->second << YELLOW << " | " << RESET;
			is++;
		}
		// std::cout << conf.get_config(i)->error_page[404] << std::endl;

		///////// LOCATIONS /////////
		for (unsigned int j = 1; j <= conf.get_config(i)->_nb_location; j++)
		{
			std::cout << CYAN << "\nlocation " << j << " : " << RESET;
			std::cout << "\t" << YELLOW << "cgi : " << RESET; 
			std::cout << conf.get_location(conf.get_config(i), j)->cgi << std::endl;
			std::cout << "\t\t" << YELLOW << "access : " << RESET; 
			std::cout << conf.get_location(conf.get_config(i), j)->access << std::endl;
			std::cout << "\t\t" << YELLOW << "root : " << RESET; 
			std::cout << conf.get_location(conf.get_config(i), j)->root << std::endl;
			std::cout << "\t\t" << YELLOW << "index : " << RESET;
			std::list<std::string>::iterator	ix = conf.get_location(conf.get_config(i), j)->index.begin();
			while (ix != conf.get_location(conf.get_config(i), j)->index.end())
				std::cout << *ix++ << YELLOW << " | " << RESET;
			std::cout << std::endl;
			std::cout << "\t\t" << YELLOW << "client_max_body_size : " << RESET;
			std::cout << conf.get_location(conf.get_config(i), j)->client_max_body_size << std::endl;
			std::cout << "\t\t" << YELLOW << "allow_methods : " << RESET;
			std::cout << conf.get_location(conf.get_config(i), j)->allow_methods << std::endl;
			std::cout << "\t\t" << YELLOW << "cgi_path : " << RESET;
			std::cout << conf.get_location(conf.get_config(i), j)->cgi_path << std::endl;
			std::cout << "\t\t" << YELLOW << "try_files: " << RESET;
			std::list<std::string>::iterator	iw = conf.get_location(conf.get_config(i), j)->try_files.begin();
			while (iw != conf.get_location(conf.get_config(i), j)->try_files.end())
				std::cout << *iw++ << YELLOW << " | " << RESET;
			std::cout << std::endl;
		}
		std::cout << "\n" << std::endl;
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
		display_help(conf);
		std::cout << GREEN << "Server config Good" << RESET << std::endl;
		
		// /* Launch Server */
		// Server	server;
	
		// for ( int i = 1; i <= conf.get_nb_server(); i++ ) {
		// 	server.setup_server_socket(conf, i);
		// 	server.set_socket_reuse();
		// 	server.binded();
		// 	server.listened();
		// 	server.add_server_lst();
		// }
		// server.selected();
	}
	catch(const std::exception& e) {
		std::cerr << RED << e.what() << RESET << std::endl;
	}
	return (0);
}
