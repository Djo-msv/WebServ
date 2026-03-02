CC=c++
FLAGS=-Wall -Werror -Wextra -std=c++98
NAME=parsingYml
SRCS=Lexer/File.cpp \
	Lexer/Parser.cpp \
	Lexer/Tokenizer.cpp \
	Lexer/TokenTransformer.cpp \
	Lexer/Lexer.cpp \
	main.cpp
	#Tree/MymlObject.cpp \
	#Tree/MymlTree.cpp \
	#Lexer/Lexer.cpp \
	#Lexer/TokenTransformer.cpp \
all: $(NAME)

$(NAME): $(SRCS)
	$(CC) $(FLAGS) $(SRCS) -o $(NAME)

fclean:
	rm -f $(NAME)
