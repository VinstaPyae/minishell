#ifndef MINISHELL_H
# define MINISHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h> // For va_list
#include <readline/readline.h>
#include <readline/history.h>
#include "libft.h"

typedef struct	s_token
{
	int	type;
	char	*token;
} 		t_token;

//AST type
typedef enum s_node_type
{
    NODE_COMMAND,
    NODE_PIPE,
    NODE_REDIRECT_IN,
    NODE_REDIRECT_OUT,
    NODE_APPEND,
    NODE_HEREDOC
} t_node_type;

//AST struct
typedef struct s_ast_node
{
    t_node_type type;
    char *data; // command, redirection operator, etc.
    struct s_ast_node *left;  // Left child (for commands before/after a pipe)
    struct s_ast_node *right; // Right child (for redirection or the next command)
} t_ast_node;

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
int		ft_isspace(int c);
//lex_gram
int	check_pipe_grammar(t_list *l_token);
int	lexer_syntax_check(t_list *l_token);

int check_redirect_in_grammar(t_list *l_token);
int check_redirect_out_grammar(t_list *l_token);
int check_append_grammar(t_list *l_token);
int check_heredoc_grammar(t_list *l_token);
int check_word_grammar(t_list *l_token);

//parser
t_ast_node *parse(t_list **tokens);
t_ast_node *create_ast_node(t_node_type type, char *data);
void free_ast(t_ast_node *node);
t_ast_node *parse_command(t_list **tokens);
t_ast_node *parse_pipe(t_list **tokens);
void print_ast_tree(t_ast_node *root);

//error_handle
void	cleanup(t_list **tokens, char **input, t_ast_node **ast);


//debug
void print_error(const char *func_name, const char *file, int line, const char *format, ...);
#endif
