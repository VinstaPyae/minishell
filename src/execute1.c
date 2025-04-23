/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute1.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pzaw <pzaw@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 03:48:50 by pzaw              #+#    #+#             */
/*   Updated: 2025/04/24 03:48:51 by pzaw             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*ft_getenv(t_env *env, const char *key)
{
	while (env)
	{
		if (ft_strcmp(env->key, key) == 0)
			return (env->value);
		env = env->next;
	}
	return (NULL);
}

char	*create_env_entry(char *key, char *value)
{
	char	*entry;
	size_t	key_len;
	size_t	value_len;
	size_t	total_len;

	key_len = ft_strlen(key);
	value_len = ft_strlen(value);
	total_len = key_len + value_len + 2;
	entry = malloc(total_len);
	if (!entry)
		return (NULL);
	ft_memcpy(entry, key, key_len);
	entry[key_len] = '=';
	ft_memcpy(entry + key_len + 1, value, value_len);
	entry[total_len - 1] = '\0';
	return (entry);
}

char	**fill_env_array(char **env_array, t_env *tmp, int i)
{
	while (tmp != NULL)
	{
		if (tmp->key != NULL && tmp->value != NULL)
		{
			env_array[i] = create_env_entry(tmp->key, tmp->value);
			if (env_array[i] == NULL)
			{
				free_array_list(env_array, i);
				return (NULL);
			}
			i++;
		}
		tmp = tmp->next;
	}
	env_array[i] = NULL;
	return (env_array);
}

char	**env_list_to_array(t_env *env)
{
	char	**env_array;
	t_env	*tmp;
	int		count;
	int		i;

	count = 0;
	tmp = env;
	i = 0;
	while (tmp != NULL)
	{
		if (tmp->key != NULL && tmp->value != NULL)
			count++;
		tmp = tmp->next;
	}
	env_array = malloc((count + 1) * sizeof(char *));
	if (env_array == NULL)
		return (NULL);
	tmp = env;
	env_array = fill_env_array(env_array, tmp, i);
	return (env_array);
}

void	handle_child_signals(void)
{
	struct sigaction	sa;

	sa.sa_handler = SIG_DFL;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGQUIT, &sa, NULL);
	sigaction(SIGTSTP, &sa, NULL);
}
