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

    if (!str)
    {
        *key = NULL;
        *value = NULL;
        return;
    }

    equal_sign = ft_strchr(str, '=');
    if (!equal_sign)
    {
        // No '=', meaning key is present but no value
        *key = ft_strdup(str);
        *value = ft_strdup(""); // Store empty string instead of NULL
        return;
    }

    // Split key and value
    *key = ft_strndup(str, equal_sign - str);
    *value = ft_strdup(equal_sign + 1);
}

void print_env(t_minishell *shell)
{
    t_env *tmp = shell->envp;
    while (tmp)
    {
        if (tmp->key && tmp->value) // Only print if value is NOT NULL
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

    print_env((*shell));
    return (0);
}
