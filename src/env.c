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
// Function to initialize the PWD environment variable node
t_env *init_pwd_node(void)
{
    t_env *node_pwd;

    node_pwd = malloc(sizeof(t_env));
    if (!node_pwd)
        return NULL;

    node_pwd->key = ft_strdup("PWD");
    node_pwd->value = init_pwd();
    node_pwd->next = NULL;

    if (!node_pwd->key || !node_pwd->value)
    {
        free(node_pwd->key);
        free(node_pwd->value);
        free(node_pwd);
        return NULL;
    }

    return node_pwd;
}

// Function to initialize the "_" environment variable node
t_env *init_underscore_node(t_env *node_pwd)
{
    t_env *node_underscore;

    node_underscore = malloc(sizeof(t_env));
    if (!node_underscore)
    {
        free_env_list(node_pwd);
        return NULL;
    }

    node_underscore->key = ft_strdup("_");
    node_underscore->value = ft_strdup("/usr/bin/env");
    node_underscore->next = node_pwd;

    if (!node_underscore->key || !node_underscore->value)
    {
        free(node_underscore->key);
        free(node_underscore->value);
        free(node_underscore);
        free_env_list(node_pwd);
        return NULL;
    }

    return node_underscore;
}

// Function to initialize the PATH environment variable node
t_env *init_path_node(t_env *node_underscore)
{
    t_env *node_path;

    node_path = malloc(sizeof(t_env));
    if (!node_path)
    {
        free_env_list(node_underscore);
        return NULL;
    }

    node_path->key = ft_strdup("PATH");
    node_path->value = ft_strdup("/bin:/usr/bin:/usr/local/bin");
    node_path->next = node_underscore;

    if (!node_path->key || !node_path->value)
    {
        free(node_path->key);
        free(node_path->value);
        free(node_path);
        free_env_list(node_underscore);
        return NULL;
    }

    return node_path;
}

// Main function to initialize the minimal environment
t_env *init_minimal_env(void)
{
    t_env *node_pwd;
    t_env *node_underscore;
    t_env *node_path;

    node_pwd = init_pwd_node();
    if (!node_pwd)
        return NULL;

    node_underscore = init_underscore_node(node_pwd);
    if (!node_underscore)
        return NULL;

    node_path = init_path_node(node_underscore);
    if (!node_path)
        return NULL;

    return node_path; // Head of the list
}