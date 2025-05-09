/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exe_cmd.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pzaw <pzaw@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 03:48:22 by pzaw              #+#    #+#             */
/*   Updated: 2025/04/24 03:48:23 by pzaw             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	execute_builtin(t_ast_node *ast, t_minishell *shell, char *cmd)
{
	if (ft_strcmp(cmd, "echo") == 0)
		return (exe_echo(ast));
	if (ft_strcmp(cmd, "cd") == 0)
		return (exe_cd(ast, shell));
	if (ft_strcmp(cmd, "pwd") == 0)
		return (exe_pwd(&shell));
	if (ft_strcmp(cmd, "export") == 0)
		return (exe_export(ast, shell));
	if (ft_strcmp(cmd, "unset") == 0)
		return (exe_unset(&shell));
	if (ft_strcmp(cmd, "env") == 0)
		return (exe_env(&shell));
	if (ft_strcmp(cmd, "exit") == 0)
		return (exe_exit(&shell, ast));
	return (-1);
}

void	execute_external_helper(t_ast_node *ast_cmd, t_minishell *shell)
{
	handle_child_signals();
	close_heredoc_fds(ast_cmd);
	close(shell->og_fd[FD_IN]);
	close(shell->og_fd[FD_OUT]);
	shell->env_path = get_env_array(shell);
	execve(shell->path, ast_cmd->cmd_arg, shell->env_path);
	perror("execve failed");
	free_array_list(shell->env_path, 0);
	cleanup(&shell);
	free_env_list(shell->envp);
	if (shell)
		free(shell);
}

int	execute_external_command(t_ast_node *ast_cmd, t_minishell *shell)
{
	pid_t	pid;
	int		status;

	g_signal_status = 2;
	pid = fork();
	if (pid < 0)
	{
		reset_close_fd(shell->og_fd);
		perror("fork failed");
		set_exit_status(shell, 1);
		return (-1);
	}
	if (pid == 0)
	{
		execute_external_helper(ast_cmd, shell);
		exit(127);
	}
	status = wait_for_child(pid);
	g_signal_status = 0;
	return (return_with_status(shell, status));
}

int	builtin_cmd_check(t_ast_node *ast, t_minishell *shell)
{
	int	ret;

	if (!ast || !ast->cmd_arg)
		return (return_with_status(shell, 0));
	ret = execute_builtin(ast, shell, ast->cmd_arg[0]);
	if (ret != -1)
		set_exit_status(shell, ret);
	return (ret);
}

int	exe_cmd(t_ast_node *node, t_minishell *shell)
{
	t_error_cmd	cmd_err;
	int			ret;

	shell->og_fd[FD_IN] = dup(STDIN_FILENO);
	shell->og_fd[FD_OUT] = dup(STDOUT_FILENO);
	if (node->redir)
	{
		if (handle_redirections(node->redir) == -1)
			return (reset_close_fd(shell->og_fd), return_with_status(shell, 1));
	}
	ret = builtin_cmd_check(node, shell);
	if (ret != -1)
		return (reset_close_fd(shell->og_fd), return_with_status(shell, ret));
	if (ft_strcmp(node->cmd_arg[0], "") == 0 && node->cmd_arg[1] == NULL)
		return (reset_close_fd(shell->og_fd),
			return_with_status(shell, shell->exit_status));
	cmd_err = search_cmd_path(node->cmd_arg[0], shell);
	if (cmd_err != OK_CMD)
		return (reset_close_fd(shell->og_fd),
			cmd_error_msg(cmd_err, node->cmd_arg[0], shell));
	ret = execute_external_command(node, shell);
	reset_close_fd(shell->og_fd);
	close_heredoc_fds(node);
	return (return_with_status(shell, ret));
}
