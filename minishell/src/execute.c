#include "minishell.h"

int execute_external_command(t_ast_node *ast_cmd, t_minishell *shell)
{
	char **cmd;
	pid_t pid;
	int status;

	cmd = trim_cmd(ast_cmd->cmd_arg);
	if (!cmd)
		return (-1);
	pid = fork();
	if (pid < 0)
	{
		perror("fork");
		return -1;
	}
	if (pid == 0)
	{
		// printf("Executing external command: %s\n", cmd[0]);
		execvp(cmd[0], cmd);
		perror("execvp");
		exit(127);
	}
	else
	{
		waitpid(pid, &status, 0);
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
