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
CC = cc -g
CFLAGS = -Wall -Wextra -Werror
LIBFT_DIR = libft
LIBFT = $(LIBFT_DIR)/libft.a
INCLUDES = -I$(LIBFT_DIR) -I.
LDLIBS = -lreadline -lhistory
RM = rm -rf
SRCS = main.c lexer.c lex_handle.c lex_wd.c token.c utils.c lex_gram_check.c error_handle.c error_handle1.c \
	debug.c parser.c redir.c execute.c exe_echo.c exe_exit.c exe_env.c exe_unset.c exe_pwd.c exe_cd.c exe_cd1.c\
	exe_export.c exe_export1.c expan_hep1.c expan_hep2.c expan_hep3.c expan_hep4.c expan_hep5.c expansion.c utils_2.c env.c env1.c env2.c status_utils.c exe_cmd.c exe_check_cmd.c
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


