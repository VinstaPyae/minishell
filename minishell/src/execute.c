#include "minishell.h"

char **split_path(char *path_env)
{
	int count = 1; // at least one directory in PATH
	for (int i = 0; path_env[i]; i++)
	{
		if (path_env[i] == ':')
		{
			count++;
		}
	}

	char **path_dirs = malloc((count + 1) * sizeof(char *));
	if (!path_dirs)
		return NULL;

	int index = 0;
	char *start = path_env;
	for (int i = 0; path_env[i]; i++)
	{
		if (path_env[i] == ':' || path_env[i + 1] == '\0')
		{
			int length = &path_env[i] - start + (path_env[i + 1] == '\0' ? 1 : 0);
			path_dirs[index] = malloc(length + 1);
			if (!path_dirs[index])
			{
				// Free already allocated memory
				for (int j = 0; j < index; j++)
				{
					free(path_dirs[j]);
				}
				free(path_dirs);
				return NULL;
			}
			strncpy(path_dirs[index], start, length);
			path_dirs[index][length] = '\0';
			index++;
			start = &path_env[i + 1];
		}
	}

	path_dirs[index] = NULL;
	return path_dirs;
}

char *find_executable(char *cmd)
{
	char *path_env = getenv("PATH");
	if (!path_env)
		return NULL;

	char *paths = ft_strdup(path_env);
	if (!paths)
		return NULL;

	char *saveptr;
	char *dir = strtok_r(paths, ":", &saveptr);
	char full_path[PATH_MAX];
	struct stat sb;

	while (dir)
	{
		snprintf(full_path, sizeof(full_path), "%s/%s", dir, cmd);
		if (stat(full_path, &sb) == 0 && (sb.st_mode & S_IXUSR))
		{
			free(paths);
			return ft_strdup(full_path);
		}
		dir = strtok_r(NULL, ":", &saveptr);
	}
	free(paths);
	return NULL;
}

char **env_list_to_array(t_env *env)
{
	int count = 0;
	t_env *tmp = env;
	while (tmp)
	{
		// Only count environment variables that have both key and value
		if (tmp->key && tmp->value)
			count++;
		tmp = tmp->next;
	}

	char **env_array = malloc((count + 1) * sizeof(char *));
	if (!env_array)
		return NULL;

	tmp = env;
	int i = 0;
	while (tmp)
	{
		// Skip entries with NULL values
		if (tmp->key && tmp->value)
		{
			char *env_entry = malloc(ft_strlen(tmp->key) + ft_strlen(tmp->value) + 2);
			if (!env_entry)
			{
				// Free previously allocated memory
				for (int j = 0; j < i; j++)
					free(env_array[j]);
				free(env_array);
				return NULL;
			}
			sprintf(env_entry, "%s=%s", tmp->key, tmp->value);
			env_array[i++] = env_entry;
		}
		tmp = tmp->next;
	}
	env_array[i] = NULL; // Null-terminate the array

	return env_array;
}
int execute_external_command(t_ast_node *ast_cmd, t_minishell *shell)
{
	char **cmd = trim_cmd(ast_cmd->cmd_arg);
	if (!cmd || !cmd[0])
		return -1;

	g_signal_status = 2; // Mark that a child process is running

	pid_t pid = fork();
	if (pid < 0)
	{
		perror("fork");
		return -1;
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

		/* Convert environment list to array */
		char **env_array = env_list_to_array(shell->envp);
		if (!env_array)
		{
			perror("env_list_to_array");
			exit(127);
		}

		/* If the command is an absolute or relative path, use it directly */
		if (cmd[0][0] == '/' || cmd[0][0] == '.')
		{
			execve(cmd[0], cmd, env_array);
			perror("execve");
			for (int i = 0; env_array[i]; i++)
				free(env_array[i]);
			free(env_array);
			exit(127);
		}

		/* Otherwise, search for the command in the directories listed in PATH */
		char *path_env = getenv("PATH");
		if (!path_env)
		{
			// If PATH is unset, use the default PATH
			path_env = "/bin:/usr/bin:/usr/local/bin";
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
					exit(127);
				}
				sprintf(full_path, "%s/%s", path_dirs[i], cmd[0]);
				if (access(full_path, X_OK) == 0)
				{
					execve(full_path, cmd, env_array);
					/* If execve returns, an error occurred. */
					perror("execve");
					free(full_path);
					break;
				}
				free(full_path);
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
		exit(127);
	}
	else // Parent process
	{
		int status;
		waitpid(pid, &status, WUNTRACED);

		/* Check if a SIGINT was received during child execution */
		int sigint_received = (g_signal_status == 1);
		g_signal_status = 0; // Reset global state

		if (sigint_received)
			write(STDOUT_FILENO, "\n", 1);

		if (WIFSIGNALED(status))
			shell->exit_status = 128 + WTERMSIG(status);
		else
			shell->exit_status = WEXITSTATUS(status);

		return shell->exit_status;
	}
}

