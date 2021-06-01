#include "header.hpp"
#include "server.hpp"

int	main(int ac, char **av)
{
	int	count = 0;

	if (ac == 3)
		count = 1;
	else if (ac == 5)
		count = 2;
	else if (ac == 7)
		count = 3;
	else
	{
		std::cout << "Error arguments numbers" << std::endl;
		return (EXIT_FAILURE);
	}
	for (int server = 0; server < count; server++)
	{
		try
		{
			set_server_socket(server, av);
		}
		catch(const std::runtime_error& e)
		{
			std::cerr << e.what() << '\n';
		}
	}
		
	return (0);
}