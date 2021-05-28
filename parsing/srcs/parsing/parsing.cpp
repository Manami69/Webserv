#include "../../includes/parsing.hpp"

// void	parse_location(t_config &conf, int fd, std::string &first_line)
// {
// 	t_location		loc;
// 	std::string		line;

// 	while (read_location(fd, &line))
// 	{
// 		if (check_line(line, "http_methods"))
// 			loc.http_methods = get_config(line, "http_methods");
// 	}
// 	conf.locations.push_back(loc);
// }

int		parse_configuration(t_config &conf, int fd)
{
	std::string		line;

	//	Read config file
	while (read_config(fd, &line))
	{
		if (check_line(line, "listen"))
			conf.listen = get_config(line, "listen");
		else if (check_line(line, "server_name"))
			conf.server_name = get_config(line, "server_name");
		else if (check_line(line, "root"))
			conf.root = get_config(line, "root");
		else if (check_line(line, "default_error_page"))
			conf.default_error_page = get_config(line, "default_error_page");
		else if (check_line(line, "body_size_limit"))
		{
			conf.body_size_limit = ft_atoi(get_config(line, "body_size_limit").c_str());
			if (conf.body_size_limit == 0)
			{
				std::cout << RED << "Error : body size limit is not a number in .conf" << std::endl;
				return (1);
			}
		}
		else if	(check_line(line, "index"))
			conf.index = get_index(line, "index");
		// else if (check_line(line, "location"))
		// 	parse_location(conf, fd, line);
	}
	return (0);
}

void		init_configuration(std::list<t_config> &config)
{
	//	Create a default configuration
	t_config conf;

	conf.listen = "";
	conf.server_name = "";
	conf.root = "";
	conf.default_error_page = "";
	conf.body_size_limit = 1;
	config.push_back(conf);
}

void		parse_config(std::string path, std::list<t_config> &config)
{
	int			fd;
	char		*line;

	//	Open the configuration file
	fd = open(path.c_str(), O_RDONLY);
	//	Error with this file
	if (fd == -1)
	{
		if (errno == EACCES)
			std::cout << RED << "Error : permission denied" << RESET << std::endl;
		else
			std::cout << RED << "Error : file doesn't exist" << RESET << std::endl;
		exit(1);
	}
	//	Read the file
	while (get_next_line(fd, &line) == 1)
	{
		//	For each configuration
		if (check_line(line, "server"))
		{
			init_configuration(config);
			if (parse_configuration(config.back(), fd) == 1)
			{
				free(line);
				exit(1);
			}
		}
		free(line);
	}
	close(fd);
	//	No configuration
	if (config.size() == 0)
	{
		std::cout << RED << "Error : server block missing" << RESET << std::endl;
		exit(1);
	}
	std::cout << GREEN << "Good : parsing" << RESET << std::endl;
}
