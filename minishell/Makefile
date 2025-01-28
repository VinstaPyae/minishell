# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: pzaw <pzaw@student.42.fr>                  +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/01/08 17:16:05 by pzaw              #+#    #+#              #
#    Updated: 2025/01/10 20:40:09 by pzaw             ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = minishell
CC = cc
CFLAGS = #-Wall -Wextra -Werror
LIBFT_DIR = libft
LIBFT = $(LIBFT_DIR)/libft.a
INCLUDES = -I$(LIBFT_DIR) -I.
LDLIBS = -lreadline -lhistory
RM = rm -rf
SRCS = main.c lexer.c lex_handle.c lex_wd.c token.c utils.c lex_gram_check.c error_handle.c debug.c parser.c redir.c execute.c exe_echo.c exe_env.c
SRC = $(addprefix ./src/,$(SRCS))
OBJ = $(SRC:.c=.o)

# Default target
all: $(NAME)

# Build Libft first if necessary
$(LIBFT):
	@echo "Building Libft..."
	@make -s -C $(LIBFT_DIR)

# Link the executable
$(NAME): $(LIBFT) $(OBJ)
	@echo "Linking $(NAME)..."
	@$(CC) $(CFLAGS) $(OBJ) $(LIBFT) $(LDLIBS) -o $(NAME)

# Compile source files into object files
./src/%.o: ./src/%.c
	@echo "Compiling $<..."
	@$(CC) $(CFLAGS) -c $< -o $@ $(INCLUDES)

# Clean object files
clean:
	@echo "Cleaning object files..."
	@$(RM) $(OBJ)
	@make -s -C $(LIBFT_DIR) clean

# Clean everything
fclean: clean
	@echo "Removing $(NAME)..."
	@$(RM) $(NAME)
	@make -s -C $(LIBFT_DIR) fclean

# Rebuild from scratch
re: fclean all
