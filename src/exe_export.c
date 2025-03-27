#include "minishell.h"

void split_value(char *str, char **key, char **value)
{
    char *equal_sign;
    //     char *quote_start;
    //     char *quote_end;

    if (!str) // Check if input string is NULL
    {
        *key = NULL;
        *value = NULL;
        return;
    }
    //     printf("cmd_arg : (%s)\n", str);
    equal_sign = ft_strchr(str, '='); // Find the first '=' in the string
    if (!equal_sign)
    {
        *key = ft_strdup(str); // If no '=', the whole string is the key
        *value = NULL;         // Set value to NULL
        if (!*key)
        {
            perror("malloc failed");
            return;
        }
        return;
    }

    // Split the string into key and value
    *key = ft_strndup(str, equal_sign - str); // Copy everything before '='
    if (!*key)
    {
        perror("malloc failed");
        return;
    }

    // Get the value part after '='
    *value = ft_strdup(equal_sign + 1);
    if (!*value)
    {
        perror("malloc failed");
        free(*key);
        return;
    }
}

// Function to add or update an environment variable in the envp linked list
static void add_or_update_env_var(const char *key, const char *value, t_minishell *shell)
{
    t_env *tmp = shell->envp;
    t_env *prev = NULL;

    while (tmp)
    {
        if (ft_strcmp(tmp->key, key) == 0)
        {
            if (value) // Only update the value if provided
            {
                free(tmp->value);
                tmp->value = ft_strdup(value);
                if (!tmp->value)
                {
                    perror("malloc failed for value");
                    return;
                }
            }
            return; // Variable already exists
        }
        prev = tmp;
        tmp = tmp->next;
    }

    // If variable does not exist, create a new one
    t_env *new_var = (t_env *)malloc(sizeof(t_env));
    if (!new_var)
    {
        perror("malloc failed for new variable");
        return;
    }

    new_var->key = ft_strdup(key);
    if (!new_var->key)
    {
        perror("malloc failed for key");
        free(new_var);
        return;
    }

    new_var->value = value ? ft_strdup(value) : NULL; // Allow NULL values

    if (value && !new_var->value)
    {
        perror("malloc failed for value");
        free(new_var->key);
        free(new_var);
        return;
    }

    new_var->next = NULL;
    if (prev)
        prev->next = new_var;
    else
        shell->envp = new_var;
}

// Helper function to check if a string is a valid environment variable name
static int is_valid_env_name(const char *name)
{
    if (!name || !*name)
        return (0);
    if (!ft_isalpha(*name) && *name != '_')
        return (0);
    while (*++name)
    {
        if (!ft_isalnum(*name) && *name != '_')
            return (0);
    }
    return (1);
}

// Function to process the arguments for export
static int process_export_args(t_minishell *shell)
{
    int i;
    char *key;
    char *value;
    char **cmd;
    int error_flag = 0;

    if (!shell || !shell->ast || !shell->ast->cmd_arg)
        return (1);
    cmd = shell->ast->cmd_arg;
    //     if (!cmd)
    //         return (1);
    i = 1;
    while (cmd[i])
    {
        // Split the string into key and value
        printf("cmd: (%s)\n", cmd[i]);
        if (cmd[i + 1])
        {
            cmd[i] = trim_last_char(cmd[i], ' ');
            printf("cmd: (%s)\n", cmd[i]);
        }
        split_value(cmd[i], &key, &value);

        if (!key || !is_valid_env_name(key))
        {
            printf("export: `%s`: not a valid identifier\n", cmd[i]);
            free(key);
            free(value);
            error_flag = 1;
            i++;
            continue;
        }
        // Add or update the environment variable
        add_or_update_env_var(key, value, shell);

        // Free allocated memory
        free(key);
        free(value);
        i++;
    }
    return (error_flag);
}

// Function to process export when no arguments are provided
static int process_export_no_args(t_minishell *shell)
{
    t_env *tmp = shell->envp;

    while (tmp)
    {
        if (tmp->key && tmp->value)
            printf("declare -x %s=\"%s\"\n", tmp->key, tmp->value);
        else if (tmp->key)
            printf("declare -x %s\n", tmp->key);
        tmp = tmp->next;
    }
    return (0);
}

// Main export function
int exe_export(t_minishell **shell)
{
    if (!shell || !*shell || !(*shell)->ast)
        return (1); // Early exit if shell or AST is invalid

    if (!(*shell)->ast->cmd_arg[1])
        return process_export_no_args(*shell);

    return process_export_args(*shell);
}
