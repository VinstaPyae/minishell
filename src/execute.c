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
static char *create_env_entry(char *key, char *value)
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

static void handle_child_signals(void)
{
	struct sigaction sa;

	sa.sa_handler = SIG_DFL;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGQUIT, &sa, NULL);
	sigaction(SIGTSTP, &sa, NULL);
}

static void print_error_message(char *cmd, char *message)
{
	write(STDERR_FILENO, cmd, ft_strlen(cmd));
	write(STDERR_FILENO, ": ", 2);
	write(STDERR_FILENO, message, ft_strlen(message));
	write(STDERR_FILENO, "\n", 1);
}

static int is_directory(char *cmd, t_minishell *shell)
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

static char **get_env_array(t_minishell *shell)
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

static void execute_command(char *cmd, char **args, char **env_array, t_minishell *shell)
{
	if (execve(cmd, args, env_array) == -1)
	{
		if (errno == EACCES)
		{
			print_error_message(cmd, "Permission denied");
			
			free_array_list(env_array, -1); // Free env_array
			free_arg(args);					// Free args
			cleanup(&shell);
			free_env_list(shell->envp);
			if (shell)
				free(shell);
			exit(126);
		}
		else if (errno == ENOENT)
		{
			print_error_message(cmd, "No such file or directory");
			free_array_list(env_array, -1); // Free env_array
			free_arg(args);					// Free args
			cleanup(&shell);
			free_env_list(shell->envp);
			if (shell)
				free(shell);
			exit(127);
		}
		else if (errno == EISDIR)
		{
			print_error_message(cmd, "Is a directory");
			free_array_list(env_array, -1); // Free env_array
			free_arg(args);					// Free args
			cleanup(&shell);
			free_env_list(shell->envp);
			if (shell)
				free(shell);
			exit(126);
		}
		else
		{
			perror(cmd);
			free_array_list(env_array, -1); // Free env_array
			free_arg(args);					// Free args
			cleanup(&shell);
			free_env_list(shell->envp);
			if (shell)
				free(shell);
			exit(126);
		}
	}
}
static int handle_no_path(char *cmd, t_minishell *shell)
{
	print_error_message(cmd, "No such file or directory");
	return (return_with_status(&shell, 127));
}

static int check_and_execute(char *full_path, char **cmd, char **env_array, t_minishell *shell)
{
	if (access(full_path, X_OK) == 0)
	{
		execute_command(full_path, cmd, env_array, shell);
		free(full_path);
		return (1);
	}
	free(full_path);
	return (0);
}

static int search_and_execute(char **cmd, char **env_array, t_minishell *shell)
{
	char **path_dirs;
	char *full_path;
	char *path_value;
    int i;

    path_value = ft_getenv(shell->envp, "PATH");
    if (!path_value)
        return (handle_no_path(cmd[0], shell));
    path_dirs = split_path(path_value);
	if (!path_dirs)
		return (return_with_status(&shell, 127));
	i = 0;
	while (path_dirs[i])
	{
		char *temp_path = ft_strjoin(path_dirs[i], "/");
		if (!temp_path)
			continue;

		full_path = ft_strjoin(temp_path, cmd[0]);
		free(temp_path); // Free the intermediate string

		if (!full_path)
			continue;

		if (check_and_execute(full_path, cmd, env_array, shell))
		{
			free_array_list(path_dirs, -1); // Free path_dirs before returning
			return (0);
		}
		i++;
	}
	free_array_list(path_dirs, -1);
	print_error_message(cmd[0], "Command not found");
	return (return_with_status(&shell, 127));
}

static void execute_child_process(char **cmd, t_minishell *shell)
{
	char **env_array;

	handle_child_signals();
	if ((cmd[0][0] == '/' || cmd[0][0] == '.') && is_directory(cmd[0], shell))
	{
		free_arg(cmd); // Free cmd
		cleanup(&shell);
		free_env_list(shell->envp);
		if (shell)
			free(shell);
		exit(126);
	}
	env_array = get_env_array(shell);
	if (!env_array)
	{
		free_arg(cmd); // Free cmd
		cleanup(&shell);
		free_env_list(shell->envp);
		if (shell)
			free(shell);
		exit(127);
	}
	if (cmd[0][0] == '/' || cmd[0][0] == '.')
	{
		execute_command(cmd[0], cmd, env_array, shell);
	}
	// Before searching paths
	int result = search_and_execute(cmd, env_array, shell);
	if (result == 127)
	{
		// cleanup(&shell);
		// free_env_list(shell->envp);
		// if (shell)
		// 	free(shell);
		free_arg(cmd); // Free cmd
	}
	free_array_list(env_array, -1); // Free env_array
	cleanup(&shell);
	free_env_list(shell->envp);
	if (shell)
		free(shell);
	exit(result);
}

