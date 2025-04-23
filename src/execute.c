#include "minishell.h"

/* Wait for child and handle status */
int wait_for_child(pid_t pid)
{
	int	status;
	int	ret;
	int	sig;

	ret = 0;
	if (waitpid(pid, &status, 0) == -1)
		return (perror("minishell: waitpid"), -1);
	if (WIFSIGNALED(status))
	{
		sig = WTERMSIG(status);
		ret = 128 + sig;
		//printf("g_signal_status: %d\n", g_signal_status);
		if ((sig == SIGINT || sig == SIGQUIT) && g_signal_status)
		{
			print_signal_message(sig);
			g_signal_status = 0; // Prevent duplicates
		}
		//printf("g_signal_status: %d\n", g_signal_status);
	}
	else if (WIFEXITED(status))
		ret = WEXITSTATUS(status);
	return (ret);
}


/* Helper function to fork and handle child processes */
static pid_t fork_and_handle(t_ast_node *pipe_node, t_minishell *shell, 
                            int pipe_fds[2], int is_left_child)
{
    pid_t pid;

    pid = fork();
    if (pid < 0)
    {
        perror("minishell: fork");
        close(pipe_fds[0]);
        close(pipe_fds[1]);
        return (-1);
    }
    if (pid == 0)
    {
        // handle_child_signals();
        if (is_left_child)
            handle_left_child(pipe_node, shell, pipe_fds);
        else
            handle_right_child(pipe_node, shell, pipe_fds);
    }
    return (pid);
}

/* Execute a pipe command */
int execute_pipe(t_ast_node *pipe_node, t_minishell *shell)
{
    int pipe_fds[2];
    pid_t left_pid;
    pid_t right_pid;
    int left_status;
    int right_status;

    if (!pipe_node)
        return (-1);
    if (create_pipe(pipe_fds, pipe_node) == -1)
        return (-1);
    g_signal_status = 2;
    left_pid = fork_and_handle(pipe_node, shell, pipe_fds, 1);
    if (left_pid < 0)
        return (-1);
    right_pid = fork_and_handle(pipe_node, shell, pipe_fds, 0);
    if (right_pid < 0)
    {
        waitpid(left_pid, NULL, 0);
        return (-1);
    }
    (close(pipe_fds[0]), close(pipe_fds[1]));
    left_status = wait_for_child(left_pid);
    right_status = wait_for_child(right_pid);
    g_signal_status = 0;
    return (return_with_status(shell, right_status));
}
static int handle_ast_execution(t_ast_node *ast_root, t_minishell *shell)
{
    int result;

    result = 0;
    if (ast_root->type == NODE_PIPE)
    {
        result = execute_pipe(ast_root, shell);
        if (result == -1)
        {
            close_heredoc_fds(ast_root);
            perror("minishell: execute_pipe");
            return (return_with_status(shell, 1));
        }
    }
    else if (ast_root->type == NODE_COMMAND)
    {
        result = exe_cmd(ast_root, shell);
        if (result == -1)
        {
            close_heredoc_fds(ast_root);
            perror("minishell: execute_ast_command");
            return (return_with_status(shell, 1));
        }
    }
    return (result);
}

int execute_ast(t_ast_node *ast_root, t_minishell *shell)
{
    int result;

    result = 0;
    if (!ast_root)
    {
        close_heredoc_fds(ast_root);
        perror("minishell: No AST to execute");
        return (return_with_status(shell, 1));
    }
    result = handle_ast_execution(ast_root, shell);
    setup_signal_handlers();
    close_heredoc_fds(ast_root);
    cleanup(&shell);
    return (result);
}


/* Execute a pipe command */
// int execute_pipe(t_ast_node *pipe_node, t_minishell *shell)
// {
//     int pipe_fds[2];
//     pid_t left_pid, right_pid;
//     int status;
    
