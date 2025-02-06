#include "minishell.h"

void split_value(char *str, char **key, char **value)
{
    char *equal_sign;
    char *quote_start;
    char *quote_end;

    if (!str) // Check if input string is NULL
    {
        *key = NULL;
        *value = NULL;
        return;
    }
    printf("cmd_arg : %s\n", str);
    equal_sign = ft_strchr(str, '='); // Find the first '=' in the string
    if (!equal_sign)
    {
        *key = ft_strdup(str);  // If no '=', the whole string is the key
        *value = NULL;          // Set value to NULL
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

    // Handle quoted values
    quote_start = *value;
    if (*quote_start == '"' || *quote_start == '\'')
    {
        quote_end = ft_strchr(quote_start + 1, *quote_start);
        if (quote_end)
        {
            // Remove the quotes by copying the substring between them
            char *unquoted_value = ft_strndup(quote_start + 1, quote_end - (quote_start + 1));
            if (unquoted_value)
            {
                free(*value);
                *value = unquoted_value;
            }
        }
    }

    /* 
     * Do NOT free the value if it's an empty string.
     * If the original token contained an '=', we want to preserve the empty value.
     * (Thus, "c=" produces key "c" with value "" while "c" produces key "c" with value NULL.)
     */
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
            free(tmp->value); // Free the old value
            if (value)
                tmp->value = ft_strdup(value); // Set new value if provided
            else
                tmp->value = NULL; // Set value to NULL if not provided
            if (value && !tmp->value) {
                perror("malloc failed for value");
                return;
            }
            return;
        }
        prev = tmp;
        tmp = tmp->next;
    }

    t_env *new_var = (t_env *)malloc(sizeof(t_env));
    if (!new_var)
    {
        perror("malloc failed for new variable");
        return;
    }

    new_var->key = ft_strdup(key);
    if (!new_var->key) {
        perror("malloc failed for key");
        free(new_var);
        return;
    }

    if (value)
        new_var->value = ft_strdup(value); // Set value if provided
    else
        new_var->value = NULL; // Set value to NULL if not provided

    if (value && !new_var->value) {
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

    i = 0;
    if (!shell || !shell->ast || !shell->ast->cmd_arg)
        return (1);

    while (shell->ast->cmd_arg[i])
    {
        // Split the string into key and value
        split_value(shell->ast->cmd_arg[i], &key, &value);

        if (!key || !is_valid_env_name(key))
        {
            printf("export: `%s`: not a valid identifier\n", shell->ast->cmd_arg[i]);
            free(key);
            free(value);
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
    return (0);
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
        return (1);  // Early exit if shell or AST is invalid

    if (!(*shell)->ast->cmd_arg || !(*shell)->ast->cmd_arg[0])
        return process_export_no_args(*shell);

    return process_export_args(*shell);
}

