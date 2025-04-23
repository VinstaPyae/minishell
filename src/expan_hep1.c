/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expan_hep1.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pzaw <pzaw@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 03:49:01 by pzaw              #+#    #+#             */
/*   Updated: 2025/04/24 03:49:02 by pzaw             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*get_env_value(t_env *env, char *key)
{
	char	*result;

	if (!key)
		return (ft_strdup(""));
	while (env)
	{
		if (ft_strcmp(env->key, key) == 0)
		{
			if (env->value)
				return (ft_strdup(env->value));
			else
				return (ft_strdup(""));
		}
		env = env->next;
	}
	result = ft_strdup("");
	return (result);
}

char	**create_single_result(char *str)
{
	char	**result;

	result = malloc(sizeof(char *) * 2);
	if (!result)
	{
		free(str);
		return (NULL);
	}
	result[0] = str;
	result[1] = NULL;
	return (result);
}

char	**expand_dollar_sign(void)
{
	char	*str;
	char	**result;

	str = ft_strdup("$");
	if (!str)
		return (NULL);
	result = create_single_result(str);
	return (result);
}

char	**expand_exit_status(t_minishell *shell)
{
	char	*exit_str;
	char	**result;

	exit_str = ft_itoa(shell->exit_status);
	if (!exit_str)
		return (NULL);
	result = create_single_result(exit_str);
	return (result);
}
