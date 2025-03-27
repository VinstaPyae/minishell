/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstdelone.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pzaw <pzaw@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/08 15:35:24 by pzaw              #+#    #+#             */
/*   Updated: 2025/01/08 15:35:25 by pzaw             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

/*
 * Function that deletes the list element 'lst' and frees its content, using
 * the function 'del'.
 */
void	ft_lstdelone(t_list *node, void (*del)(void *))
{
	if (node != NULL)
	{
		if (del != NULL)
			del(node->content);
		free(node);
	}
}
