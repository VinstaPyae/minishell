#ifndef MINISHELL_H
# define MINISHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "libft.h"

typedef struct	s_token
{
	int	type;
	char	*token;
} t_token;

typedef enum e_token_type
{
    TOKEN_WORD,         // Regular word (e.g., "ls", "-l")
    TOKEN_PIPE,         // Pipe operator ("|")
    TOKEN_REDIRECT_IN,  // Input redirection ("<")
    TOKEN_REDIRECT_OUT, // Output redirection (">")
    TOKEN_CMD,          // Command (e.g., "ls", "echo")
    TOKEN_BRACKET,      // Brackets ("(", ")")
    TOKEN_HDC,          // Heredoc ("<<")
    TOKEN_QUOTE,        // Quotes ("'", "\"")
    TOKEN_EOF           // End of input
} t_token_type;

//lexer
t_list *lexer(char *input);
t_list	*get_token_list(char *input);
void handle_redirect(char *input, int *i, t_list **list);
void handle_command(char *input, int *i, t_list **list);
void handle_bracket(char *input, int *i, t_list **list);
void handle_pipe(char *input, int *i, t_list **list);
void handle_quote(char *input, int *i, t_list **list);
void handle_word(char *input, int *i, t_list **list);

//token
t_token *create_token(t_token_type type, char *value);
t_list *create_list_node(t_token *token);
void add_to_list(t_list **list, t_list *new_node);
void free_list(t_list *list);

//main

#endif