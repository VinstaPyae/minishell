#include "minishell.h"

static void handle_split_with_equal_sign(char *str, char *equal_sign, char **key, char **value)
{
    *key = ft_strndup(str, equal_sign - str);
    if (!*key)
    {
        perror("malloc failed");
        return;
    }
    *value = ft_strdup(equal_sign + 1);
    if (!*value)
    {
        perror("malloc failed");
        free(*key);
        return;
    }
}

void split_value(char *str, char **key, char **value)
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
        *key = ft_strdup(str);
        *value = NULL;
        if (!*key)
        {
            perror("malloc failed");
            return;
        }
        return;
    }
    handle_split_with_equal_sign(str, equal_sign, key, value);
}

static int init_new_env_var(const char *key, const char *value, t_env **new_var)
{
    *new_var = malloc(sizeof(t_env));
    if (!*new_var)
        return (perror("malloc failed for new variable"), 1);
    (*new_var)->key = ft_strdup(key);
    if (!(*new_var)->key)
    {
        perror("malloc failed for key");
        free(*new_var);
        return (1);
    }
    if (value)
        (*new_var)->value = ft_strdup(value);
    else
        (*new_var)->value = NULL;
    if (value && !(*new_var)->value)
    {
        perror("malloc failed for value");
        free((*new_var)->key);
        free(*new_var);
        return (1);
    }
    (*new_var)->next = NULL;
    return (0);
}

void new_var_update(const char *key, const char *value, t_env *prev, t_minishell *shell)
{
    t_env *new_var;
    int result;

    result = init_new_env_var(key, value, &new_var);
    if (result != 0)
        return;
    if (prev)
        prev->next = new_var;
    else
        shell->envp = new_var;
}


// Function to add or update an environment variable in the envp linked list
static void add_or_update_env_var(const char *key, const char *value, t_minishell *shell)
{
    t_env *tmp;
    t_env *prev;

    tmp = shell->envp;
    prev = NULL;
    while (tmp)
    {
        if (ft_strcmp(tmp->key, key) == 0)
        {
            if (value)
            {
                free(tmp->value);
                tmp->value = ft_strdup(value);
                if (!tmp->value)
                {
                    perror("malloc failed for value");
                    return;
                }
            }
            return;
        }
        prev = tmp;
        tmp = tmp->next;
    }
    new_var_update(key, value, prev, shell);
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
