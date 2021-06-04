#include "server.hpp"

void	run_server(char **av) {
	Server	server;

	for (int i = 1; av[i]; i++)
	{
		server.setup_server_socket(av[i]);
		server.set_socket_reuse();
		server.binded();
		server.listened();
		server.add_server_lst();
	}
	std::cout << "Server number: " << server.get_server_nbr() << std::endl;
}

int	main(int ac, char **av) {

	// Enter arguments to set listening port

	if (ac == 1) {
		std::cout << "Error: need at least one argument." << std::endl;
		return (1);
	}
	else {
		try
		{
			run_server(av);
		}
		catch(const std::runtime_error& e)
		{
			std::cerr << e.what() << '\n';
		}
	}
}