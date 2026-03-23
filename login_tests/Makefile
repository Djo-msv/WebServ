CC=c++
FLAGS=-Wall -Wextra -Werror
NAME=tester
SRC=main.cpp utils.cpp

all: $(NAME)

$(NAME): $(SRC)
	$(CC) $(FLAGS) -o $(NAME) $(SRC)

clean:
	rm -f $(NAME)

fclean: clean

re: fclean all

.PHONY: all re fclean clean
