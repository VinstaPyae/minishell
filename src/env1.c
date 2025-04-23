/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env1.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pzaw <pzaw@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 01:55:05 by pzaw              #+#    #+#             */
/*   Updated: 2025/04/24 01:55:06 by pzaw             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// Main function to initialize the environment
t_env	*init_env(char **envp)
{
	t_env	*env;

	if (!envp || envp[0] == NULL)
	{
		env = handle_minimal_env();
		if (!env)
			return (NULL);
	}
	else
	{
		env = init_env_from_envp(envp);
		if (!env)
		{
			printf("Error: Failed to initialize environment from envp\n");
			return (NULL);
		}
	}
	update_shlvl(&env);
	return (env);
}

// Helper function to find the SHLVL node in the environment list
t_env	*find_shlvl_node(t_env *env_list)
{
	t_env	*env;

	env = env_list;
	while (env)
	{
		if (ft_strcmp(env->key, "SHLVL") == 0)
			return (env);
		env = env->next;
	}
	return (NULL);
}

// Helper function to increment the SHLVL value
void	increment_shlvl_value(t_env *shlvl_node)
{
	char	*new_val;
	int		lvl;

	lvl = ft_atoi(shlvl_node->value);
	lvl++;
	new_val = ft_itoa(lvl);
	if (!new_val)
	{
		perror("ft_itoa");
		return ;
	}
	free(shlvl_node->value);
	shlvl_node->value = new_val;
}

// Helper function to add SHLVL if it does not exist
void	add_shlvl_if_missing(t_env **env_list)
{
	replace_or_add_env_var("SHLVL", "1", *env_list);
}

// Main function to update the SHLVL environment variable
void	update_shlvl(t_env **env_list)
{
	t_env	*shlvl_node;

	shlvl_node = find_shlvl_node(*env_list);
	if (shlvl_node)
	{
		increment_shlvl_value(shlvl_node);
	}
	else
	{
		add_shlvl_if_missing(env_list);
	}
}
