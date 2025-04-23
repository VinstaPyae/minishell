/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser1.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pzaw <pzaw@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 02:58:50 by pzaw              #+#    #+#             */
/*   Updated: 2025/04/24 02:58:51 by pzaw             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	is_pipe_token(t_list *tokens)
{
	int	result;

	result = (tokens != NULL && token_content(tokens)->type == TOKEN_PIPE);
	return (result);
}

int	process_redirections(t_ast_node *cmd_node, t_list **tokens)
{
	t_list	*redir;
	t_list	*temp;

	redir = get_redir(tokens);
	if (!redir)
		return (0);
	if (cmd_node->redir)
	{
		temp = cmd_node->redir;
		while (temp->next)
			temp = temp->next;
		temp->next = redir;
	}
	else
	{
		cmd_node->redir = redir;
	}
	return (1);
}

int	count_strs(char **arr)
{
	int	i;

	i = 0;
	while (arr && arr[i])
		i++;
	return (i);
}

int	duplicate_strings(char **dest, char **src, int *index)
{
	int	i;

	i = 0;
	while (src && src[i])
	{
		dest[*index] = ft_strdup(src[i]);
		if (!dest[*index])
		{
			free_arg(dest);
			return (0);
		}
		(*index)++;
		i++;
	}
	return (1);
}

int	calculate_total_length(char **arg, char **new)
{
	int	arg_len;
	int	new_len;

	arg_len = count_strs(arg);
	new_len = count_strs(new);
	return (arg_len + new_len);
}