//     if (!pipe_node)
//         return -1;
//     if (pipe(pipe_fds) == -1)
//     {
//         close_heredoc_fds(pipe_node); // Add this
//         return (perror("minishell: pipe Failed"), -1);
//     }
//     g_signal_status = 2; /* Mark child processes running */
//     left_pid = fork();
//     if (left_pid < 0)
//     {
//         (close(pipe_fds[0]), close(pipe_fds[1]));
//         return (perror("minishell: fork"), -1);
//     }
//     if (left_pid == 0) /* Left child process */
//     {
//         handle_child_signals();
//         close_heredoc_fds(pipe_node->right);
//         close(pipe_fds[0]);
//         if (dup2(pipe_fds[1], STDOUT_FILENO) == -1)
//         {
//             perror("minishell: dup2");
//             exit(1);
//         }
//         close(pipe_fds[1]);
//         int ret = execute_ast(pipe_node->left, shell);
//         cleanup(&shell);
//         free_env_list(shell->envp);
//         if (shell)
//             free(shell);
//         exit(ret);
//     }
//     /* Create right child (reads from pipe) */
//     right_pid = fork();
//     if (right_pid < 0)
//     {
//         perror("minishell: fork");
//         close(pipe_fds[0]);
//         close(pipe_fds[1]);
//         waitpid(left_pid, NULL, 0); /* Clean up the left child */
//         return -1;
//     }
    
//     if (right_pid == 0) /* Right child process */
//     {
//         handle_child_signals();
//         close_heredoc_fds(pipe_node->left);
//         /* Close write end in right child */
//         close(pipe_fds[1]);
//         /* Redirect stdin from pipe read end */
//         if (dup2(pipe_fds[0], STDIN_FILENO) == -1)
//         {
//             perror("minishell: dup2");
//             exit(1);
//         }
//         close(pipe_fds[0]);
        
//         int ret;
//         ret = execute_ast(pipe_node->right, shell);
//         cleanup(&shell);
//         free_env_list(shell->envp);
//         if (shell)
//             free(shell);
//         exit(ret);
//     }
//     close(pipe_fds[0]);
//     close(pipe_fds[1]);
//     int left_status = wait_for_child(left_pid, shell);
//     int right_status = wait_for_child(right_pid, shell);
//     g_signal_status = 0;    
//     return (return_with_status(shell, right_status));
// }
///////////////////////////****************** //////////////////



// int execute_ast(t_ast_node *ast_root, t_minishell *shell)
// {
//     int	result;

//     result = 0;//(signal(SIGINT, SIG_IGN), signal(SIGQUIT, SIG_IGN));
//     if (!ast_root)
//     {
//         close_heredoc_fds(ast_root);
//         perror("minishell: No AST to execute");
//         return (return_with_status(shell, 1));
//     }
//     if (ast_root && ast_root->type == NODE_PIPE)
//     {
//         result = execute_pipe(ast_root, shell);
//         if (result == -1)
//         {
//             close_heredoc_fds(ast_root);
//             perror("minishell: execute_pipe");
//             return (return_with_status(shell, 1));
//         }
//     }
//     else if (ast_root && ast_root->type == NODE_COMMAND)
//     {
//         result = exe_cmd(ast_root, shell);
//         if (result == -1)
//         {
//             close_heredoc_fds(ast_root);
//             perror("minishell: execute_ast_command");
//             return (return_with_status(shell, 1));
//         }
//     }
//     setup_signal_handlers(); // Restore signal handlers for parent process
//     close_heredoc_fds(ast_root); // Close heredoc file descriptors
//     cleanup(&shell);
//     return result;
// }



// void close_og_fd(t_minishell *shell)
// {
//     close(shell->og_fd[1]);
//     close(shell->og_fd[0]);
// }
// char **split_path(char *path_env)
// {
//     char **path_dirs;
//     char *start;
//     int index;
//     int count;

//     path_dirs = allocate_paths(path_env);
//     if (!path_dirs)
//         return (NULL);
//     start = path_env;
//     index = 0;
//     while (*path_env)
//     {
//         if (*path_env == ':' || !*(path_env + 1))
//         {
//             count = path_env - start + (!*(path_env + 1));
//             path_dirs[index] = ft_strndup(start, count);
//             if (!path_dirs[index])
//                 return (free_array_list(path_dirs, index), NULL);
//             index++;
//             start = path_env + 1;
//         }
//         path_env++;
//     }
//     path_dirs[index] = NULL;
//     return (path_dirs);
// }

// int is_directory(char *cmd, t_minishell *shell)
// {
//     struct stat sb;

//     if (stat(cmd, &sb) == 0 && S_ISDIR(sb.st_mode))
//     {
//         print_error_message(cmd, "Is a directory");
//         set_exit_status(shell, 126);
//         return (1);
//     }
//     return (0);
// }
////////////////////////////////