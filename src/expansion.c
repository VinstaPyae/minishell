/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansion.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pzaw <pzaw@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 03:49:21 by pzaw              #+#    #+#             */
/*   Updated: 2025/04/24 03:49:22 by pzaw             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	process_split_values(t_token *token,
	t_list **current, char **sv, int space1)
{
	t_list	*ip;
	int		i;

	i = 0;
	ip = *current;
	while (sv && sv[i])
	{
		if (i == 0)
		{
			handle_first_split_value(token, sv, i, space1);
		}
		else
		{
			ip = handle_remaining_split_values(ip, sv, i, space1);
			if (!ip)
				break ;
		}
		i++;
	}
	*current = ip;
}

static void	insert_split_values(t_token *token,
	t_list **current, char **split_value, int space1)
{
	process_split_values(token, current, split_value, space1);
}

static void	process_expanded_values(t_token *token,
	t_list **current, char **expanded_value, int space1)
{
	char	*result;
	char	**split_value;

	result = join_expanded_values(expanded_value);
	free_arg(expanded_value);
	split_value = split_expanded_value(result);
	split_value = handle_split_value(result, split_value);
	free(result);
	insert_split_values(token, current, split_value, space1);
	free_arg(split_value);
}

static void	process_variable_token(t_token *token,
	t_list **current, t_minishell *shell)
{
	char	**expanded_value;
	int		space1;

	expanded_value = expand_variable(token->token, shell);
	if (!expanded_value || !expanded_value[0])
	{
		handle_empty_expansion(token);
		free_arg(expanded_value);
		return ;
	}
	space1 = token->space;
	process_expanded_values(token, current, expanded_value, space1);
}

void	expand_tokens(t_minishell *shell)
{
	t_list	*current;
	t_token	*token;

	current = shell->l_token;
	while (current)
	{
		token = (t_token *)current->content;
		if (token->type == TOKEN_VARIABLE)
		{
			process_variable_token(token, &current, shell);
		}
		else if (token->type == TOKEN_DQUOTE)
		{
			process_double_quote_token(token, shell);
		}
		current = current->next;
	}
}
