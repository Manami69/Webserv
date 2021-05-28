#include "../includes/parsing.hpp"
#include "../includes/server.hpp"

int		main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cout << RED << "Error : need config file" << RESET << std::endl;
		exit(1);
	}
	//	Create list of configuration in config file
	std::list<t_config> config;
	//	Parse this configuration
	parse_config(argv[1], config);

	//	Example to access value
	std::cout << MAGENTA << std::endl;
	std::cout << config.begin()->listen << std::endl;
	std::cout << config.begin()->server_name << std::endl;
	std::cout << config.begin()->root << std::endl;
	std::cout << config.begin()->default_error_page << std::endl;
	std::cout << config.begin()->body_size_limit << std::endl;
	std::list<std::string>::iterator id = config.begin()->index.begin();
	while (id != config.begin()->index.end())
	{
		std::cout << *id << std::endl;
		id++;
	}
	std::cout << RESET << std::endl;

	//	Launch the server(s)
	std::list<t_config>::iterator it = config.begin();
	while (it != config.end())
		launch_server(*it++);
}
