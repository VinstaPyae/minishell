/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pzaw <pzaw@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 02:14:36 by pzaw              #+#    #+#             */
/*   Updated: 2025/04/24 02:14:36 by pzaw             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_list	*create_token(char *str, int type, int s)
{
	t_token	*c_token;
	t_list	*token;

	c_token = malloc(sizeof(t_token));
	if (c_token == NULL)
		return (NULL);
	c_token->token = str;
	c_token->type = type;
	c_token->space = s;
	token = ft_lstnew(c_token);
	if (token == NULL)
		return (free(c_token), NULL);
	token->next = NULL;
	return (token);
}

t_token	*token_content(t_list *token)
{
	return ((t_token *)token->content);
}

int	handle_input_and_signals(t_minishell *shell)
{
	shell->input = get_input(shell);
	if (g_signal_status == 130)
	{
		shell->exit_status = 130;
		g_signal_status = 0;
	}
	if (!shell->input || ft_strlen(shell->input) == 0)
	{
		free(shell->input);
		shell->input = NULL;
		return (0);
	}
	return (1);
}
