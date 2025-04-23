/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute3.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pzaw <pzaw@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 03:48:58 by pzaw              #+#    #+#             */
/*   Updated: 2025/04/24 03:48:59 by pzaw             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	create_pipe(int pipe_fds[2], t_ast_node *pipe_node)
{
	if (pipe(pipe_fds) == -1)
	{
		close_heredoc_fds(pipe_node);
		perror("minishell: pipe Failed");
		return (-1);
	}
	return (0);
}

/* Helper function to handle left child process */
void	handle_left_child(t_ast_node *pipe_node,
	t_minishell *shell, int pipe_fds[2])
{
	int	ret;

	signal(SIGINT, handle_sigint);
	signal(SIGQUIT, SIG_DFL);
	close_heredoc_fds(pipe_node->right);
	close(pipe_fds[0]);
	if (dup2(pipe_fds[1], STDOUT_FILENO) == -1)
	{
		perror("minishell: dup2");
		cleanup(&shell);
		free_env_list(shell->envp);
		if (shell)
			free(shell);
		exit(1);
	}
	close(pipe_fds[1]);
	ret = execute_ast(pipe_node->left, shell);
	cleanup(&shell);
	free_env_list(shell->envp);
	if (shell)
		free(shell);
	exit(ret);
}

/* Helper function to handle right child process */
void	handle_right_child(t_ast_node *pipe_node,
	t_minishell *shell, int pipe_fds[2])
{
	int	ret;

	signal(SIGINT, handle_sigint);
	signal(SIGQUIT, SIG_DFL);
	close_heredoc_fds(pipe_node->left);
	close(pipe_fds[1]);
	if (dup2(pipe_fds[0], STDIN_FILENO) == -1)
	{
		perror("minishell: dup2");
		cleanup(&shell);
		free_env_list(shell->envp);
		if (shell)
			free(shell);
		exit(1);
	}
	close(pipe_fds[0]);
	ret = execute_ast(pipe_node->right, shell);
	cleanup(&shell);
	free_env_list(shell->envp);
	if (shell)
		free(shell);
	exit(ret);
}
