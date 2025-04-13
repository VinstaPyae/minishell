#include "minishell.h"

char *ft_getenv(t_env *env, const char *key)
{
	while (env)
	{
		if (ft_strcmp(env->key, key) == 0)
			return env->value;
		env = env->next;
	}
	return NULL;
}

void close_og_fd(t_minishell *shell)
{
	close(shell->og_fd[1]);
	close(shell->og_fd[0]);
}

static char **allocate_paths(char *path_env)
{
	int count;
	char **path_dirs;

	count = 1;
	if (!path_env) // Handle NULL case
		return (NULL);
	while (*path_env)
	{
		if (*path_env == ':')
			count++;
		path_env++;
	}
	path_dirs = malloc((count + 1) * sizeof(char *));
	if (!path_dirs)
		return (NULL);
	return (path_dirs);
}

char **split_path(char *path_env)
{
	char **path_dirs;
	char *start;
	int index;
	int count;

	path_dirs = allocate_paths(path_env);
	if (!path_dirs)
		return (NULL);
	start = path_env;
	index = 0;
	while (*path_env)
	{
		if (*path_env == ':' || !*(path_env + 1))
		{
			count = path_env - start + (!*(path_env + 1));
			path_dirs[index] = ft_strndup(start, count);
			if (!path_dirs[index])
				return (free_array_list(path_dirs, index), NULL);
			index++;
			start = path_env + 1;
		}
		path_env++;
	}
	path_dirs[index] = NULL;
	return (path_dirs);
}
////////////////////////////////
char *create_env_entry(char *key, char *value)
{
	char *entry;
	size_t key_len;
	size_t value_len;
	size_t total_len;

	key_len = ft_strlen(key);
	value_len = ft_strlen(value);
	total_len = key_len + value_len + 2; // +1 for '=', +1 for '\0'
	entry = malloc(total_len);
	if (!entry)
		return (NULL);

	ft_memcpy(entry, key, key_len);
	entry[key_len] = '=';
	ft_memcpy(entry + key_len + 1, value, value_len);
	entry[total_len - 1] = '\0';
	return (entry);
}

char **fill_env_array(char **env_array, t_env *tmp, int i)
{

	while (tmp != NULL)
	{
		if (tmp->key != NULL && tmp->value != NULL)
		{
			env_array[i] = create_env_entry(tmp->key, tmp->value);
			if (env_array[i] == NULL)
			{
				free_array_list(env_array, i);
				return (NULL);
			}
			i++;
		}
		tmp = tmp->next;
	}
	env_array[i] = NULL;
	return (env_array);
}

char **env_list_to_array(t_env *env)
{
	int count;
	t_env *tmp;
	char **env_array;
	int i;

	count = 0;
	tmp = env;
	i = 0;
	while (tmp != NULL)
	{
		if (tmp->key != NULL && tmp->value != NULL)
			count++;
		tmp = tmp->next;
	}
	env_array = malloc((count + 1) * sizeof(char *));
	if (env_array == NULL)
		return (NULL);
	tmp = env;
	env_array = fill_env_array(env_array, tmp, i);
	return (env_array);
}
///////////

void handle_child_signals(void)
{
	struct sigaction sa;

	sa.sa_handler = SIG_DFL;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGQUIT, &sa, NULL);
	sigaction(SIGTSTP, &sa, NULL);
}

void print_error_message(char *cmd, char *message)
{
	write(STDERR_FILENO, cmd, ft_strlen(cmd));
	write(STDERR_FILENO, ": ", 2);
	write(STDERR_FILENO, message, ft_strlen(message));
	write(STDERR_FILENO, "\n", 1);
}

int is_directory(char *cmd, t_minishell *shell)
{
	struct stat sb;

	if (stat(cmd, &sb) == 0 && S_ISDIR(sb.st_mode))
	{
		print_error_message(cmd, "Is a directory");
		set_exit_status(shell, 126);
		return (1);
	}
	return (0);
}

char **get_env_array(t_minishell *shell)
{
	char **env_array;

	env_array = env_list_to_array(shell->envp);
	if (!env_array)
	{
		perror("env_list_to_array");
		set_exit_status(shell, 127);
	}
	return (env_array);
}
int handle_no_path(char *cmd, t_minishell *shell)
{
	print_error_message(cmd, "No such file or directory");
	return (return_with_status(&shell, 127));
}

void print_signal_message(int sig)
{
	if (isatty(STDERR_FILENO))
	{
		if (sig == SIGQUIT)
			write(STDERR_FILENO, "Quit\n", 4);
	}
	write(STDERR_FILENO, "\n", 1);
}

/* Helper function to create a child process for pipe execution */
static pid_t create_pipe_child(t_ast_node *node, t_minishell *shell, int in_fd, int out_fd)
{
	pid_t pid;

	pid = fork();
	if (pid < 0)
	{
		perror("minishell: fork");
		return -1;
	}

	if (pid == 0)
	{
		/* Child process */
		handle_child_signals();

		/* Set up input redirection */
		if (in_fd != STDIN_FILENO)
		{
			if (dup2(in_fd, STDIN_FILENO) == -1)
			{
				perror("minishell: dup2");
				exit(EXIT_FAILURE);
			}
			close(in_fd);
		}

		/* Set up output redirection */
		if (out_fd != STDOUT_FILENO)
		{
			if (dup2(out_fd, STDOUT_FILENO) == -1)
			{
				perror("minishell: dup2");
				exit(EXIT_FAILURE);
			}
			close(out_fd);
		}

		/* Execute the command */
		printf("Creating child process for command: %s\n", node->cmd_arg[0]);
		int ret = execute_ast_command(node, shell);
		cleanup(&shell); // Cleanup after execution
		free_env_list(shell->envp);
		if (shell)
			free(shell);
		exit(ret);
	}

	return pid;
}