static int handle_parent_process(pid_t pid, t_minishell *shell)
{
	int status;

	waitpid(pid, &status, WUNTRACED);
	g_signal_status = 0;
	if (WIFEXITED(status))
	{
		set_exit_status(shell, WEXITSTATUS(status));
	}
	else if (WIFSIGNALED(status))
	{
		set_exit_status(shell, 128 + WTERMSIG(status));
		print_signal_message(WTERMSIG(status));
	}
	return (shell->exit_status);
}

int execute_external_command(t_ast_node *ast_cmd, t_minishell *shell)
{
	char **cmd;
	pid_t pid;

	cmd = trim_cmd(ast_cmd->cmd_arg);
	if (!cmd || !cmd[0] || cmd[0][0] == '\0')
	{
		if (cmd[0][0] == '\0')
			print_error_message("Command", "'' not found");
		else if (cmd && cmd[0])
			print_error_message(cmd[0], "Command not found");
		free_arg(cmd);
		set_exit_status(shell, 127);
		return (127);
	}
	g_signal_status = 2;
	pid = fork();
	if (pid < 0)
	{
		perror("fork");
		set_exit_status(shell, 1);
		free_arg(cmd);
		return (-1);
	}
	if (pid == 0)
	{
		execute_child_process(cmd, shell);
		// Child never reaches here, but add exit just in case
		cleanup(&shell);
		free_env_list(shell->envp);
		if (shell)
			free(shell);
		exit(127);
	}
	free_arg(cmd);
	return (handle_parent_process(pid, shell));
}

////////
int execute_builtin(t_minishell **shell, char *cmd)
{
	if (ft_strcmp(cmd, "echo") == 0)
		return (exe_echo(shell));
	if (ft_strcmp(cmd, "cd") == 0)
		return (exe_cd(shell));
	if (ft_strcmp(cmd, "pwd") == 0)
		return (exe_pwd(shell));
	if (ft_strcmp(cmd, "export") == 0)
		return (exe_export(shell));
	if (ft_strcmp(cmd, "unset") == 0)
		return (exe_unset(shell));
	if (ft_strcmp(cmd, "env") == 0)
		return (exe_env(shell));
	return (-1);
}
int builtin_cmd_check(t_minishell **shell, int fd[2])
{
	char *cmd;
	int ret;

	if (!shell || !(*shell) || !(*shell)->ast || !(*shell)->ast->cmd_arg)
		return (return_with_status(shell, 1));
	cmd = ft_strtrim((*shell)->ast->cmd_arg[0], " ");
	if (!cmd)
		return (return_with_status(shell, 1));
	ret = execute_builtin(shell, cmd);
	if (ret == -1 && ft_strcmp(cmd, "exit") == 0)
	{
		free(cmd);
		return (exe_exit(shell, fd));
	}
	free(cmd);
	if (ret != -1)
		set_exit_status(*shell, ret);
	return (ret);
}
//////
int exe_cmd(t_minishell **shell, int fd[2])
{
	int ret;

	if (!(*shell)->ast)
	{
		ft_putstr_fd("Error: No command provided\n", STDERR_FILENO);
		return (return_with_status(shell, 1));
	}
	if ((*shell)->ast->redir)
	{
		if (handle_redirections((*shell)->ast->redir) == -1)
			return (return_with_status(shell, 1));
	}
	ret = builtin_cmd_check(shell,fd);
	if (ret != -1)
		return (return_with_status(shell, ret));
	ret = execute_external_command((*shell)->ast, *shell);
	return (return_with_status(shell, ret));
}
/*
int execute_pipe(t_ast_node *pipe_node, t_minishell *shell)
{
	int pipe_fds[2];
	pid_t pid1, pid2;
	int status;
	int ret = 0;

	if (!pipe_node || pipe_node->type != NODE_PIPE)
		return -1;

	if (pipe(pipe_fds) == -1)
	{
		perror("pipe");
		return -1;
	}

	pid1 = fork();
	if (pid1 < 0)
	{
		perror("fork");
		return -1;
	}
	if (pid1 == 0)
	{
		close(pipe_fds[0]);
		dup2(pipe_fds[1], STDOUT_FILENO);
		close(pipe_fds[1]);
		exit(execute_ast_command(pipe_node->left, shell));
	}

	pid2 = fork();
	if (pid2 < 0)
	{
		perror("fork");
		return -1;
	}
	if (pid2 == 0)
	{
		close(pipe_fds[1]);
		dup2(pipe_fds[0], STDIN_FILENO);
		close(pipe_fds[0]);
		exit(execute_ast_command(pipe_node->right, shell));
	}

	close(pipe_fds[0]);
	close(pipe_fds[1]);

	waitpid(pid1, &status, 0);
	waitpid(pid2, &status, 0);

	if (WIFEXITED(status))
		ret = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
		ret = 128 + WTERMSIG(status);

	return ret;
}
*/
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

	return result;
}
