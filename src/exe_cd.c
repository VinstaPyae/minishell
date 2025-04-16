#include "minishell.h"

// Helper function to replace or add an environment variable
t_env *replace_or_add_env_var(const char *name, const char *value, t_env *envp)
{
    t_env *env = envp;
    t_env *head = envp; // Store the head of the list
    int found = 0;

    // Iterate through the environment linked list
    while (env)
    {
        // printf("update in env : %s\n", env->key);
        if (ft_strcmp(env->key, name) == 0)
        {
            //     printf("found in env : %s\n", name);
            // Replace the value if the key matches
            free(env->value);              // Free the old value
            env->value = ft_strdup(value); // Assign the new value
            found = 1;
            //     printf("found in env : %s\n", env->value);
            break;
        }
        env = env->next;
    }

    if (!found)
    {
        // printf("Not found in env : %s\n", name);
        // If the variable is not found, add a new one
        t_env *new_env = malloc(sizeof(t_env));
        if (!new_env)
            return envp; // Handle allocation failure

        new_env->key = ft_strdup(name);
        new_env->value = ft_strdup(value);
        new_env->next = NULL;

        // Add the new node to the end of the list
        if (!head) // If the list was empty
        {
            head = new_env;
        }
        else
        {
            // Find the last node
            t_env *last = head;
            while (last->next)
                last = last->next;

            // Append the new node
            last->next = new_env;
        }

        // printf("added to env : %s\n", new_env->key);
    }

    return head; // Return the head of the list
}

char *get_oldpwd(char *key, t_minishell **shell)
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
        oldpwd = ft_strdup(""); // If PWD is not found, set oldpwd to an empty string

    // Update OLDPWD
    if (oldpwd)
    {
        replace_or_add_env_var("OLDPWD", oldpwd, (*shell)->envp);
        free(oldpwd);
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

static char *path_handle(t_ast_node *ast, t_minishell *shell)
{
    char *dir;
    if (!ast->cmd_arg[1] || ft_strncmp(ast->cmd_arg[1], "~", 2) == 0)
    {
        dir = ft_getenv((shell)->envp, "HOME");
        if (!dir)
        {
            printf("cd: HOME not set\n");
            return (NULL);
        }
        printf("You are in Home directory\n");
    }
    else if (ft_strncmp(ast->cmd_arg[1], "-", 2) == 0)
    {
        dir = ft_getenv((shell)->envp, "OLDPWD");
        if (!dir)
        {
            printf("cd: OLDPWD not set\n");
            return (NULL);
        }
        printf("%s\n", dir);
    }
    else
    {
        dir = ast->cmd_arg[1];
        printf("You are in Other directory\n");
    }
    return (dir);
}
void ft_fprintf(int fd, const char *format, ...)
{
    va_list args;
    va_start(args, format);

    while (*format)
    {
        if (*format == '%')
        {
            format++;
            if (*format == 's')
            {
                char *str = va_arg(args, char *);
                ft_putstr_fd(str, fd);
            }
            else if (*format == 'd')
            {
                int num = va_arg(args, int);
                ft_putnbr_fd(num, fd);
            }
            else if (*format == 'c')
            {
                char c = (char)va_arg(args, int);
                ft_putchar_fd(c, fd);
            }
            else if (*format == '%')
                ft_putchar_fd('%', fd);
            format++;
        }
        else
            ft_putchar_fd(*format++, fd);
    }
    va_end(args);
}
int exe_cd(t_ast_node *ast, t_minishell *shell)
{
    char *path;
    char *curr_dir;

    if (!ast)
        return (1); // Error: Invalid shell/ast

    // Case: Too many arguments (e.g., `cd dir1 dir2`)
    if (ast->cmd_arg[1] && ast->cmd_arg[2])
    {
        ft_fprintf(2, "cd: too many arguments\n");
        return (1); // Exit status 1
    }

    path = path_handle(ast, shell); // Resolve path (HOME/OLDPWD/custom dir)
    if (!path)
        return (1); // Error: HOME/OLDPWD unset or invalid

    curr_dir = getcwd(NULL, 0); // Save current dir for OLDPWD
    if (!curr_dir)
    {
        ft_fprintf(2, "cd: error retrieving current directory\n");
        return (1); // Exit status 1
    }

    if (chdir(path) != 0) // Try changing directory
    {
        ft_fprintf(2, "cd: %s: %s\n", path, strerror(errno));
        free(curr_dir);
        return (1); // Exit status 1
    }

    // Update PWD and OLDPWD in env
    update_env_vars(&shell);
    free(curr_dir);
    return (0); // Success: Exit status 0
}