#include "minishell.h"

char *init_pwd(void)
{
	char *pwd = getenv("PWD");
	if (!pwd)
	{
		// Fallback: get the current working directory.
		char buffer[1024];
		if (getcwd(buffer, sizeof(buffer)) != NULL)
			pwd = strdup(buffer); // You need to free this later.
		else
			pwd = ft_strdup("/"); // Default fallback value.
	}
	else
	{
		// Duplicate it so you can store it in your internal env list.
		pwd = ft_strdup(pwd);
	}
	return pwd;
}
t_env *init_minimal_env(void)
{
	t_env *node_pwd;
	// t_env *node_shlvl;
	t_env *node_underscore;
	t_env *node_path;

	node_pwd = malloc(sizeof(t_env));
	// node_shlvl = malloc(sizeof(t_env));
	node_underscore = malloc(sizeof(t_env));
	node_path = malloc(sizeof(t_env));
	if (!node_pwd /*|| !node_shlvl*/ || !node_underscore || !node_path)
	{
		free(node_pwd);
		// free(node_shlvl);
		free(node_underscore);
		free(node_path);
		return NULL;
	}

	// Initialize PWD using getenv("PWD") or getcwd fallback
	node_pwd->key = ft_strdup("PWD");
	node_pwd->value = init_pwd();
	node_pwd->path = 0;
	node_pwd->next = NULL;

	// Initialize SHLVL
	// node_shlvl->key = ft_strdup("SHLVL");
	// node_shlvl->value = ft_strdup("1");
	// node_shlvl->path = 0;
	// node_shlvl->next = node_pwd; // Link PWD after SHLVL

	// Initialize "_" variable
	node_underscore->key = ft_strdup("_");
	node_underscore->value = ft_strdup("/usr/bin/env");
	node_underscore->path = 0;
	node_underscore->next = node_pwd; // Link PWD after SHLVL
	// node_underscore->next = node_shlvl; // Link SHLVL after _

	// Initialize PATH with default directories
	node_path->key = ft_strdup("PATH");
	node_path->value = ft_strdup("/bin:/usr/bin:/usr/local/bin");
	node_path->path = 1;
	node_path->next = node_underscore; // Link _ after PATH

	return node_path; // Head of the list
}

t_env *init_env(char **envp)
{
	t_env *env = NULL;
	t_env *new_node = NULL;
	char *key;
	char *value;
	int i = 0;

	if (!envp || envp[0] == NULL)
	{
		// If the passed envp is NULL or empty, create a minimal environment.
		env = init_minimal_env();
		if (!env)
			printf("Error: Failed to initialize minimal environment\n");
		update_shlvl(&env);
		return (env);
	}
	while (envp[i])
	{
		split_key_value(envp[i], &key, &value);
		if (!key || !value)
		{
			i++;
			continue; // Skip invalid entries
		}

		new_node = (t_env *)malloc(sizeof(t_env));
		if (!new_node)
			return (NULL);
		new_node->key = key;
		new_node->value = value;
		new_node->path = 0;
		new_node->next = env;
		env = new_node;
		i++;
	}
	update_shlvl(&env);
	return (env);
}

void update_shlvl(t_env **env_list)
{
	int lvl;
	t_env *env = *env_list;
	int found = 0;

	while (env)
	{
		if (ft_strcmp(env->key, "SHLVL") == 0)
		{
			lvl = atoi(env->value);
			lvl++;
			char *new_val = ft_itoa(lvl);
			if (!new_val)
			{
				perror("ft_itoa");
				return;
			}
			free(env->value);
			env->value = new_val;
			found = 1;
			break;
		}
		env = env->next;
	}
	if (!found)
		replace_or_add_env_var("SHLVL", "1", (*env_list));
	// If SHLVL is not found, add it with a starting value of 1.
}

