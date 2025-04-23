/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error_handle1.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pzaw <pzaw@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 01:55:16 by pzaw              #+#    #+#             */
/*   Updated: 2025/04/24 01:55:16 by pzaw             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	free_redir(void *redir)
{
	t_redir	*r;

	r = (t_redir *)redir;
	if (r)
	{
		if (r->file)
			free(r->file);
		free(r);
	}
}

void	free_arg(char **str)
{
	int	i;

	if (!str)
		return ;
	i = 0;
	while (str[i])
	{
		free(str[i]);
		str[i] = NULL;
		i++;
	}
	free(str);
	str = NULL;
}

void	remove_node(t_list **head, t_list *node_to_remove, void (*del)(void *))
{
	t_list	*current;
	t_list	*prev;

	current = *head;
	prev = NULL;
	while (current)
	{
		if (current == node_to_remove)
		{
			if (prev)
				prev->next = current->next;
			else
				*head = current->next;
			ft_lstdelone(current, del);
			return ;
		}
		prev = current;
		current = current->next;
	}
}

t_list	*destroy_token_node(t_list *node)
{
	t_list	*next;
	t_token	*content;

	if (!node)
		return (NULL);
	next = node->next;
	content = (t_token *)node->content;
	c_token_destroy(content);
	free(node);
	return (next);
}
