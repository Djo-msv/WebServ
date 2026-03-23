NAME := webserv
MAKEFLAGS += --no-print-directory

#==============================COMPIL===========================#

CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98

ifeq ($(DEBUG), 1)
	CFLAGS += -g
endif

#==============================COLORS==============================#
NOC			= \e[0m
BOLD		= \e[1m
DIM			= \e[2m
UNDERLINE	= \e[4m
BLACK		= \e[1;30m
RED			= \e[1m\e[38;5;196m
GREEN		= \e[1m\e[38;5;76m
YELLOW		= \e[1m\e[38;5;220m
BLUE		= \e[1m\e[38;5;33m
PURPLE		= \e[1;35m
CYAN		= \e[1;36m
WHITE		= \e[1;37m
SPECIAL		= \e[1m\e[38;5;223m

BLACK_BG 	= \033[0;40m
RED_BG 		= \033[0;41m
GREEN_BG 	= \033[0;42m
YELLOW_BG 	= \033[0;43m
BLUE_BG 	= \033[0;44m
MAGENTA_BG 	= \033[0;45m
CYAN_BG 	= \033[0;46m
WHITE_BG 	= \033[0;47m
RESET_BG	= \033[0m

#================================DIRS============================#

SRC_DIR				:=  srcs
HEADER_DIR			:=	includes
BUILD_DIR			:=	.build

#==============================OS================================#

UNAME = $(shell uname)

ifeq ($(UNAME), Linux)
	OS = 0
else
	OS = 1
endif

#==============================SOURCES===========================#

SRCS_FILES:=	main.cpp \
				mimetype.cpp \
				HttpError.cpp \
				Execution/ProcessExecution.cpp \
				Parser/Tree/MymlObject.cpp \
				Parser/Tree/MymlDictionary.cpp \
				Parser/Tree/MymlList.cpp \
				Parser/Tree/MymlPair.cpp \
				Parser/Tree/MymlTree.cpp \
				Parser/Tree/treeUtils.cpp \
				Parser/Lexer/Parser.cpp \
				Parser/Lexer/Tokenizer.cpp \
				Parser/Lexer/File.cpp \
				Parser/Lexer/Lexer.cpp \
				Parser/Lexer/TokenTransformer.cpp \
				Parser/ParserError.cpp \
				Socket/ClientSocket.cpp \
				Socket/ServerSocket.cpp \
				Socket/ServerConfig.cpp \
				Socket/Socket.cpp \
				Socket/socket_utils.cpp \
				Message/Request.cpp \
				Message/request_utils.cpp \
				Message/Response.cpp

SRCS:=			$(addprefix $(SRC_DIR)/, $(SRCS_FILES))

#================================COUNT============================#

NB_COMP	:=	1
TO_COMP	:=	$(words $(SRCS))
PERCENT	:= 0

#=============================OBJECTS===========================#

OBJS:=			${SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o}

#===============================DEPS=============================#

DEPS:=			${SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.d}

#=============================INCLUDES===========================#

INC := -I$(HEADER_DIR)

#================================DIR=============================#

DIRS			:=	$(sort $(shell dirname $(OBJS))) #no duplicate

#===============================RULES============================#

all: $(NAME)	

$(DIRS):
	@mkdir -p $@

$(NAME): $(OBJS)
	@echo "\n$(GREEN)Create binaries$(NOC)"
	@$(CC) $(CFLAGS) $(OBJS) $(INC) -o $@ $(LIBS) $(LDFLAGS) -lm
	@echo "$(PURPLE) $(BOLD)"
	@printf "%s\n" \
	' __      __          __          ____                       ' \
	'/\ \  __/\ \        /\ \        /\  _`\                           ' \
	'\ \ \/\ \ \ \     __\ \ \____   \ \,\L\_\     __   _ __   __  __  ' \
	' \ \ \ \ \ \ \  /`__`\ \ `__`\   \/_\__ \   /`__`\/\``__\/\ \/\ \ ' \
	'  \ \ \_/ \_\ \/\  __/\ \ \L\ \    /\ \L\ \/\  __/\ \ \/ \ \ \_/ |' \
	'   \ `\___x___/\ \____\\ \_,__/    \ `\____\ \____\\ \_\  \ \___/ ' \
	'    `\/__//__/  \/____/ \/___/      \/_____/\/____/ \/_/   \/__/  '
	@echo "$(NOC)"
	@if [ 0 -eq 0 ]; then echo "os : Linux"; else echo "os : Mac/Win"; fi


$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(DIRS)
	@mkdir -p $(BUILD_DIR)
	@if [ $(NB_COMP) -eq 1 ]; then echo "\n$(BOLD)Compilation of source files :$(NOC)";fi
	$(eval PERCENT=$(shell expr $(NB_COMP)00 "/" $(TO_COMP)))
	@if [ $(PERCENT) -le 30 ]; then echo -n "$(RED)"; elif [ $(PERCENT) -le 66 ]; then echo -n "$(YELLOW)"; elif [ $(PERCENT) -gt 66 ]; then echo -n "$(GREEN)"; fi
	@echo -n "\r"; for i in $$(seq 1 $$(/usr/bin/tput cols)); do echo -n " "; done
	@echo -n "\r"; for i in $$(seq 1 25); do if [ $$(expr $$i "*" 4) -le $(PERCENT) ]; then echo -n "█"; else echo -n " "; fi; done; echo -n "";
	@printf " $(NB_COMP)/$(TO_COMP) - Compiling $<"
	@echo -n "$(NOC)"
	@$(CC) $(CFLAGS) $(INC) $< -c -o $@
	$(eval NB_COMP=$(shell expr $(NB_COMP) + 1))


clean:
	@echo "$(RED)Remove objects$(NOC)"
	@rm -rf $(BUILD_DIR) 
	@rm -rf $(BUILD_DIR_BONUS)

fclean: clean
	@echo "$(RED)Remove binary$(NOC)"
	@rm -f $(NAME)
	@rm -f $(BONUS_NAME)

re: fclean
	@make

rebonus: fclean
	@make bonus

.PHONY: all clean fclean re rebonus

-include $(DEPS)
