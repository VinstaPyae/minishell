#include "minishell.h"

t_env *find_and_update_env_var(const char *name, const char *value, t_env *envp, int *found)
{
    t_env *env;

    env = envp;
    *found = 0;
    while (env)
    {
        if (ft_strcmp(env->key, name) == 0)
        {
            if (env->value)
                free(env->value);
            env->value = ft_strdup(value); // Assign the new value
            *found = 1;
            break;
        }
        env = env->next;
    }
    return (envp);
}

t_env *add_new_env_var(const char *name, const char *value, t_env *envp)
{
    t_env *new_env;
    t_env *last;

    new_env = malloc(sizeof(t_env));
    if (!new_env)
        return (envp); // Handle allocation failure
    new_env->key = ft_strdup(name);
    new_env->value = ft_strdup(value);
    new_env->next = NULL;

    if (!envp) // If the list was empty
        return (new_env);

    last = envp;
    while (last->next)
        last = last->next;
    last->next = new_env;
    return (envp);
}
