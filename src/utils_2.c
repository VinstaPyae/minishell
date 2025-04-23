/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pzaw <pzaw@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 02:11:02 by pzaw              #+#    #+#             */
/*   Updated: 2025/04/24 03:03:26 by pzaw             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

volatile sig_atomic_t	g_signal_status;

void	handle_sigint(int signo)
{
	(void)signo;
	if (g_signal_status == 2)
		return ;
	write(STDOUT_FILENO, "\n", 1);
	rl_replace_line("", 0);
	rl_on_new_line();
	rl_redisplay();
	g_signal_status = 130;
}

int	check_sigint(void)
{
	if (g_signal_status == 130)
	{
		rl_done = 1;
		return (1);
	}
	return (0);
}

void	handle_sigint_heredoc(int signo)
{
	(void)signo;
	g_signal_status = 130;
	rl_replace_line("", 0);
	rl_done = 1;
}

void	setup_signal_handlers(void)
{
	struct sigaction	sa;

	signal(SIGQUIT, SIG_IGN);
	sa.sa_handler = handle_sigint;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGTSTP, &sa, NULL);
}

void	handle_eof(t_minishell *shell)
{
	int	saved_exit_status;

	if (g_signal_status == 130)
		saved_exit_status = 130;
	else
		saved_exit_status = shell->exit_status;
	if (isatty(STDIN_FILENO))
		printf("exit\n");
	cleanup(&shell);
	if (shell->envp)
		free_env_list(shell->envp);
	if (shell->input)
		free(shell->input);
	rl_clear_history();
	free(shell);
	exit(saved_exit_status);
}
