#include "minishell.h"

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

// Function to process a single export argument
static int process_single_export_arg(char *arg, t_minishell *shell)
{
    char *key;
    char *value;

    key = NULL;
    value = NULL;
    split_value(arg, &key, &value);
    if (!key || !is_valid_env_name(key))
    {
        printf("minishell: export: `%s`: not a valid identifier\n", arg);
        free(key);
        free(value);
        return (1);
    }
    add_or_update_env_var(key, value, shell);
    free(key);
    free(value);
    return (0);
}

// Function to process the arguments for export
static int process_export_args(t_ast_node *ast, t_minishell *shell)
{
    int i;
    int error_flag;
    char **cmd;

    error_flag = 0;
    if (!shell || !ast || !ast->cmd_arg)
        return (1);
    cmd = ast->cmd_arg;
    i = 1;
    while (cmd[i])
    {
        if (process_single_export_arg(cmd[i], shell) != 0)
            error_flag = 1;
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
int exe_export(t_ast_node *ast, t_minishell *shell)
{
    if (!shell || !ast)
        return (1); // Early exit if shell or AST is invalid
    if (!ast->cmd_arg[1])
        return process_export_no_args(shell);
    return process_export_args(ast, shell);
}