/* Wait for child and handle status */
static int wait_for_child(pid_t pid)
{
	int status;
	int ret = 0;

	if (waitpid(pid, &status, 0) == -1)
	{
		perror("minishell: waitpid");
		return -1;
	}

	if (WIFSIGNALED(status))
	{
		int sig = WTERMSIG(status);
		ret = 128 + sig;
		if (sig == SIGINT || sig == SIGQUIT)
		{
			print_signal_message(sig);
		}
	}
	else if (WIFEXITED(status))
	{
		ret = WEXITSTATUS(status);
	}

	return ret;
}

/* Recursive pipe execution with proper fd handling */
int execute_pipe(t_ast_node *pipe_node, t_minishell *shell)
{
	int pipe_fds[2];
	pid_t left_pid;
	int exit_status = 0;

	if (!pipe_node || pipe_node->type != NODE_PIPE)
	{
		return -1;
	}

	if (pipe(pipe_fds) == -1)
	{
		perror("minishell: pipe");
		return -1;
	}

	g_signal_status = 2; // Mark child processes running

	/* Execute left side (writes to pipe) */
	printf("Executing left side of pipe\n");
	left_pid = create_pipe_child(pipe_node->left, shell, STDIN_FILENO, pipe_fds[1]);
	if (left_pid < 0)
	{
		close(pipe_fds[0]);
		close(pipe_fds[1]);
		return -1;
	}
	printf("Left side PID: %d\n", left_pid);
	// print_ast_node(pipe_node->left); // Print left side AST node for debugging
	close(pipe_fds[1]); // Close write end in parent

	/* Execute right side (reads from pipe) */
	if (pipe_node->right->type == NODE_PIPE)
	{
		printf("Executing next pipe\n");
		// Recursive case for multiple pipes
		exit_status = execute_pipe(pipe_node->right, shell);
	}
	else
	{
		// Base case - execute final command
		printf("Executing right side of pipe\n");
		pid_t right_pid = create_pipe_child(pipe_node->right, shell, pipe_fds[0], STDOUT_FILENO);
		if (right_pid < 0)
		{
			close(pipe_fds[0]);
			return -1;
		}
		exit_status = wait_for_child(right_pid);
		printf("Right side PID: %d\n", right_pid);
		// print_ast_node(pipe_node->right); // Print right side AST node for debugging
	}
	close(pipe_fds[0]);
	printf("Pipe closed\n");
	// print_ast_node(pipe_node); // Print current AST node for debugging
	/* Wait for left process */
	int left_status = wait_for_child(left_pid);
	// cleanup(&shell); // Cleanup after left process
	(void)left_status; // We return rightmost command's status

	g_signal_status = 0;
	return exit_status;
}

/* Execute AST command with proper cleanup */
int execute_ast_command(t_ast_node *cmd_node, t_minishell *shell)
{
	if (!cmd_node)
	{
		return 1;
	}

	if (cmd_node->type == NODE_COMMAND)
	{
		t_ast_node *old_ast = shell->ast;
		shell->ast = cmd_node;
		int ret = exe_cmd(&shell);
		shell->ast = old_ast;
		return ret;
	}
	else if (cmd_node->type == NODE_PIPE)
	{
		return execute_pipe(cmd_node, shell);
	}

	return 1;
}

/* Main execution function with full cleanup */
int execute_ast(t_minishell **shell)
{
	int result = 0;

	if (!(*shell)->ast)
	{
		return 1;
	}

	/* Save original file descriptors */
	(*shell)->og_fd[0] = dup(STDIN_FILENO);
	(*shell)->og_fd[1] = dup(STDOUT_FILENO);
	if ((*shell)->og_fd[0] == -1 || (*shell)->og_fd[1] == -1)
	{
		perror("minishell: dup");
		if ((*shell)->og_fd[0] != -1)
			close((*shell)->og_fd[0]);
		if ((*shell)->og_fd[1] != -1)
			close((*shell)->og_fd[1]);
		return 1;
	}

	/* Process heredocs */
	if (process_heredocs((*shell)->ast) == -1)
	{
		(*shell)->exit_status = 130;
		close((*shell)->og_fd[0]);
		close((*shell)->og_fd[1]);
		return 1;
	}

	/* Execute the AST */
	result = execute_ast_command((*shell)->ast, *shell);
	(*shell)->exit_status = result;

	/* Restore original file descriptors */
	if (dup2((*shell)->og_fd[0], STDIN_FILENO) == -1)
	{
		perror("minishell: dup2 stdin");
	}
	if (dup2((*shell)->og_fd[1], STDOUT_FILENO) == -1)
	{
		perror("minishell: dup2 stdout");
	}
	close((*shell)->og_fd[0]);
	close((*shell)->og_fd[1]);

	/* Cleanup */
	cleanup(shell);
	return result;
}