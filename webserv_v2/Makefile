################################################################################
#									COLOR									   #
################################################################################

GREY = \x1b[30m
RED = \x1b[31m
GREEN = \x1b[32m
YELLOW = \x1b[33m
BLUE = \x1b[34m
PURPLE = \x1b[35m
CYAN = \x1b[36m
WHITE= \x1b[37m

################################################################################
#									VARIABLES								   #
################################################################################

NAME		= 		webserv
PATH_HEADER	=		includes/
PATH_SRCS	=		srcs/
PATH_OBJS	=		objs/
SRCS_LIST	= 		main.cpp parse_token.cpp parse_config.cpp parse_location.cpp server.cpp getRequest.cpp getResponse.cpp CGI.cpp getLocation.cpp
OBJS		= 		$(addprefix $(PATH_OBJS), $(SRCS_LIST:.cpp=.o))
HEADER_LIST	=		header.hpp config.hpp server.hpp getRequest.hpp getResponse.hpp CGI.hpp getLocation.hpp error_msg.hpp
HEADER		=		$(addprefix $(PATH_HEADER), $(HEADER_LIST))
CC			=		clang++
RM			= 		rm -rf
FLAGS 		= 		-Wall -Wextra -Werror -std=c++98 -fsanitize=address
FSANITIZE	=		-fsanitize=address

################################################################################
#									RULES									   #
################################################################################

all:				$(NAME)

$(PATH_OBJS)%.o	:	$(PATH_SRCS)%.cpp $(HEADER)
					@mkdir -p $(PATH_OBJS)
					$(CC) $(FLAGS) -c $< -o $@

$(NAME):			$(OBJS) $(HEADER)
					$(CC) $(FLAGS) $(OBJS) -o $(NAME)
					@rm -f ./tmp/php_content
					@echo "MODIFIER ICI AVANT DE PUSH POUR QUE WWW soit dans le bon dossier"
					@mkdir -p ./../www/upload
					@echo "$(BLUE)webserv : \t:\t$(GREEN)[OK]$(WHITE)"

clean:
					$(RM) $(PATH_OBJS)
					@echo "${RED}[CLEAN]:\t$(BLUE)objs"

fclean:				clean
					$(RM) $(NAME)
					@echo "${RED}[CLEAN]:\t$(BLUE)webserv"

re:					fclean all

.PHONY: 			all clean fclean re