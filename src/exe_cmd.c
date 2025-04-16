#include "minishell.h"

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

int execute_builtin(t_ast_node *ast, t_minishell *shell, char *cmd)
{
	if (ft_strcmp(cmd, "echo") == 0)
		return (exe_echo(ast));
	if (ft_strcmp(cmd, "cd") == 0)
		return (exe_cd(ast, shell));
	if (ft_strcmp(cmd, "pwd") == 0)
		return (exe_pwd(&shell));
	if (ft_strcmp(cmd, "export") == 0)
		return (exe_export(&shell));
	if (ft_strcmp(cmd, "unset") == 0)
		return (exe_unset(&shell));
	if (ft_strcmp(cmd, "env") == 0)
		return (exe_env(&shell));
	return (-1);
}

static void execute_and_check_command(char *cmd, char **args, char **env_array, t_minishell *shell)
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

static int check_and_execute(char *full_path, char **cmd, char **env_array, t_minishell *shell)
{
	if (access(full_path, X_OK) == 0)
	{
		execute_and_check_command(full_path, cmd, env_array, shell);
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
		return (return_with_status(shell, 127));
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
	close_og_fd(shell);
	print_error_message(cmd[0], "Command not found? HoHoHOOOOOOO");
	return (return_with_status(shell, 127));
}

static void execute_external_child_process(char **cmd, t_minishell *shell)
{
	char **env_array;

	handle_child_signals();
	if (!cmd || !cmd[0] || cmd[0][0] == '\0')
	{
		print_error_message("Command", "'' not found");
		free_arg(cmd); // Free cmd
		cleanup(&shell);
		free_env_list(shell->envp);
		if (shell)
			free(shell);
		exit(127);
	}
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
		printf("before in child process, cmd[0]: %s\n", cmd[0]);
		execute_and_check_command(cmd[0], cmd, env_array, shell);
		printf("afer in child process, cmd[0]: %s\n", cmd[0]);
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
	printf("external command parent before cleanup\n");
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

int execute_external_command(t_ast_node *ast_cmd, int *og_fd, t_minishell *shell)
{
	char **cmd;
	pid_t pid;
	int status;

	cmd = trim_cmd(ast_cmd->cmd_arg);
	g_signal_status = 2;
	pid = fork();
	if (pid < 0)
	{
		perror("fork failed");
		set_exit_status(shell, 1);
		free_arg(cmd);
		return (-1);
	}
	if (pid == 0)
	{
		(close(og_fd[0]), close(og_fd[1]));
		// printf("execute_external_command in child process, cmd[0]: %s\n", cmd[0]);
		// print_ast_node(ast_cmd); // Print AST node for debugging
		if (ast_cmd->cmd_arg)
			execute_external_child_process(cmd, shell);

		// cleanup(&shell);
		// free_env_list(shell->envp);
		// if (shell)
		// 	free(shell);
		// exit(127);
	}
	waitpid(pid, &status, 0);
	free_arg(cmd);
	return (status);
}

int builtin_cmd_check(char *cmd, t_ast_node *ast, t_minishell *shell)
{
	int ret;

	if (!ast || !ast->cmd_arg)
		return (return_with_status(shell, 1));
	cmd = ft_strtrim(ast->cmd_arg[0], " ");
	if (!cmd)
		return (return_with_status(shell, 1));
	ret = execute_builtin(ast, shell, cmd);
	if (ret == -1 && ft_strcmp(cmd, "exit") == 0)
	{
		free(cmd);
		return (exe_exit(&shell));
	}
	free(cmd);
	if (ret != -1)
		set_exit_status(shell, ret);
	return (ret);
}

int exe_cmd(t_ast_node *node, int *og_fd, t_minishell *shell)
{
	int ret;
	char **cmd;

	// printf("Node command: %s\n", node->cmd_arg[0]);
	cmd = node->cmd_arg;
	if (!cmd || !*cmd)
	{
		ft_putstr_fd("Error: No command provided\n", STDERR_FILENO);
		reset_close_fd(og_fd,1,1);
		return (return_with_status(shell, 1));
	}
	if (node->redir)
	{
		if (handle_redirections(node->redir) == -1)
			return (return_with_status(shell, 1));
	}
	ret = builtin_cmd_check(*cmd, node, shell);
	if (ret != -1)
		return (return_with_status(shell, ret));
	ret = execute_external_command(node, og_fd, shell);

	return (return_with_status(shell, ret));
}
