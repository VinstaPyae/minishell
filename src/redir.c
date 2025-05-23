/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pzaw <pzaw@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 02:39:57 by pzaw              #+#    #+#             */
/*   Updated: 2025/04/24 02:39:57 by pzaw             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	setup_heredoc_signals(struct sigaction *old_sa)
{
	struct sigaction	new_sa;

	sigaction(SIGINT, NULL, old_sa);
	new_sa = *old_sa;
	new_sa.sa_handler = handle_sigint_heredoc;
	new_sa.sa_flags &= ~SA_RESTART;
	sigaction(SIGINT, &new_sa, NULL);
	rl_event_hook = check_sigint;
	g_signal_status = 0;
}

void	restore_heredoc_signals(struct sigaction *old_sa)
{
	sigaction(SIGINT, old_sa, NULL);
	rl_event_hook = NULL;
}

int	read_heredoc_loop(char *deli, t_minishell *shell, int write_fd)
{
	char	*line;
	char	*expanded_line;
	int		eof_warning;
	int		sh;

	eof_warning = 0;
	sh = 0;
	while (1)
	{
		line = readline("> ");
		if (!line)
		{
			if (!eof_warning)
			{
				process_eof_warning(deli);
				eof_warning = 1;
			}
			break ;
		}
		expanded_line = process_heredoc_line(line, deli, shell, &sh);
		if (sh)
			break ;
		write_expanded_line_to_pipe(expanded_line, write_fd);
	}
	return (0);
}

int	handle_heredoc(char *delimiter, t_minishell *shell)
{
	int					pipefd[2];
	struct sigaction	old_sa;
	int					result;

	if (pipe(pipefd) == -1)
	{
		perror("pipe");
		return (-1);
	}
	setup_heredoc_signals(&old_sa);
	read_heredoc_loop(delimiter, shell, pipefd[1]);
	close(pipefd[1]);
	restore_heredoc_signals(&old_sa);
	if (g_signal_status == 130)
	{
		close(pipefd[0]);
		return (-1);
	}
	result = pipefd[0];
	return (result);
}

int	handle_redirections(t_list *redir_list)
{
	t_redir	*redir;
	int		result;

	while (redir_list)
	{
		redir = (t_redir *)redir_list->content;
		if (redir->type == TOKEN_REDIRECT_OUT || redir->type == TOKEN_APPEND)
		{
			result = handle_output_redirection(redir);
			if (result == -1)
				return (-1);
		}
		else if (redir->type == TOKEN_REDIRECT_IN || redir->type == TOKEN_HDC)
		{
			result = handle_input_redirection(redir);
			if (result == -1)
				return (-1);
		}
		redir_list = redir_list->next;
	}
	return (0);
}
