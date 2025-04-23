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

typedef enum e_fd_type
{
    FD_IN,
    FD_OUT,
    FD_ERR
} t_fd_type;

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

typedef enum e_error_cmd
{
    OK_CMD,
    CMD_NOT_FOUND,
    CMD_IS_DIR,
    CMD_NO_PERM,
    CMD_NO_FILE,
} t_error_cmd;

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
    char **env_path;
    char *path;
    int exit_status;
    int og_fd[2];
    //int signal_printed; // Flag to track signal message
} t_minishell;

typedef struct s_lee
{
    int a;
    int b;
    int c;
    int d;
} t_lee;

// lex_handle
int lex_token_pipe(char *str, int *i, t_list **l_token);
// int	lex_token_bracket(char *str, int *i, t_list **l_token);
int lex_token_quote(char *str, int *i, t_list **l_token, int c_space);
int lex_token_redirin_hdc(char *str, int *i, t_list **l_token);
int lex_token_redirout_app(char *str, int *i, t_list **l_token);
int lex_token_variable(char *str, int *i, t_list **l_token);
int lex_token_wd(char *str, int *i, t_list **l_token);

// lex
t_list *get_token_list(char *input);
t_list *lexer(t_minishell *shell);
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
void print_ast_node(t_ast_node *node);
void print_redirs(t_list *redir_list);

// utils_2
char **trim_cmd(char **cmd_arg);
char *trim_last_char(const char *s, char c);
void handle_eof(t_minishell *shell);

// lex_gram
int check_pipe_grammar(t_list *l_token);
int lexer_syntax_check(t_list *l_token);

int check_redirect_in_grammar(t_list *l_token);
int check_redirect_out_grammar(t_list *l_token);
int check_append_grammar(t_list *l_token);
int check_heredoc_grammar(t_list *l_token, t_token *next);
// int check_word_grammar(t_list *l_token);

// redir
t_list *create_redir(char *file, int type);
int is_word_token(t_token_type type);
int is_redirection_token(t_token_type type);
void print_redir(t_list *redir);
int handle_redirections(t_list *redir_list);
int process_heredocs(t_ast_node *node, t_minishell *shell);
int handle_heredoc(char *delimiter, t_minishell *shell);
void close_heredoc_fds(t_ast_node *node);

// expansion
void expand_tokens(t_minishell *shell);
char	*expand_quote_variable(char *var, t_minishell *shell);
char *extract_variable_name(const char *input, int *i);
char *append_expanded_variable(char *result, char *var_name, t_minishell *shell);
char *append_normal_character(char *result, char c);
char *get_env_value(t_env *env, char *key);

// parser
t_ast_node *create_node(t_node_type type);
char **get_cmd_args(t_list **tokens);
// char	*get_cmd(t_list **tokens);
t_list *get_redir(t_list **tokens);
t_ast_node *parse_pipe(t_list *tokens);
t_ast_node *parse_cmd(t_list **tokens);

// exe_check_cmd
t_error_cmd search_cmd_path(char *cmd, t_minishell *shell);

// execute_builtin
char *ft_getenv(t_env *env, const char *key);
//void close_og_fd(t_minishell *shell);
int builtin_cmd_check(t_ast_node *ast, t_minishell *shell);
int exe_cmd(t_ast_node *left_node, t_minishell *shell);
int execute_ast(t_ast_node *ast_root, t_minishell *shell);
int n_option_checked(const char *str);
int exe_echo(t_ast_node *ast);
int exe_exit(t_minishell **shell, t_ast_node *ast);
void split_key_value(char *str, char **key, char **value);
t_env *init_env(char **envp);
t_env *init_env(char **envp);
void print_env(t_minishell *shell);
int exe_env(t_minishell **shell);
char *ft_strndup(const char *s, size_t n);
int exe_unset(t_minishell **shell);
int exe_pwd(t_minishell **shell);
int exe_cd(t_ast_node *ast, t_minishell *shell);
void split_value(char *str, char **key, char **value);
int exe_export(t_ast_node *ast, t_minishell *shell);
t_env *replace_or_add_env_var(const char *name, const char *value, t_env *envp);

// exe env
t_env	*search_env_list(t_env *env_list, const char *name);

// execute
void print_error_message(char *cmd, char *message);
void handle_child_signals(void);
int handle_no_path(char *cmd, t_minishell *shell);
char **get_env_array(t_minishell *shell);
//char **split_path(char *path_env);
//int execute_left_command(t_ast_node *cmd_node, t_minishell *shell);
void reset_close_fd(int *org_fd);
int wait_for_child(pid_t pid);
//

// env
void update_shlvl(t_env **env_list);
t_env *init_minimal_env(void);
//env1
t_env *init_env(char **envp);
t_env *find_shlvl_node(t_env *env_list);
void increment_shlvl_value(t_env *shlvl_node);
void add_shlvl_if_missing(t_env **env_list);
void update_shlvl(t_env **env_list);
//env2
t_env *handle_minimal_env(void);
t_env *create_env_node(char *key, char *value, t_env *env);
int process_envp_entry(char *envp_entry, t_env **env);
t_env *init_env_from_envp(char **envp);
// error_handle
int cmd_error_msg(t_error_cmd cmd_err, char *cmd, t_minishell *shell);
void cleanup(t_minishell **shell);
void free_ast(t_ast_node *node);
void free_arg(char **str);
void free_redir(void *redir);
void free_env_list(t_env *head);
void remove_node(t_list **head, t_list *node_to_remove, void (*del)(void *));

//void handle_sigquit(int signo);
void handle_sigint(int signo);
void setup_signal_handlers(void);
// debug
// void print_error(const char *func_name, const char *file, int line, const char *format, ...);
void free_array_list(char **path_dirs, int index);

int check_sigint(void);
void handle_sigint_heredoc(int signo);

void print_signal_message(int sig);
// status_utils
void set_exit_status(t_minishell *shell, int status);
int return_with_status(t_minishell *shell, int status);
int return_error(t_minishell **shell, const char *msg, int status);
#endif
