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

void close_saved_fds(int saved_fd[2])
{
	dup2(saved_fd[1], STDOUT_FILENO);
	dup2(saved_fd[0], STDIN_FILENO);
    if (saved_fd[1] != -1)
        close(saved_fd[1]);
    if (saved_fd[0] != -1)
        close(saved_fd[0]);
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

static pid_t create_child_process(t_ast_node *node, t_minishell *shell, int in_fd, int out_fd)
{
	pid_t pid;

	pid = fork();
	if (pid < 0)
	{
		perror("fork");
		return -1;
	}
	if (pid == 0)
	{
		if (in_fd != STDIN_FILENO)
		{
			dup2(in_fd, STDIN_FILENO);
			close(in_fd);
		}
		if (out_fd != STDOUT_FILENO)
		{
			dup2(out_fd, STDOUT_FILENO);
			close(out_fd);
		}
		exit(execute_ast_command(node, shell));
	}
	return pid;
}
static int wait_for_child_processes(pid_t pid1, pid_t pid2)
{
	int status1;
	int status2;
	int ret;
	ret = 0;

	waitpid(pid1, &status1, 0);
	waitpid(pid2, &status2, 0);

	if (WIFSIGNALED(status2))
	{
		int sig = WTERMSIG(status2);
		ret = 128 + sig;
		print_signal_message(sig);
	}
	else if (WIFSIGNALED(status1))
	{
		ret = 128 + WTERMSIG(status1);
	}
	else if (WIFEXITED(status2))
	{
		ret = WEXITSTATUS(status2);
	}

	return (ret);
}

int execute_pipe(t_ast_node *pipe_node, t_minishell *shell)
{
	int pipe_fds[2];
	pid_t pid1, pid2;
	int ret;

	ret = 0;
	if (!pipe_node || pipe_node->type != NODE_PIPE)
		return (-1);
	if (pipe(pipe_fds) == -1)
	{
		perror("pipe");
		return (-1);
	}
	g_signal_status = 2; // Mark that child processes are running
	pid1 = create_child_process(pipe_node->left, shell, STDIN_FILENO, pipe_fds[1]);
	if (pid1 < 0)
    {
        close(pipe_fds[0]); // Close both ends of the pipe on error
        close(pipe_fds[1]);
        return (-1);
    }
	close(pipe_fds[1]);
	pid2 = create_child_process(pipe_node->right, shell, pipe_fds[0], STDOUT_FILENO);
	if (pid2 < 0)
    {
        close(pipe_fds[0]); // Close the read end of the pipe on error
        return (-1);
    }
	close(pipe_fds[0]);
	ret = wait_for_child_processes(pid1, pid2);
	g_signal_status = 0; // Reset signal status
	return (ret);
}

/*
 * This function temporarily overrides shell->ast with the given command node.
 * That way, your built-in functions (which expect the command in (*shell)->ast)
 * work without modification.
 */
int execute_ast_command(t_ast_node *cmd_node, t_minishell *shell)
{
	if (!cmd_node)
		return (1);

	if (cmd_node->type == NODE_COMMAND)
	{
		t_ast_node *old_ast = shell->ast;
		shell->ast = cmd_node;
		int ret = exe_cmd(&shell, 0);
		shell->ast = old_ast;
		shell->exit_status = ret; // Ensure status is set
		cleanup(&shell);
		free_env_list(shell->envp);
		if (shell)
			free(shell);
		return (ret);
	}

	if (cmd_node->type == NODE_PIPE)
	{
		int ret = execute_pipe(cmd_node, shell);
		shell->exit_status = ret; // Ensure status is set
		cleanup(&shell);
		free_env_list(shell->envp);
		if (shell)
			free(shell);
		return ret;
	}
	shell->exit_status = 1;
	cleanup(&shell);
	free_env_list(shell->envp);
	if (shell)
		free(shell);
	return (1);
}
int execute_ast(t_minishell **shell)
{
	int saved_fd[2];
	int result = 0;

	if (!(*shell)->ast)
		return 1;

	if (process_heredocs((*shell)->ast) == -1)
	{
		(*shell)->exit_status = 130;
		return 1;
	}

	saved_fd[1] = dup(STDOUT_FILENO);
	saved_fd[0] = dup(STDIN_FILENO);

	if ((*shell)->ast->type == NODE_COMMAND)
	{
		result = exe_cmd(shell, saved_fd);
	}
	else if ((*shell)->ast->type == NODE_PIPE)
	{
		result = execute_pipe((*shell)->ast, *shell);
	}
	else
	{
		(*shell)->exit_status = 1;
		result = 1;
	}

	(*shell)->exit_status = result;
	dup2(saved_fd[1], STDOUT_FILENO);
	dup2(saved_fd[0], STDIN_FILENO);
	close(saved_fd[1]);
	close(saved_fd[0]);
	printf("before cleanup\n");
	cleanup(shell);
	printf("after cleanup\n");
	return result;
}
