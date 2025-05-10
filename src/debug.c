/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pzaw <pzaw@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 01:54:57 by pzaw              #+#    #+#             */
/*   Updated: 2025/04/24 01:54:57 by pzaw             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	free_array_list(char **path_dirs, int index)
{
	if (!path_dirs)
		return ;
	if (index < 0)
	{
		index = 0;
		while (path_dirs[index])
			index++;
		while (index-- > 0)
			free(path_dirs[index]);
	}
	else
	{
		while (index-- > 0)
			free(path_dirs[index]);
	}
	free(path_dirs);
}

void	init_tmp(int *tmp)
{
	tmp[0] = 0;
	tmp[1] = 0;
	tmp[2] = 0;
	tmp[3] = 0;
	tmp[4] = 0;
}

int	n_option_checked(const char *str)
{
	int	i;

	if (str[0] != '-')
		return (0);
	i = 1;
	while (str[i] == 'n')
		i++;
	return ((i > 1) && (str[i] == '\0'));
}
