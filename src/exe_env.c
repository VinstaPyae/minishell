#include "minishell.h"

t_env	*search_env_list(t_env *env_list, const char *name)
{
	while (env_list)
	{
		if (ft_strcmp(env_list->key, name) == 0)
			return (env_list);
		env_list = env_list->next;
	}
	return (NULL);
}

char *ft_strndup(const char *s, size_t n)
{
    char *dup;
    size_t len;

    len = ft_strlen(s);
    if (len > n)
        len = n;
    dup = (char *)malloc(len + 1);
    if (!dup)
        return (NULL);
    ft_memcpy(dup, s, len);
    dup[len] = '\0';
    return (dup);
}
void split_key_value(char *str, char **key, char **value)
{
    char *equal_sign;

    if (!str)
    {
        *key = NULL;
        *value = NULL;
        return;
    }
    equal_sign = ft_strchr(str, '=');
    if (!equal_sign)
    {
        *key = ft_strdup(str);
        *value = ft_strdup(""); // Store empty string instead of NULL
        return;
    }
    *key = ft_strndup(str, equal_sign - str);
    *value = ft_strdup(equal_sign + 1);
}
void print_env(t_minishell *shell)
{
    t_env *tmp;

    tmp = shell->envp;
    while (tmp)
    {
        if (tmp->key && tmp->value) // Only print if value is NOT NULL
            printf("%s=%s\n", tmp->key, tmp->value);
        tmp = tmp->next;
    }
}
int exe_env(t_minishell **shell)
{
    int path_exist;
    t_env *env;

    if (!shell || !*shell)
        return (1);
    env = (*shell)->envp;
    path_exist = 0;
    while (env)
    {
        if (ft_strcmp(env->key, "PATH") == 0)
        {
            path_exist = 1;
            break;
        }
        env = env->next;
    }
    if (!path_exist)
        return return_error(shell, "env: No such file or directory\n", 127);
    print_env(*shell);
    return return_with_status((*shell), 0); // Update shell status and return success
}
