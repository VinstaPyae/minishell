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
    TOKEN_WD,         // Regular word (e.g., "ls", "-l")
    TOKEN_PIPE,         // Pipe operator ("|")
    TOKEN_REDIRECT_IN,  // Input redirection ("<")
    TOKEN_REDIRECT_OUT, // Output redirection (">")
    TOKEN_HDC,          // Heredoc ("<<")
    TOKEN_APPEND,
    TOKEN_SQUOTE,
    TOKEN_DQUOTE,        // Quotes ("'", "\"")
    TOKEN_COMMENT,
    TOKEN_VARIABLE,
    TOKEN_EOF           // End of input
} t_token_type;

//lex_handle
int	lex_token_pipe(char *str, int *i, t_list **l_token);
//int	lex_token_bracket(char *str, int *i, t_list **l_token);
int	lex_token_quote(char *str, int *i, t_list **l_token);
int	lex_token_redirin_hdc(char *str, int *i, t_list **l_token);
int	lex_token_redirout_app(char *str, int *i, t_list **l_token);
int	lex_token_variable(char *str, int *i, t_list **l_token);
int	lex_token_wd(char *str, int *i, t_list **l_token);



//lex
t_list *get_token_list(char *input);
t_list	*lexer(char *input);
//token
t_list	*create_token(char *str, int type);
t_token	*token_content(t_list *token);
void    c_token_destroy(void *c_token);
void	printer_token(t_list *l_token);
//utils
int	quote_len(char *str);
int	ot_len(char *input);
int	wd_len(char *input);
int	variable_len(char *input);
void	free_token_list(t_list **l_token);
int		ft_isspace(int c);
//lex_gram
int	check_pipe_grammar(t_list *l_token);
int	lexer_syntax_check(t_list *l_token);

int check_redirect_in_grammar(t_list *l_token);
int check_redirect_out_grammar(t_list *l_token);
int check_append_grammar(t_list *l_token);
int check_heredoc_grammar(t_list *l_token);
int check_word_grammar(t_list *l_token);
#endif
