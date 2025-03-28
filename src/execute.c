#include "minishell.h"

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

static void free_paths(char **path_dirs, int index)
{
	while (index-- > 0)
		free(path_dirs[index]);
	free(path_dirs);
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
				return (free_paths(path_dirs, index), NULL);
			index++;
			start = path_env + 1;
		}
		path_env++;
	}
	path_dirs[index] = NULL;
	return (path_dirs);
}
////////////////////////////////

char **env_list_to_array(t_env *env)
{
	int count = 0;
	t_env *tmp = env;

	// First pass: count valid environment variables
	while (tmp)
	{
		// Only count environment variables that have both key and value
		if (tmp->key && tmp->value)
			count++;
		tmp = tmp->next;
	}

	// Allocate array with space for pointers
	char **env_array = malloc((count + 1) * sizeof(char *));
	if (!env_array)
		return NULL;

	// Reset tmp pointer
	tmp = env;
	int i = 0;

	// Second pass: create environment entries
	while (tmp)
	{
		// Skip entries with NULL key or value
		if (tmp->key && tmp->value)
		{
			// Calculate total length needed (+1 for '=' and +1 for null terminator)
			int total_len = ft_strlen(tmp->key) + ft_strlen(tmp->value) + 2;

			// Allocate memory for the full environment entry
			env_array[i] = malloc(total_len * sizeof(char));
			if (!env_array[i])
			{
				// Free previously allocated memory if allocation fails
				for (int j = 0; j < i; j++)
					free(env_array[j]);
				free(env_array);
				return NULL;
			}

			// Correctly create the environment entry string
			snprintf(env_array[i], total_len, "%s=%s", tmp->key, tmp->value);
			i++;
		}
		tmp = tmp->next;
	}

	// Null-terminate the array
	env_array[i] = NULL;

	return env_array;
}
int execute_external_command(t_ast_node *ast_cmd, t_minishell *shell)
{
	char **cmd = trim_cmd(ast_cmd->cmd_arg); //$$$ need to clean this cmd
	if (!cmd || !cmd[0])
		return (free_arg(cmd), -1);

	g_signal_status = 2; // Mark that a child process is running

	pid_t pid = fork();
	if (pid < 0)
	{
		perror("fork");
		shell->exit_status = 1;
		return (free_arg(cmd), -1);
	}

	if (pid == 0) // Child process
	{
		struct sigaction sa;
		sa.sa_handler = SIG_DFL; // Restore default signal handling
		sigemptyset(&sa.sa_mask);
		sa.sa_flags = 0;
		sigaction(SIGINT, &sa, NULL);
		sigaction(SIGQUIT, &sa, NULL);
		sigaction(SIGTSTP, &sa, NULL);

		// Check if the command is a directory
		struct stat sb;
		if (stat(cmd[0], &sb) == 0 && S_ISDIR(sb.st_mode))
		{
			fprintf(stderr, "%s: Is a directory\n", cmd[0]);
			exit(126);
		}

		/* Convert environment list to array */
		char **env_array = env_list_to_array(shell->envp);
		if (!env_array)
		{
			perror("env_list_to_array");
			free_arg(cmd);
			exit(127);
		}

		if (cmd[0][0] == '/' || cmd[0][0] == '.')
		{
			if (execve(cmd[0], cmd, env_array) == -1)
			{
				if (errno == EACCES)
				{
					fprintf(stderr, "%s: Permission denied\n", cmd[0]);
					exit(126);
				}
				else if (errno == EISDIR)
				{
					fprintf(stderr, "%s: Is a directory\n", cmd[0]);
					exit(126);
				}
				else
				{
					perror("execve");
					// ... clean up and exit ...
				}
			}
		}

		/* Otherwise, search for the command in the directories listed in PATH */
		/* Otherwise, search for the command in the directories listed in PATH */
		char *path_env = NULL;
		t_env *env_ptr = shell->envp;
		while (env_ptr)
		{
			if (ft_strcmp(env_ptr->key, "PATH") == 0)
			{
				path_env = env_ptr->value;
				break;
			}
			env_ptr = env_ptr->next;
		}

		if (!path_env)
		{
			// If PATH is unset or not found
			fprintf(stderr, "%s: No such file or directory!\n", cmd[0]);
			for (int i = 0; env_array[i]; i++)
				free(env_array[i]);
			free(env_array);
			free_arg(cmd);
			exit(127);
		}

		char **path_dirs = split_path(path_env);
		if (path_dirs)
		{
			for (int i = 0; path_dirs[i]; i++)
			{
				int len = strlen(path_dirs[i]) + strlen(cmd[0]) + 2;
				char *full_path = malloc(len);
				if (!full_path)
				{
					perror("malloc");
					free_arg(cmd);
					exit(127);
				}
				sprintf(full_path, "%s/%s", path_dirs[i], cmd[0]);
				// First check permissions to return 126 for permission issues
				if (access(full_path, X_OK) != 0)
				{
					if (errno == EACCES)
					{
						fprintf(stderr, "%s: Permission denied\n", full_path);
						free(full_path);
						for (int j = 0; path_dirs[j]; j++)
							free(path_dirs[j]);
						free(path_dirs);
						exit(126);
					}
					free(full_path);
					continue;
				}

				execve(full_path, cmd, env_array);
				free(full_path);
				break;
			}
			/* Free the split PATH array */
			for (int i = 0; path_dirs[i]; i++)
				free(path_dirs[i]);
			free(path_dirs);
		}

		/* If no valid command was found, print an error and exit */
		fprintf(stderr, "Command not found: %s\n", cmd[0]);
		for (int i = 0; env_array[i]; i++)
			free(env_array[i]);
		free(env_array);
		free_arg(cmd);
		exit(127);
	}
	else // Parent process
	{
		int status;
		waitpid(pid, &status, WUNTRACED);

		/* Check if a SIGINT was received during child execution */
		int sigint_received = (g_signal_status == 1);
		g_signal_status = 0; // Reset global state

		// Handle newline if signal was received
		if (sigint_received)
		{
			// Use stderr directly (file descriptor 2)
			if (isatty(2)) // 2 is the file descriptor for stderr
			{
				write(2, "\n", 1); // Write newline to stderr
			}
		}

		// Set exit status based on child termination
		if (WIFEXITED(status))
			shell->exit_status = WEXITSTATUS(status);
		else if (WIFSIGNALED(status))
			shell->exit_status = 128 + WTERMSIG(status);

		free_arg(cmd);
		return shell->exit_status;
	}
}

