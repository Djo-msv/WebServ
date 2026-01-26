CC=c++
FLAGS=-Wall -Wextra -Werror -std=c++98
NAME=parsing
SRC=main.cpp Request.cpp Response.cpp ProcessExecution.cpp

all: $(NAME)

$(NAME): $(SRC)
	$(CC) $(FLAGS) -o $(NAME) $(SRC)

clean:
	rm -f $(NAME)

fclean: clean

re: fclean all

.PHONY: all re fclean clean
