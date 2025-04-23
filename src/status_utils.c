/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   status_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pzaw <pzaw@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 02:16:36 by pzaw              #+#    #+#             */
/*   Updated: 2025/04/24 02:16:39 by pzaw             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	set_exit_status(t_minishell *shell, int status)
{
	if (shell)
		shell->exit_status = status;
}

int	return_with_status(t_minishell *shell, int status)
{
	if (shell)
		(shell)->exit_status = status;
	return (status);
}

int	return_error(t_minishell **shell, const char *msg, int status)
{
	if (msg)
		ft_putstr_fd((char *)msg, STDERR_FILENO);
	return (return_with_status((*shell), status));
}

t_minishell	*create_minshell(t_env *envp)
{
	t_minishell	*shell;

	shell = malloc(sizeof(t_minishell));
	if (!shell)
		return (NULL);
	shell = (t_minishell *)ft_memset(shell, 0, sizeof(t_minishell));
	shell->ast = NULL;
	shell->input = NULL;
	shell->l_token = NULL;
	shell->envp = envp;
	if (!shell->envp)
	{
		free(shell);
		return (NULL);
	}
	shell->exit_status = 0;
	return (shell);
}

t_minishell	*initialize_shell(char **env)
{
	t_env		*envp;
	t_minishell	*shell;

	envp = init_env(env);
	if (!envp)
	{
		printf("Error: Failed to initialize environment\n");
		return (NULL);
	}
	shell = create_minshell(envp);
	if (!shell)
	{
		printf("Error: Failed to create minishell\n");
		free_env_list(envp);
		return (NULL);
	}
	return (shell);
}