int builtin_cmd_check(t_minishell **shell)
{
	if (!shell || !(*shell) || !(*shell)->ast || !(*shell)->ast->cmd_arg)
	{
		(*shell)->exit_status = 1;
		return -1;
	}

	char *cmd = ft_strtrim((*shell)->ast->cmd_arg[0], " ");
	if (!cmd)
	{
		(*shell)->exit_status = 1;
		return -1;
	}

	int ret = -1;
	if (ft_strcmp(cmd, "echo") == 0)
	{
		ret = exe_echo(shell);
	}
	else if (strcmp(cmd, "env") == 0)
	{
		ret = exe_env(shell);
	}
	else if (strcmp(cmd, "unset") == 0)
	{
		ret = exe_unset(shell);
	}
	else if (strcmp(cmd, "exit") == 0)
	{
		free(cmd);
		ret = exe_exit(shell);
	}
	else if (strcmp(cmd, "pwd") == 0)
	{
		ret = exe_pwd(shell);
	}
	else if (strcmp(cmd, "cd") == 0)
	{
		ret = exe_cd(shell);
	}
	else if (strcmp(cmd, "export") == 0)
	{
		ret = exe_export(shell);
	}
	if (cmd)
		free(cmd);
	if (ret != -1)
		(*shell)->exit_status = ret;
	return ret;
}

int exe_cmd(t_minishell **shell)
{
	if (!(*shell)->ast)
	{
		(*shell)->exit_status = 1;
		fprintf(stderr, "Error: No command provided\n");
		return 1;
	}

	if ((*shell)->ast->redir)
	{
		if (handle_redirections((*shell)->ast->redir) == -1)
		{
			(*shell)->exit_status = 1;
			return 1;
		}
	}

	int ret = builtin_cmd_check(shell);
	if (ret != -1)
	{
		(*shell)->exit_status = ret;
		return ret;
	}

	ret = execute_external_command((*shell)->ast, *shell);
	(*shell)->exit_status = ret;
	return ret;
}
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

/*
 * This function temporarily overrides shell->ast with the given command node.
 * That way, your built-in functions (which expect the command in (*shell)->ast)
 * work without modification.
 */
int execute_ast_command(t_ast_node *cmd_node, t_minishell *shell)
{
	if (!cmd_node)
		return 1;

	if (cmd_node->type == NODE_COMMAND)
	{
		t_ast_node *old_ast = shell->ast;
		shell->ast = cmd_node;
		int ret = exe_cmd(&shell);
		shell->ast = old_ast;
		shell->exit_status = ret; // Ensure status is set
		return ret;
	}

	if (cmd_node->type == NODE_PIPE)
	{
		int ret = execute_pipe(cmd_node, shell);
		shell->exit_status = ret; // Ensure status is set
		return ret;
	}

	shell->exit_status = 1;
	return 1;
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
		result = exe_cmd(shell);
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

	// Only update exit status if not interrupted
	if (g_signal_status != 130)
	{
		(*shell)->exit_status = result;
	}

	dup2(saved_fd[1], STDOUT_FILENO);
	dup2(saved_fd[0], STDIN_FILENO);
	close(saved_fd[1]);
	close(saved_fd[0]);

	return result;
}
