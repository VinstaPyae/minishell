/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pzaw <pzaw@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 03:49:30 by pzaw              #+#    #+#             */
/*   Updated: 2025/04/24 03:49:30 by pzaw             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*get_input(t_minishell *shell)
{
	char	*input;

	input = readline("minishell$> ");
	if (!input)
	{
		handle_eof(shell);
		return (NULL);
	}
	if (*input)
		add_history(input);
	return (input);
}

int	process_tokens_and_ast(t_minishell *shell)
{
	shell->l_token = lexer(shell);
	if (!shell->l_token)
	{
		shell->exit_status = 1;
		cleanup(&shell);
		return (0);
	}
	expand_tokens(shell);
	shell->ast = parse_pipe(shell->l_token);
	if (!shell->ast)
	{
		shell->exit_status = 1;
		printf("Error: AST creation failed\n");
		cleanup(&shell);
		return (0);
	}
	return (1);
}

// Function to handle heredocs and execute AST
int	handle_heredocs_and_execute(t_minishell *shell)
{
	if (process_heredocs(shell->ast, shell) == -1)
	{
		if (g_signal_status == 130)
			shell->exit_status = 130;
		else
			shell->exit_status = 1;
		g_signal_status = 0;
		close_heredoc_fds(shell->ast);
		cleanup(&shell);
		return (0);
	}
	shell->exit_status = execute_ast(shell->ast, shell);
	close_heredoc_fds(shell->ast);
	return (1);
}

// Main loop for minishell
void	minishell_loop(t_minishell *shell)
{
	while (1)
	{
		if (!handle_input_and_signals(shell))
			continue ;
		if (!process_tokens_and_ast(shell))
			continue ;
		if (!handle_heredocs_and_execute(shell))
			continue ;
	}
}

// Main function
int	main(int ac, char **av, char **env)
{
	t_minishell	*shell;

	(void)ac;
	(void)av;
	g_signal_status = 0;
	setup_signal_handlers();
	shell = initialize_shell(env);
	if (!shell)
		return (1);
	minishell_loop(shell);
	cleanup(&shell);
	if (shell->envp)
		free_env_list(shell->envp);
	free(shell);
	rl_clear_history();
	return (0);
}