int builtin_cmd_check(t_minishell **shell)
{
	if (!shell || !(*shell) || !(*shell)->ast || !(*shell)->ast->cmd_arg)
		return -1;

	char *cmd = ft_strtrim((*shell)->ast->cmd_arg[0], " ");

	// printf("cmd: %s\n", cmd);
	if (ft_strcmp(cmd, "echo") == 0)
		return exe_echo(shell);
	else if (strcmp(cmd, "env") == 0)
		return exe_env(shell);
	else if (strcmp(cmd, "unset") == 0)
		return exe_unset(shell);
	else if (strcmp(cmd, "exit") == 0)
		return exe_exit(shell);
	else if (strcmp(cmd, "pwd") == 0)
		return exe_pwd(shell);
	else if (strcmp(cmd, "cd") == 0)
		return exe_cd(shell);
	else if (strcmp(cmd, "export") == 0)
		return exe_export(shell);

	return -1;
}

int exe_cmd(t_minishell **shell)
{
	if (!(*shell)->ast)
	{
		fprintf(stderr, "Error: No command provided\n");
		return 1;
	}

	if ((*shell)->ast->redir)
	{
		if (handle_redirections((*shell)->ast->redir) == -1)
			return 1;
	}

	int ret = builtin_cmd_check(shell);
	if (ret != -1)
		return ret;

	return execute_external_command((*shell)->ast, *shell);
}

int execute_pipe(t_ast_node *pipe_node, t_minishell *shell)
{
	int pipe_fds[2];
	pid_t pid1, pid2;
	int status;

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
	if (pid1 == 0) // First child executes left command
	{
		close(pipe_fds[0]);				  // Close unused read end
		dup2(pipe_fds[1], STDOUT_FILENO); // Redirect stdout to pipe
		close(pipe_fds[1]);				  // Close original write end

		exit(execute_ast_command(pipe_node->left, shell));
	}

	pid2 = fork();
	if (pid2 < 0)
	{
		perror("fork");
		return -1;
	}
	if (pid2 == 0) // Second child executes right command
	{
		close(pipe_fds[1]);				 // Close unused write end
		dup2(pipe_fds[0], STDIN_FILENO); // Redirect stdin from pipe
		close(pipe_fds[0]);				 // Close original read end

		exit(execute_ast_command(pipe_node->right, shell));
	}

	// Parent process: close pipe ends and wait for children
	close(pipe_fds[0]);
	close(pipe_fds[1]);

	waitpid(pid1, &status, 0);
	shell->exit_status = WEXITSTATUS(status);
	waitpid(pid2, &status, 0);
	shell->exit_status = WEXITSTATUS(status);

	return shell->exit_status;
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
		return ret;
	}

	if (cmd_node->type == NODE_PIPE)
		return execute_pipe(cmd_node, shell);

	return 1;
}

int execute_ast(t_minishell **shell)
{
	int saved_stdout, saved_stdin;
	int result = 0;

	if (!(*shell)->ast)
		return 1;

	saved_stdout = dup(STDOUT_FILENO);
	saved_stdin = dup(STDIN_FILENO);

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
		fprintf(stderr, "Error: Unsupported AST node type\n");
		result = 1;
	}

	dup2(saved_stdout, STDOUT_FILENO);
	dup2(saved_stdin, STDIN_FILENO);
	close(saved_stdout);
	close(saved_stdin);

	return result;
}

