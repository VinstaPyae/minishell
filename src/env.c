#include "minishell.h"

char *init_pwd(void)
{
    char *pwd;
    char buffer[1024];

    pwd = getenv("PWD");
    if (!pwd)
    {
        if (getcwd(buffer, sizeof(buffer)) != NULL)
            pwd = strdup(buffer); // You need to free this later.
        else
            pwd = ft_strdup("/"); // Default fallback value.
    }
    else
    {
        pwd = ft_strdup(pwd); // Duplicate it so you can store it in your internal env list.
    }
    return pwd;
}

t_env *init_minimal_env(void)
{
    t_env *node_pwd;
    t_env *node_underscore;
    t_env *node_path;

    node_pwd = malloc(sizeof(t_env));
    node_underscore = malloc(sizeof(t_env));
    node_path = malloc(sizeof(t_env));
    if (!node_pwd || !node_underscore || !node_path)
    {
        free(node_pwd);
        free(node_underscore);
        free(node_path);
        return NULL;
    }

    // Initialize PWD
    node_pwd->key = ft_strdup("PWD");
    node_pwd->value = init_pwd();
    node_pwd->next = NULL;
    if (!node_pwd->key || !node_pwd->value)
    {
        free(node_pwd->key);
        free(node_pwd->value);
        free(node_pwd);
        free(node_underscore);
        free(node_path);
        return NULL;
    }

    // Initialize "_"
    node_underscore->key = ft_strdup("_");
    node_underscore->value = ft_strdup("/usr/bin/env");
    node_underscore->next = node_pwd;
    if (!node_underscore->key || !node_underscore->value)
    {
        free(node_pwd->key);
        free(node_pwd->value);
        free(node_pwd);
        free(node_underscore->key);
        free(node_underscore->value);
        free(node_underscore);
        free(node_path);
        return NULL;
    }

    // Initialize PATH
    node_path->key = ft_strdup("PATH");
    node_path->value = ft_strdup("/bin:/usr/bin:/usr/local/bin");
    node_path->next = node_underscore;
    if (!node_path->key || !node_path->value)
    {
        free(node_pwd->key);
        free(node_pwd->value);
        free(node_pwd);
        free(node_underscore->key);
        free(node_underscore->value);
        free(node_underscore);
        free(node_path->key);
        free(node_path->value);
        free(node_path);
        return NULL;
    }

    return node_path; // Head of the list
}

t_env *init_env(char **envp)
{
    t_env *env;
    t_env *new_node;
    char *key;
    char *value;
    int i;

    env = NULL;
    new_node = NULL;
    key = NULL;
    value = NULL;
    i = 0;

    if (!envp || envp[0] == NULL)
    {
        env = init_minimal_env();
        if (!env)
        {
            printf("Error: Failed to initialize minimal environment\n");
            return NULL;
        }
        update_shlvl(&env);
        return env;
    }

    while (envp[i])
    {
        split_key_value(envp[i], &key, &value);
        if (!key || !value)
        {
            free(key);
            free(value);
            i++;
            continue;
        }

        new_node = malloc(sizeof(t_env));
        if (!new_node)
        {
            free(key);
            free(value);
            return NULL;
        }

        new_node->key = key;
        new_node->value = value;
        new_node->next = env;
        env = new_node;
        i++;
    }

    update_shlvl(&env);
    return env;
}

void update_shlvl(t_env **env_list)
{
    t_env *env;
    int lvl;
    char *new_val;
    int found;

    env = *env_list;
    found = 0;

    while (env)
    {
        if (ft_strcmp(env->key, "SHLVL") == 0)
        {
            lvl = ft_atoi(env->value);
            lvl++;
            new_val = ft_itoa(lvl);
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
        replace_or_add_env_var("SHLVL", "1", *env_list);
}
