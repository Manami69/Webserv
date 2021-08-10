#ifndef HEADER_HPP
# define HEADER_HPP

# define BLACK   "\033[1m\033[30m"       /* Bold Black */
# define RED     "\033[1m\033[31m"      /* Bold Red */
# define GREEN   "\033[1m\033[32m"       /* Bold Green */
# define YELLOW  "\033[1m\033[33m"       /* Bold Yellow */
# define BLUE    "\033[1m\033[34m"       /* Bold Blue */
# define MAGENTA "\033[1m\033[35m"       /* Bold Magenta */
# define CYAN    "\033[1m\033[36m"	    /* Bold Cyan */
# define WHITE   "\033[1m\033[37m"
# define RESET   "\033[0m"
# define NOTFOUND std::string::npos
# define GET 0
# define POST 1
# define DELETE 2
# define DEFAULT_LOCATION "./www"
# define PHP_CONTENT "./tmp/php_content"

# include <sys/socket.h> // For socket functions
# include <netinet/in.h> // For sockaddr_in
# include <arpa/inet.h> // For inet_addr
# include <iostream>
# include <cstring>
# include <sys/select.h>
# include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
# include <list>
# include <cerrno>
# include <map>
# include <vector>
# include <string>
# include <unistd.h>
# include <cstdlib> // exit() and EXIT_FAILURE
# include <fcntl.h>
# include <stdexcept> 
# include <stdlib.h> //DELETE LATER
# include <iterator> // distance
# include <fstream>
# include <sstream>
# include <algorithm>
# include <dirent.h>
# include <iomanip>
# include <sys/stat.h>
# include <ctime>
# include <sys/types.h>
# include <sys/wait.h>
# include <string.h>
# include <locale>

#endif