#ifndef ALL_HPP
# define ALL_HPP

# define BLACK   "\033[1m\033[30m"       /* Bold Black */
# define RED     "\033[1m\033[31m"      /* Bold Red */
# define GREEN   "\033[1m\033[32m"       /* Bold Green */
# define YELLOW  "\033[1m\033[33m"       /* Bold Yellow */
# define BLUE    "\033[1m\033[34m"       /* Bold Blue */
# define MAGENTA "\033[1m\033[35m"       /* Bold Magenta */
# define CYAN    "\033[1m\033[36m"	    /* Bold Cyan */
# define WHITE   "\033[1m\033[37m"
# define RESET   "\033[0m"

# include <sys/socket.h> // For socket functions
# include <netinet/in.h> // For sockaddr_in
# include <arpa/inet.h> // For inet_addr
# include <iostream>
// # include <stdlib.h>
# include <list>
# include <map>
// # include <string>
# include <unistd.h>
# include <csignal>
# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>
# include "utils.hpp"

#endif