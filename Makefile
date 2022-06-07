NAME = webserv
SRCS = cgi.cpp cluster.cpp config.cpp main.cpp request.cpp response.cpp server.cpp
#FLAGS = -Wall -Wextra -Werror -std=c++98
FLAGS = -fsanitize=address -g
CC = c++
HEADER = cgi.hpp cluster.hpp config.hpp main.hpp request.hpp response.hpp server.hpp
all: $(NAME)

$(NAME): $(SRCS) $(HEADER)
	@$(CC) $(FLAGS) $(SRCS) -o $(NAME)

clean:

fclean: clean
	@rm -f $(NAME)
re: fclean all
