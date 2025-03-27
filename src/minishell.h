#ifndef MINISHELL_H
#define MINISHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h> // For va_list
#include <signal.h>
#include <termios.h> // For tcsetattr and terminal control
#include <sys/wait.h>
#include <sys/stat.h>
#include <errno.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "libft.h"

extern volatile sig_atomic_t g_signal_status;

typedef struct s_token
{
    int type;
    int space;
    char *token;
} t_token;

typedef enum e_redir_type
{
    REDIR_IN,
    REDIR_OUT,
    HDC,
    APPEND
} t_redir_type;

typedef struct s_redir
{
    int fd;
    int type;
    char *file;
} t_redir;

// AST type
typedef enum s_node_type
{
    NODE_PIPE,
    NODE_COMMAND
} t_node_type;

// AST struct
typedef struct s_ast_node
{
    t_node_type type;
    // char    *cmd;
    char **cmd_arg;
    t_list *redir;
    struct s_ast_node *left;
    struct s_ast_node *right;
} t_ast_node;

typedef enum e_token_type
{
    TOKEN_WD,           // Regular word (e.g., "ls", "-l")
    TOKEN_PIPE,         // Pipe operator ("|")
    TOKEN_REDIRECT_IN,  // Input redirection ("<")
    TOKEN_REDIRECT_OUT, // Output redirection (">")
    TOKEN_HDC,          // Heredoc ("<<")
    TOKEN_APPEND,
    TOKEN_SQUOTE,
    TOKEN_DQUOTE, // Quotes ("'", "\"")
    TOKEN_VARIABLE
} t_token_type;

typedef struct s_env
{
    char *key;
    char *value;
    struct s_env *next;
} t_env;

// minshell struct
typedef struct s_minishell
{
    char *input;
    t_list *l_token;
    t_ast_node *ast;
    t_env *envp;
    int exit_status;
} t_minishell;

// lex_handle
int lex_token_pipe(char *str, int *i, t_list **l_token);
// int	lex_token_bracket(char *str, int *i, t_list **l_token);
int lex_token_quote(char *str, int *i, t_list **l_token);
int lex_token_redirin_hdc(char *str, int *i, t_list **l_token);
int lex_token_redirout_app(char *str, int *i, t_list **l_token);
int lex_token_variable(char *str, int *i, t_list **l_token);
int lex_token_wd(char *str, int *i, t_list **l_token);

// lex
t_list *get_token_list(char *input);
t_list *lexer(char *input);
// token
t_list *create_token(char *str, int type, int s);
t_token *token_content(t_list *token);
void c_token_destroy(void *c_token);
void printer_token(t_list *l_token);
// utils
int quote_len(char *str);
int ot_len(char *input);
int wd_len(char *input);
int variable_len(char *input);
int ft_isspace(int c);
int ft_strcmp(const char *s1, const char *s2);

// utils_2
char **trim_cmd(char **cmd_arg);
char *trim_last_char(const char *s, char c);

// lex_gram
int check_pipe_grammar(t_list *l_token);
int lexer_syntax_check(t_list *l_token);

int check_redirect_in_grammar(t_list *l_token);
int check_redirect_out_grammar(t_list *l_token);
int check_append_grammar(t_list *l_token);
int check_heredoc_grammar(t_list *l_token);
// int check_word_grammar(t_list *l_token);

// redir
t_list *create_redir(char *file, int type);
int is_word_token(t_token_type type);
int is_redirection_token(t_token_type type);
void print_redir(t_list *redir);
int handle_redirections(t_list *redir_list);
int process_heredocs(t_ast_node *node);
int handle_heredoc(char *delimiter);

// expansion
void expand_tokens(t_minishell *shell);

// parser
t_ast_node *create_node(t_node_type type);
char **get_cmd_args(t_list **tokens);
// char	*get_cmd(t_list **tokens);
t_list *get_redir(t_list **tokens);
t_ast_node *parse_pipe(t_list *tokens);
t_ast_node *parse_cmd(t_list **tokens);

// execute_builtin
int builtin_cmd_check(t_minishell **shell);
int exe_cmd(t_minishell **shell);
int execute_ast_command(t_ast_node *cmd_node, t_minishell *shell);
int execute_ast(t_minishell **shell);
int n_option_checked(const char *str);
int exe_echo(t_minishell **shell);
int exe_exit(t_minishell **shell);
void split_key_value(char *str, char **key, char **value);
t_env *init_env(char **envp);
t_env *init_env(char **envp);
void print_env(t_minishell *shell);
int exe_env(t_minishell **shell);
char *ft_strndup(const char *s, size_t n);
int exe_unset(t_minishell **shell);
int exe_pwd(t_minishell **shell);
int exe_cd(t_minishell **shell);
void split_value(char *str, char **key, char **value);
static void add_or_update_env_var(const char *key, const char *value, t_minishell *shell);
static int is_valid_env_name(const char *name);
static int process_export_args(t_minishell *shell);
static int process_export_no_args(t_minishell *shell);
int exe_export(t_minishell **shell);
t_env *replace_or_add_env_var(const char *name, const char *value, t_env *envp);

// env
void update_shlvl(t_env **env_list);

// error_handle
void cleanup(t_minishell **shell);
void free_ast(t_ast_node *node);
void free_arg(char **str);
void free_redir(void *redir);
void free_env_list(t_env *head);
void remove_node(t_list **head, t_list *node_to_remove, void (*del)(void *));

void handle_sigquit(int signo);
void handle_sigint(int signo);
void setup_signal_handlers(void);
// debug
void print_error(const char *func_name, const char *file, int line, const char *format, ...);
void ft_fprintf(int fd, const char *format, ...);
#endif
