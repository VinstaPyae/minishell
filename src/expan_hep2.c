#include "minishell.h"

char **handle_empty_var_name(void)
{
    char *empty_str;
    char **result;

    empty_str = ft_strdup("");
    result = create_single_result(empty_str);
    return result;
}

char **get_valid_env_value(char **var_name, int c, t_minishell *shell)
{
    char **value;
    int i;

    value = malloc(sizeof(char *) * (c + 1));
    if (!value)
        return create_single_result(ft_strdup(""));
    i = -1;
    while (++i < c)
    {
        value[i] = get_env_value(shell->envp, var_name[i]);
    }
    value[i] = NULL;
    if (!value || value[0] == NULL)
    {
        free(value);
        return create_single_result(ft_strdup(""));
    }
    return value;
}

char **expand_env_variable(char **var_name, t_minishell *shell)
{
    char **value;
    int c;

    if (!var_name || !var_name[0])
        return handle_empty_var_name();
    c = 0;
    while (var_name[c])
        c++;
    value = get_valid_env_value(var_name, c, shell);
    if (!value || !value[0])
        return create_single_result(ft_strdup(""));
    return value;
}

void debug_print_expansion(char **result)
{
    int i;

    i = 0;
    while (result[i])
    {
        printf("expand: (%s)\n", result[i]);
        i++;
    }
}

char **expand_variable(char *var, t_minishell *shell)
{
    char **result;
    char **var2;

    if (!var)
        return (NULL);
    if (var[1] == '\0')
        return (expand_dollar_sign());
    if (var[1] == '?' && var[2] == '\0')
        return (expand_exit_status(shell));
    var2 = ft_split(var, '$');
    result = expand_env_variable(var2, shell);
    free_arg(var2);
    return (result);
}
