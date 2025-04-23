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

// Helper function to handle minimal environment initialization
t_env *handle_minimal_env(void)
{
    t_env *env;

    env = init_minimal_env();
    if (!env)
    {
        printf("Error: Failed to initialize minimal environment\n");
        return NULL;
    }
    update_shlvl(&env);
    return env;
}

// Helper function to create a new environment node
t_env *create_env_node(char *key, char *value, t_env *env)
{
    t_env *new_node;

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

    return new_node;
}

// Helper function to process a single envp entry
int process_envp_entry(char *envp_entry, t_env **env)
{
    char *key;
    char *value;
    t_env *new_node;

    key = NULL;
    value = NULL;
    split_key_value(envp_entry, &key, &value);

    if (!key || !value)
    {
        free(key);
        free(value);
        return 0; // Skip invalid entry
    }

    new_node = create_env_node(key, value, *env);
    if (!new_node)
    {
        free_env_list(*env);
        return -1; // Memory allocation failure
    }

    *env = new_node;
    return 1; // Successfully processed
}

// Function to initialize environment from envp
t_env *init_env_from_envp(char **envp)
{
    t_env *env;
    int i;
    int result;

    env = NULL;
    i = 0;

    while (envp[i])
    {
        result = process_envp_entry(envp[i], &env);
        if (result == -1)
            return NULL; // Memory allocation failure
        i++;
    }

    return env;
}

// Main function to initialize the environment
t_env *init_env(char **envp)
{
    t_env *env;

    if (!envp || envp[0] == NULL)
    {
        env = handle_minimal_env();
        if (!env)
            return NULL;
    }
    else
    {
        env = init_env_from_envp(envp);
        if (!env)
        {
            printf("Error: Failed to initialize environment from envp\n");
            return NULL;
        }
    }

    update_shlvl(&env);
    return env;
}
/////////////////
// Helper function to find the SHLVL node in the environment list
t_env *find_shlvl_node(t_env *env_list)
{
    t_env *env;

    env = env_list;
    while (env)
    {
        if (ft_strcmp(env->key, "SHLVL") == 0)
            return env;
        env = env->next;
    }
    return NULL;
}

// Helper function to increment the SHLVL value
void increment_shlvl_value(t_env *shlvl_node)
{
    int lvl;
    char *new_val;

    lvl = ft_atoi(shlvl_node->value);
    lvl++;
    new_val = ft_itoa(lvl);
    if (!new_val)
    {
        perror("ft_itoa");
        return;
    }
    free(shlvl_node->value);
    shlvl_node->value = new_val;
}

// Helper function to add SHLVL if it does not exist
void add_shlvl_if_missing(t_env **env_list)
{
    replace_or_add_env_var("SHLVL", "1", *env_list);
}

// Main function to update the SHLVL environment variable
void update_shlvl(t_env **env_list)
{
    t_env *shlvl_node;

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
