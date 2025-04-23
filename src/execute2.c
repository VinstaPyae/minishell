/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute2.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pzaw <pzaw@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 03:48:53 by pzaw              #+#    #+#             */
/*   Updated: 2025/04/24 03:48:54 by pzaw             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	print_error_message(char *cmd, char *message)
{
	write(STDERR_FILENO, cmd, ft_strlen(cmd));
	write(STDERR_FILENO, ": ", 2);
	write(STDERR_FILENO, message, ft_strlen(message));
	write(STDERR_FILENO, "\n", 1);
}

char	**get_env_array(t_minishell *shell)
{
	char	**env_array;

	env_array = env_list_to_array(shell->envp);
	if (!env_array)
	{
		perror("env_list_to_array");
		set_exit_status(shell, 127);
	}
	return (env_array);
}

int	handle_no_path(char *cmd, t_minishell *shell)
{
	print_error_message(cmd, "No such file or directory");
	return (return_with_status(shell, 127));
}

void	print_signal_message(int sig)
{
	if (isatty(STDERR_FILENO))
	{
		if (sig == SIGQUIT)
			write(STDERR_FILENO, "Quit\n", 4);
	}
	write(STDERR_FILENO, "\n", 1);
}

void	reset_close_fd(int *org_fd)
{
	dup2(org_fd[FD_IN], STDIN_FILENO);
	dup2(org_fd[FD_OUT], STDOUT_FILENO);
	close(org_fd[FD_IN]);
	close(org_fd[FD_OUT]);
}
