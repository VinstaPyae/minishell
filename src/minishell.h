/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pzaw <pzaw@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 03:38:05 by pzaw              #+#    #+#             */
/*   Updated: 2025/04/24 03:38:05 by pzaw             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdio.h>
# include <stdlib.h>
# include <stdarg.h>
# include <signal.h>
# include <termios.h>
# include <sys/wait.h>
# include <sys/stat.h>
# include <errno.h>
# include <readline/readline.h>
# include <readline/history.h>
# include "libft.h"

extern volatile sig_atomic_t	g_signal_status;

typedef struct s_token
{
	int		type;
	int		space;
	char	*token;
}	t_token;

typedef enum e_redir_type
{
	REDIR_IN,
	REDIR_OUT,
	HDC,
	APPEND
}	t_redir_type;

typedef enum e_fd_type
{
	FD_IN,
	FD_OUT,
	FD_ERR
}	t_fd_type;

typedef struct s_redir
{
	int		fd;
	int		type;
	char	*file;
}	t_redir;

typedef enum s_node_type
{
	NODE_PIPE,
	NODE_COMMAND
}	t_node_type;

typedef struct s_ast_node
{
	t_node_type			type;
	char				**cmd_arg;
	t_list				*redir;
	struct s_ast_node	*left;
	struct s_ast_node	*right;
}	t_ast_node;

typedef enum e_token_type
{
	TOKEN_WD,
	TOKEN_PIPE,
	TOKEN_REDIRECT_IN,
	TOKEN_REDIRECT_OUT,
	TOKEN_HDC,
	TOKEN_APPEND,
	TOKEN_SQUOTE,
	TOKEN_DQUOTE,
	TOKEN_VARIABLE
}	t_token_type;

typedef enum e_error_cmd
{
	OK_CMD,
	CMD_NOT_FOUND,
	CMD_IS_DIR,
	CMD_NO_PERM,
	CMD_NO_FILE,
}	t_error_cmd;

typedef struct s_env
{
	char			*key;
	char			*value;
	struct s_env	*next;
}	t_env;

// minshell struct
typedef struct s_minishell
{
	char		*input;
	t_list		*l_token;
	t_ast_node	*ast;
	t_env		*envp;
	char		**env_path;
	char		*path;
	int			exit_status;
	int			og_fd[2];
}	t_minishell;

typedef struct s_lee
{
	int	a;
	int	b;
	int	c;
	int	d;
}	t_lee;

t_minishell	*initialize_shell(char **env);
t_ast_node	*create_node(t_node_type type);
int			lex_token_pipe(char *str, int *i, t_list **l_token);
int			lex_token_quote(char *str, int *i, t_list **l_token, int c_space);
int			lex_token_redirin_hdc(char *str, int *i, t_list **l_token);
int			lex_token_redirout_app(char *str, int *i, t_list **l_token);
int			lex_token_variable(char *str, int *i, t_list **l_token);
int			lex_token_wd(char *str, int *i, t_list **l_token);
t_list		*get_token_list(char *input);
t_list		*lexer(t_minishell *shell);
t_list		*create_token(char *str, int type, int s);
t_token		*token_content(t_list *token);
void		c_token_destroy(void *c_token);
int			handle_input_and_signals(t_minishell *shell);
int			quote_len(char *str);
int			ot_len(char *input);
int			wd_len(char *input);
int			variable_len(char *input);
int			ft_isspace(int c);
int			ft_strcmp(const char *s1, const char *s2);
char		**trim_cmd(char **cmd_arg);
char		*trim_last_char(const char *s, char c);
void		handle_eof(t_minishell *shell);
int			check_pipe_grammar(t_list *l_token);
int			lexer_syntax_check(t_list *l_token);
int			check_redirect_in_grammar(t_list *l_token);
int			check_redirect_out_grammar(t_list *l_token);
int			check_append_grammar(t_list *l_token);
int			check_heredoc_grammar(t_list *l_token, t_token *next);
t_list		*create_redir(char *file, int type);
void		close_heredoc_fds(t_ast_node *node);
void		print_redir(t_list *redir);
int			process_heredocs(t_ast_node *node, t_minishell *shell);
char		*append_expanded_heredoc(char *result,
				char *var_name, t_minishell *shell);
void		write_expanded_line_to_pipe(char *expanded_line, int pipefd);
char		*process_heredoc_line(char *line, char *delimiter,
				t_minishell *shell, int *should_break);
char		*expand_heredoc(char *input, t_minishell *shell);
char		*extract_heredoc_name(const char *input, int *i);
void		process_eof_warning(char *delimiter);
int			handle_output_redirection(t_redir *redir);
int			handle_input_redirection(t_redir *redir);
int			is_word_token(t_token_type type);
int			is_redirection_token(t_token_type type);
int			handle_redirections(t_list *redir_list);
int			handle_heredoc(char *delimiter, t_minishell *shell);
char		*get_env_value(t_env *env, char *key);
char		**create_single_result(char *str);
char		**expand_dollar_sign(void);
char		**expand_exit_status(t_minishell *shell);
//expan_hep2
char		**handle_empty_var_name(void);
char		**get_valid_env_value(char **var_name, int c, t_minishell *shell);
char		**expand_env_variable(char **var_name, t_minishell *shell);
void		debug_print_expansion(char **result);
char		**expand_variable(char *var, t_minishell *shell);
//expan_hep3
char		*expand_quote_variable(char *var, t_minishell *shell);
char		*extract_variable_name(const char *input, int *i);
char		*append_expanded_variable(char *result,
				char *var_name, t_minishell *shell);
char		*append_normal_character(char *result, char c);
char		*expand_double_quotes(char *input, t_minishell *shell);
//expan_hep4
void		update_token_with_expansion(t_token *token,
				char *expanded_value, int space);
void		handle_empty_expansion(t_token *token);
void		process_double_quote_token(t_token *token, t_minishell *shell);
int			count_words(char *expanded_value);
char		**allocate_result_array(int word_count);
//expan_hep5
char		*extract_word(char *expanded_value,
				int start, int len);
int			populate_result_array(char **result,
				char *expanded_value, int word_count);
char		**split_expanded_value(char *expanded_value);
char		*join_expanded_values(char **expanded_value);
char		**handle_split_value(char *result, char **split_value);
// expansion
void		expand_tokens(t_minishell *shell);
// parser
char		**get_cmd_args(t_list **tokens);
//parser1
int			is_pipe_token(t_list *tokens);
int			process_redirections(t_ast_node *cmd_node, t_list **tokens);
int			count_strs(char **arr);
int			duplicate_strings(char **dest, char **src, int *index);
int			calculate_total_length(char **arg, char **new);
// parser2
char		**join_args(char **arg, char **new);
int			handle_existing_arguments(t_ast_node *cmd_node, t_list **tokens);
int			handle_new_arguments(t_ast_node *cmd_node, t_list **tokens);
int			trim_first_argument(char **tmp_arg, t_list *tmp_list);
int			process_command_arguments(t_ast_node *cmd_node, t_list **tokens);
t_list		*process_redirection(t_list **redir,
				t_token *current_token, t_list *tmp_list);
// parser3
t_ast_node	*create_command_node(void);
int			count_parse_arguments(t_list *tokens);
char		*join_tokens(t_list **tmp_list, char *arg);
int			validate_redirection(t_list *tmp_list);
// parser4
t_list		*handle_invalid_redirection(t_list **redir);
t_ast_node	*create_pipe_node(t_list *tokens, t_ast_node *left);
t_list		*get_redir(t_list **tokens);
char		*get_argument(t_list **tmp_list, char *tmp, t_list *tokens);
int			allocate_cmd_args(char ***cmd_arg, int arg_count, char **tmp);
// char	*get_cmd(t_list **tokens);
t_ast_node	*parse_pipe(t_list *tokens);
t_ast_node	*parse_cmd(t_list **tokens);
t_error_cmd	search_cmd_path(char *cmd, t_minishell *shell);
//execute1
char		*ft_getenv(t_env *env, const char *key);
char		*create_env_entry(char	*key, char	*value);
char		**fill_env_array(char	**env_array, t_env	*tmp, int i);
char		**env_list_to_array(t_env	*env);
void		handle_child_signals(void);
// execute2
void		print_error_message(char	*cmd, char	*message);
char		**get_env_array(t_minishell	*shell);
int			handle_no_path(char	*cmd, t_minishell	*shell);
void		print_signal_message(int sig);
void		reset_close_fd(int *org_fd);
//execute3
int			create_pipe(int pipe_fds[2], t_ast_node *pipe_node);
void		handle_left_child(t_ast_node *pipe_node,
				t_minishell *shell, int pipe_fds[2]);
void		handle_right_child(t_ast_node *pipe_node,
				t_minishell *shell, int pipe_fds[2]);
// execute_builtin
//void close_og_fd(t_minishell *shell);
int			builtin_cmd_check(t_ast_node *ast, t_minishell *shell);
int			exe_cmd(t_ast_node *left_node, t_minishell *shell);
int			execute_ast(t_ast_node *ast_root, t_minishell *shell);
void		split_key_value(char *str, char **key, char **value);
t_env		*init_env(char **envp);
t_env		*init_env(char **envp);
void		print_env(t_minishell *shell);
int			exe_env(t_minishell **shell);
char		*ft_strndup(const char *s, size_t n);
int			exe_pwd(t_minishell **shell);
int			exe_cd(t_ast_node *ast, t_minishell *shell);
void		split_value(char *str, char **key, char **value);
int			exe_export(t_ast_node *ast, t_minishell *shell);
t_env		*replace_or_add_env_var(const char *name,
				const char *value, t_env *envp);
//exe_cd1
t_env		*find_and_update_env_var(const char *name,
				const char *value, t_env *envp, int *found);
t_env		*add_new_env_var(const char *name, const char *value, t_env *envp);
//exe_unset
int			exe_unset(t_minishell **shell);
void		handle_first_split_value(t_token *token,
				char **split_value, int i, int space1);
t_list		*create_and_insert_node(t_list *insert_pos, char *value, int space);
t_list		*handle_remaining_split_values(t_list *insert_pos,
				char **split_value, int i, int space1);
// exe env
t_env		*search_env_list(t_env *env_list, const char *name);
//exe_echo
int			exe_echo(t_ast_node *ast);
int			is_llong_min(const char *str, int start);
int			skip_whitespace_and_sign(const char *str, int *sign, int *index);
int			handle_llong_min(const char *str,
				int sign, int index, long long *num);
int			convert_to_long_long(const char *str,
				int index, long long *res);
//exe_export1
void		handle_split_with_equal_sign(char *str,
				char *equal_sign, char **key, char **value);
void		split_value(char *str, char **key, char **value);
int			init_new_env_var(const char *key,
				const char *value, t_env **new_var);
void		new_var_update(const char *key,
				const char *value, t_env *prev, t_minishell *shell);
void		add_or_update_env_var(const char *key,
				const char *value, t_minishell *shell);
//exe_exit
int			exe_exit(t_minishell **shell, t_ast_node *ast);
// execute
int			wait_for_child(pid_t pid);
// env
void		update_shlvl(t_env **env_list);
t_env		*init_minimal_env(void);
//env1
t_env		*init_env(char **envp);
t_env		*find_shlvl_node(t_env *env_list);
void		increment_shlvl_value(t_env *shlvl_node);
void		add_shlvl_if_missing(t_env **env_list);
void		update_shlvl(t_env **env_list);
//env2
t_env		*handle_minimal_env(void);
t_env		*create_env_node(char *key, char *value, t_env *env);
int			process_envp_entry(char *envp_entry, t_env **env);
t_env		*init_env_from_envp(char **envp);
// error_handle
int			cmd_error_msg(t_error_cmd cmd_err,
				char *cmd, t_minishell *shell);
void		cleanup(t_minishell **shell);
void		free_ast(t_ast_node *node);
void		free_arg(char **str);
void		free_redir(void *redir);
void		free_env_list(t_env *head);
void		remove_node(t_list **head,
				t_list *node_to_remove, void (*del)(void *));
//void handle_sigquit(int signo);
void		handle_sigint(int signo);
void		setup_signal_handlers(void);
// debug
void		init_tmp(int *tmp);
void		free_array_list(char **path_dirs, int index);
int			n_option_checked(const char *str);
int			check_sigint(void);
void		handle_sigint_heredoc(int signo);
// status_utils
t_minishell	*create_minshell(t_env *envp);
void		set_exit_status(t_minishell *shell, int status);
int			return_with_status(t_minishell *shell, int status);
int			return_error(t_minishell **shell,
				const char *msg, int status);
int			handle_input_and_signals(t_minishell *shell);
//main
char		*get_input(t_minishell *shell);
#endif
