/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env2.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pzaw <pzaw@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 01:55:09 by pzaw              #+#    #+#             */
/*   Updated: 2025/04/24 01:55:10 by pzaw             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// Helper function to handle minimal environment initialization
t_env	*handle_minimal_env(void)
{
	t_env	*env;

	env = init_minimal_env();
	if (!env)
	{
		printf("Error: Failed to initialize minimal environment\n");
		return (NULL);
	}
	update_shlvl(&env);
	return (env);
}

// Helper function to create a new environment node
t_env	*create_env_node(char *key, char *value, t_env *env)
{
	t_env	*new_node;

	new_node = malloc(sizeof(t_env));
	if (!new_node)
	{
		free(key);
		free(value);
		return (NULL);
	}
	new_node->key = key;
	new_node->value = value;
	new_node->next = env;
	return (new_node);
}

// Helper function to process a single envp entry
int	process_envp_entry(char *envp_entry, t_env **env)
{
	char	*key;
	char	*value;
	t_env	*new_node;

	key = NULL;
	value = NULL;
	split_key_value(envp_entry, &key, &value);
	if (!key || !value)
	{
		free(key);
		free(value);
		return (0);
	}
	new_node = create_env_node(key, value, *env);
	if (!new_node)
	{
		free_env_list(*env);
		return (-1);
	}
	*env = new_node;
	return (1);
}

// Function to initialize environment from envp
t_env	*init_env_from_envp(char **envp)
{
	t_env	*env;
	int		i;
	int		result;

	env = NULL;
	i = 0;
	while (envp[i])
	{
		result = process_envp_entry(envp[i], &env);
		if (result == -1)
			return (NULL);
		i++;
	}
	return (env);
}
