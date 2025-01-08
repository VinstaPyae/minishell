# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: pzaw <pzaw@student.42.fr>                  +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/01/08 17:16:05 by pzaw              #+#    #+#              #
#    Updated: 2025/01/08 17:25:14 by pzaw             ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = minishell
CC = cc
CFLAGS = -Wall -Wextra -Werror
LIBFT_DIR = libft
LIBFT = $(LIBFT_DIR)/libft.a
INCLUDES = -I$(LIBFT_DIR) -I.
LDLIBS = -lreadline -lhistory
RM = rm -rf
SRCS = main.c lexer.c lex_handle.c token.c
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
