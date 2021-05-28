##################################################
# COLOR
##################################################
GREY = \x1b[30m
RED = \x1b[31m
GREEN = \x1b[32m
YELLOW = \x1b[33m
BLUE = \x1b[34m
PURPLE = \x1b[35m
CYAN = \x1b[36m
WHITE= \x1b[37m
##################################################
# COMPILER
##################################################
NAME		=		webserv
HEADER		=		includes/all.hpp
CC			=		clang++
FLAGS		=		-Wall -Werror -Wextra -std=c++98
RM			=		rm -rf
##################################################
# FILES
##################################################
MAIN		=		main
PARSING		=		parsing utils
SERVER		=		server utils
UTILS		=		get_next_line get_next_line_utils ft_atoi
##################################################
# SRC
##################################################
SRC 		= 		$(addsuffix .cpp, $(MAIN)) \
						$(addprefix parsing/, $(addsuffix .cpp, $(PARSING))) \
						$(addprefix utils/, $(addsuffix .cpp, $(UTILS))) \
						$(addprefix server/, $(addsuffix .cpp, $(SERVER))) \

SRC_DIR		=		srcs

OBJ 		= 		$(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(MAIN))) \
						$(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(PARSING))) \
						$(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(UTILS))) \
						$(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(SERVER))) \

OBJ_DIR 	= 		objs
# OBJ_SUBDIR	=		$(addprefix $(OBJ_DIR)/, parsing utils)
OBJ_BUILD	=		$(addprefix $(OBJ_DIR)/, $(SRC:cpp=o))
##################################################
# MAKING
##################################################
all:				$(NAME)

$(NAME): 			$(OBJ_DIR) $(OBJ_BUILD)
					@$(CC) -o $(NAME) $(OBJ_BUILD)
					@echo "$(YELLOW)webserv \t:\t$(GREEN)[OK]$(WHITE)"

$(OBJ_DIR)/%.o: 	$(SRC_DIR)/%.cpp
					@echo "$(GREEN)[Compiling]:\t$(CYAN)"$<
					@$(CC) -c $< -o $@

$(OBJ_DIR):
					@mkdir -p $(OBJ_DIR)
					@mkdir -p objs/utils
					@mkdir -p objs/parsing
					@mkdir -p objs/server

clean:
					@$(RM) $(OBJ_DIR)
					@echo "${RED}[CLEAN]:\t$(YELLOW)objs"

fclean:				clean
					@$(RM) $(NAME)
					@echo "${RED}[CLEAN]:\t$(YELLOW)webserv"

re:					fclean all

.PHONY: 			all clean fclean re