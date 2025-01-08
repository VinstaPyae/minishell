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
} 		t_token;

typedef enum e_token_type
{
    TOKEN_TXT,         // Regular word (e.g., "ls", "-l")
    TOKEN_PIPE,         // Pipe operator ("|")
    TOKEN_REDIRECT_IN,  // Input redirection ("<")
    TOKEN_REDIRECT_OUT, // Output redirection (">")
    TOKEN_OBRACKET,
    TOKEN_CBRACKET,      // Brackets ("(", ")")
    TOKEN_HDC,          // Heredoc ("<<")
    TOKEN_APPEND,
    TOKEN_SQUOTE,
    TOKEN_DQUOTE,        // Quotes ("'", "\"")
    TOKEN_COMMENT,
    TOKEN_EOF           // End of input
} t_token_type;

//lex_handle
int	lex_token_pipe(char *str, int *i, t_list **l_token);
//lex
t_list *get_token_list(char *input);
t_list	*lexer(char *input);
//token
t_list	*create_token(char *str, int type);

#endif