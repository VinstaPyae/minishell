#include "minishell.h"

int execute_external_command(t_ast_node *ast_cmd, t_minishell *shell)
{
	char    **cmd;
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
	if (pid == 0)  // Child process
	{
		/* 
		* Setup any redirections for this command.
		* If redirection fails, exit immediately.
		*/
		// if (ast_cmd->redir && handle_redirections(ast_cmd->redir) == -1)
		//     exit(1);

		/* Debug print (optional) */
		printf("Executing external command: %s\n", cmd[0]);
		/* Execute the command. execvp() will use PATH to locate the command */
		execvp(cmd[0] , cmd);
		perror("execvp");
		exit(127);
	}
	else  // Parent process
	{
		waitpid(pid, &status, 0);
		shell->exit_status = WEXITSTATUS(status);
		return shell->exit_status;
	}
}

//builtin check and execute
int builtin_cmd_check(t_minishell **shell)
{
    if (!shell || !(*shell) || !(*shell)->ast || !(*shell)->ast->cmd_arg)
        return -1;

    char *cmd = ft_strtrim((*shell)->ast->cmd_arg[0], " ");

    printf("cmd: %s\n", cmd);
    if (ft_strcmp(cmd, "echo") == 0)
    {
        return exe_echo(shell);
    }
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

    return -1;  // Not a built-in command.
}


//all command check and execute
int exe_cmd(t_minishell **shell)
{
    if (!(*shell)->ast || !(*shell)->ast->cmd_arg)
    {
        fprintf(stderr, "Error: No command provided\n");
        return 1;
    }

    /* Check for built-in commands first */
    int ret = builtin_cmd_check(shell);
    if (ret != -1)
    {
        /* Built-in command executed successfully */
        return ret;
    }

    /* Otherwise, execute as an external command */
    return execute_external_command((*shell)->ast, *shell);
}


//execute pipe
int execute_pipeline(t_ast_node *ast, t_minishell *shell)
{
    int pipefd[2];
    pid_t pid1, pid2;
    int status;

    if (pipe(pipefd) < 0)
    {
        perror("pipe");
        return -1;
    }

    /* Fork the first child for the left command */
    pid1 = fork();
    if (pid1 < 0)
    {
        perror("fork");
        return -1;
    }
    if (pid1 == 0)
    {
        /* Redirect STDOUT to the pipe's write end */
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[0]);
        close(pipefd[1]);

        if (ast->left->redir && handle_redirections(ast->left->redir) == -1)
            exit(1);

        printf("Executing piped command (left): %s\n", ast->left->cmd_arg[0]);
        execvp(ast->left->cmd_arg[0], ast->left->cmd_arg);
        perror("execvp (left)");
        exit(127);
    }

    /* Fork the second child for the right command */
    pid2 = fork();
    if (pid2 < 0)
    {
        perror("fork");
        return -1;
    }
    if (pid2 == 0)
    {
        /* Redirect STDIN to the pipe's read end */
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]);
        close(pipefd[1]);

        if (ast->right->redir && handle_redirections(ast->right->redir) == -1)
            exit(1);

        printf("Executing piped command (right): %s\n", ast->right->cmd_arg[0]);
        execvp(ast->right->cmd_arg[0], ast->right->cmd_arg);
        perror("execvp (right)");
        exit(127);
    }

    /* Parent: close pipe file descriptors and wait for children */
    close(pipefd[0]);
    close(pipefd[1]);

    waitpid(pid1, &status, 0);
    shell->exit_status = WEXITSTATUS(status);
    waitpid(pid2, &status, 0);
    shell->exit_status = WEXITSTATUS(status);

    return shell->exit_status;
}


////all execution 
int execute_ast(t_minishell **shell)
{
    int saved_stdout, saved_stdin;
    int result = 0;

    if (!(*shell)->ast)
        return 1;

    /* Save the original STDOUT and STDIN */
    saved_stdout = dup(STDOUT_FILENO);
    saved_stdin  = dup(STDIN_FILENO);

    if ((*shell)->ast->type == NODE_COMMAND)
    {
	if ((*shell)->ast->redir)
        {
            if (handle_redirections((*shell)->ast->redir) == -1)
                return (1);
        }
        result = exe_cmd(shell);
    }
//     else if ((*shell)->ast->type == NODE_PIPE)
//     {
//         if (!(*shell)->ast->left || !(*shell)->ast->right)
//         {
//             fprintf(stderr, "Error: Invalid pipeline syntax\n");
//             result = 1;
//         }
//         else
//         {
//             result = execute_pipeline((*shell)->ast, *shell);
//         }
//     }
    else
    {
        fprintf(stderr, "Error: Unsupported AST node type\n");
        result = 1;
    }

    /* Restore original STDOUT and STDIN */
    dup2(saved_stdout, STDOUT_FILENO);
    dup2(saved_stdin, STDIN_FILENO);
    close(saved_stdout);
    close(saved_stdin);

    return result;
}
