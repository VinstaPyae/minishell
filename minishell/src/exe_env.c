#include "minishell.h"

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

void free_env(t_env *env)
{
    t_env *tmp;

    while (env)
    {
        tmp = env;
        env = env->next;
        free(tmp->key);
        free(tmp->value);
        free(tmp);
    }
}

void split_key_value(char *str, char **key, char **value)
{
    char *equal_sign;

    equal_sign = ft_strchr(str, '='); // Find the first '=' in the string
    if (!equal_sign)
    {
        *key = NULL;
        *value = NULL;
        return;
    }

    // Split the string into key and value
    *key = ft_strndup(str, equal_sign - str); // Copy everything before '='
    *value = ft_strdup(equal_sign + 1);       // Copy everything after '='
}

t_env *init_env(char **envp)
{
    t_env *env = NULL;
    t_env *new_node = NULL;
    char *key;
    char *value;
    int i = 0;

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
        new_node->next = env;
        env = new_node;
        i++;
    }
    return (env);
}

void print_env(t_env *env)
{
    t_env *tmp = env;

    while (tmp)
    {
        if (tmp->key && tmp->value) // Ensure key and value are not null
            printf("%s=%s\n", tmp->key, tmp->value);
        tmp = tmp->next;
    }
}

int exe_env(t_minishell **shell)
{
    if (!shell || !(*shell) || !(*shell)->envp)
    {
        printf("env: No environment variables found\n");
        return (1);
    }

    print_env((*shell)->envp);
    return (0);
}