/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exe_export1.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pzaw <pzaw@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 03:48:38 by pzaw              #+#    #+#             */
/*   Updated: 2025/04/24 03:48:39 by pzaw             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	handle_split_with_equal_sign(char *str, char *equal_sign,
	char **key, char **value)
{
	*key = ft_strndup(str, equal_sign - str);
	if (!*key)
	{
		perror("malloc failed");
		return ;
	}
	*value = ft_strdup(equal_sign + 1);
	if (!*value)
	{
		perror("malloc failed");
		free(*key);
		return ;
	}
}

void	split_value(char *str, char **key, char **value)
{
	char	*equal_sign;

	if (!str)
	{
		*key = NULL;
		*value = NULL;
		return ;
	}
	equal_sign = ft_strchr(str, '=');
	if (!equal_sign)
	{
		*key = ft_strdup(str);
		*value = NULL;
		if (!*key)
		{
			perror("malloc failed");
			return ;
		}
		return ;
	}
	handle_split_with_equal_sign(str, equal_sign, key, value);
}

int	init_new_env_var(const char *key, const char *value, t_env **new_var)
{
	*new_var = malloc(sizeof(t_env));
	if (!*new_var)
		return (perror("malloc failed for new variable"), 1);
	(*new_var)->key = ft_strdup(key);
	if (!(*new_var)->key)
	{
		perror("malloc failed for key");
		free(*new_var);
		return (1);
	}
	if (value)
		(*new_var)->value = ft_strdup(value);
	else
		(*new_var)->value = NULL;
	if (value && !(*new_var)->value)
	{
		perror("malloc failed for value");
		free((*new_var)->key);
		free(*new_var);
		return (1);
	}
	(*new_var)->next = NULL;
	return (0);
}

void	new_var_update(const char *key,
	const char *value, t_env *prev, t_minishell *shell)
{
	t_env	*new_var;
	int		result;

	result = init_new_env_var(key, value, &new_var);
	if (result != 0)
		return ;
	if (prev)
		prev->next = new_var;
	else
		shell->envp = new_var;
}

// Function to add or update an environment variable in the envp linked list
void	add_or_update_env_var(const char *key,
	const char *value, t_minishell *shell)
{
	t_env	*tmp;
	t_env	*prev;

	tmp = shell->envp;
	prev = NULL;
	while (tmp)
	{
		if (ft_strcmp(tmp->key, key) == 0)
		{
			if (value)
			{
				free(tmp->value);
				tmp->value = ft_strdup(value);
				if (!tmp->value)
				{
					perror("malloc failed for value");
					return ;
				}
			}
			return ;
		}
		prev = tmp;
		tmp = tmp->next;
	}
	new_var_update(key, value, prev, shell);
}
