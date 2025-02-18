#include "minishell.h"


// Helper function to replace or add an environment variable
t_env *replace_or_add_env_var(const char *name, const char *value, t_env *envp)
{
    t_env *env = envp;
    int found = 0;

    // Iterate through the environment linked list
    while (env)
    {
        // printf("update pwd in env : %s\n", env->key);
        if (ft_strcmp(env->key, name) == 0)
        {
            // printf("found pwd in env : %s\n", name);
            // Replace the value if the key matches
            free(env->value);  // Free the old value
            env->value = ft_strdup(value);  // Assign the new value
            found = 1;
            // printf("found pwd in env : %s\n", env->value);
            break;
        }
        env = env->next;
    }

    if (!found)
    {
        // If the variable is not found, add a new one
        t_env *new_env = malloc(sizeof(t_env));
        if (!new_env)
            return envp;  // Handle allocation failure

        new_env->key = ft_strdup(name);
        new_env->value = ft_strdup(value);
        new_env->next = envp;
        envp = new_env;
    }
    return (envp);
}

char    *get_oldpwd(char *key, t_minishell **shell)
{
    t_env *env = (*shell)->envp;

    while (env)
    {
        if (ft_strcmp(env->key, key) == 0)
            return (ft_strdup(env->value));
        env = env->next;
    }
    return (NULL);
}


static void update_env_vars(t_minishell **shell)
{
    char cwd[1024];
    char *oldpwd = get_oldpwd("PWD", shell);
    if (!oldpwd)
        return ;
    // printf("OldPWD: %s\n", oldpwd);
    char *new_oldpwd;

    // Update OLDPWD
    if (oldpwd)
    {
        new_oldpwd = ft_strdup(oldpwd);  // Duplicate the old PWD
        if (new_oldpwd)
        {
            // Update the custom environment list
            replace_or_add_env_var("OLDPWD", new_oldpwd, (*shell)->envp);
            free(new_oldpwd);
        }
    }

    // Update PWD
    if (getcwd(cwd, sizeof(cwd)))
    {
        replace_or_add_env_var("PWD", cwd, (*shell)->envp);
    }
    else
    {
        perror("pwd");
    }
}


static char *path_handle(t_minishell **shell)
{
    char *dir;
    if (!(*shell)->ast->cmd_arg[1] || ft_strncmp((*shell)->ast->cmd_arg[1], "~", 2) == 0)
    {
        dir = getenv("HOME");
        if (!dir)
        {
            printf("cd: HOME not set\n");
            return (NULL); 
        }
        printf("You are in Home directory\n");
    }
    else if (ft_strncmp((*shell)->ast->cmd_arg[1], "-", 2) == 0)
    {
        dir = getenv("OLDPWD");
        if (!dir)
        {
            printf("cd: OLDPWD not set\n");
            return (NULL);
        }
        printf("%s\n", dir);
    }
    else
    {
        dir = (*shell)->ast->cmd_arg[1];
        printf("You are in Other directory\n");
    }
    return (dir);
}

int exe_cd(t_minishell **shell)
{
    char *path;
    char *curr_dir;
    if (!shell || !*shell || !(*shell)->ast)
        return (1);
    if ((*shell)->ast->cmd_arg[0] && (*shell)->ast->cmd_arg[1] && (*shell)->ast->cmd_arg[2])
        return (printf("cd: Too many arguments\n"), 1);
    path = path_handle(shell);
    if (!path)
        return (1);
    curr_dir = getcwd(NULL, 0);
    if (!curr_dir)
        return (printf("cd: getcwd Failed\n"), 1);
    if (chdir(path) != 0)
    {
        free(curr_dir);
        return (printf("cd: chdir Failed\n"), 1);
    }
    free(curr_dir);
    update_env_vars(shell);
    return (0);
}

