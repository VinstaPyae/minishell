#include "minishell.h"

#include "minishell.h"

char *get_env_value(t_env *env, char *key)
{
    char *result;

    if (!key)
        return ft_strdup("");
    while (env)
    {
        if (ft_strcmp(env->key, key) == 0)
        {
            if (env->value)
                return (ft_strdup(env->value));
            else
                return (ft_strdup(""));
        }
        env = env->next;
    }
    result = ft_strdup("");
    return result;
}

char **create_single_result(char *str)
{
    char **result;

    result = malloc(sizeof(char *) * 2);
    if (!result)
    {
        free(str);
        return (NULL);
    }
    result[0] = str;
    result[1] = NULL;
    return (result);
}

char **expand_dollar_sign(void)
{
    char *str;
    char **result;

    str = ft_strdup("$");
    if (!str)
        return (NULL);
    result = create_single_result(str);
    return result;
}

char **expand_exit_status(t_minishell *shell)
{
    char *exit_str;
    char **result;

    exit_str = ft_itoa(shell->exit_status);
    if (!exit_str)
        return (NULL);
    result = create_single_result(exit_str);
    return result;
}

static char **handle_empty_var_name(void)
{
    char *empty_str;
    char **result;

    empty_str = ft_strdup("");
    result = create_single_result(empty_str);
    return result;
}

static char **get_valid_env_value(char **var_name, int c, t_minishell *shell)
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

char *expand_quote_variable(char *var, t_minishell *shell)
{
    char *value;
    char *dup;

    if (!var)
        return (NULL);
    if (var[1] == '\0')
        return (ft_strdup("$"));
    if (var[1] == '?' && var[2] == '\0')
        return (ft_itoa(shell->exit_status));
    value = get_env_value(shell->envp, &var[1]);
    if (value)
    {
        dup = ft_strdup(value);
        free(value);
        return (dup);
    }
    return (ft_strdup(""));
}

char *extract_variable_name(const char *input, int *i)
{
    int j;
    char *var_name;

    j = *i + 1;
    if (input[j] == '?')
        j++;
    while (ft_isalnum(input[j]) || input[j] == '_')
        j++;
    var_name = ft_substr(input, *i, j - *i);
    *i = j;
    return var_name;
}

char *append_expanded_variable(char *result, char *var_name, t_minishell *shell)
{
    char *expanded;
    char *temp;

    expanded = expand_quote_variable(var_name, shell);
    free(var_name);
    temp = ft_strjoin(result, expanded);
    free(result);
    free(expanded);
    return temp;
}

char *append_normal_character(char *result, char c)
{
    char char_str[2];
    char *temp;

    char_str[0] = c;
    char_str[1] = '\0';
    temp = ft_strjoin(result, char_str);
    free(result);
    return temp;
}

char *expand_double_quotes(char *input, t_minishell *shell)
{
    char *result;
    int i;

    result = ft_strdup("");
    i = 0;
    while (input[i])
    {
        if (input[i] == '$')
        {
            char *var_name = extract_variable_name(input, &i);
            result = append_expanded_variable(result, var_name, shell);
        }
        else
        {
            result = append_normal_character(result, input[i]);
            i++;
        }
    }
    return result;
}
static void update_token_with_expansion(t_token *token, char *expanded_value, int space)
{
    char *new_token;

    new_token = ft_strdup(expanded_value);
    if (!new_token)
        return;

    free(token->token);
    token->token = new_token;
    token->type = TOKEN_WD;
    token->space = space;
}

static void handle_empty_expansion(t_token *token)
{
    char *empty_token;

    empty_token = ft_strdup("");
    if (!empty_token)
        return;

    free(token->token);
    token->token = empty_token;
    token->type = TOKEN_WD;
}

// static void free_expanded_value(char **expanded_value)
// {
//     int j;

//     if (!expanded_value)
//         return;

//     j = 0;
//     while (expanded_value[j])
//     {
//         free(expanded_value[j]);
//         j++;
//     }
//     free(expanded_value);
// }

static void process_double_quote_token(t_token *token, t_minishell *shell)
{
    char *d_qvalue;

    d_qvalue = expand_double_quotes(token->token, shell);
    if (!d_qvalue)
        return;

    free(token->token);
    token->token = d_qvalue;
}

char **split_expanded_value(char *expanded_value)
{
    char **result;
    int i;
    int word_count;
    int start;
    int len;
    int idx;

    result = NULL;
    i = 0;
    word_count = 0;

    // —— Pass 1: count words —— 
    while (expanded_value[i])
    {
        while (expanded_value[i] && ft_isspace((unsigned char)expanded_value[i]))
            i++;
        if (expanded_value[i] && !ft_isspace((unsigned char)expanded_value[i]))
        {
            word_count++;
            while (expanded_value[i] && !ft_isspace((unsigned char)expanded_value[i]))
                i++;
        }
    }

    result = malloc(sizeof(char *) * (word_count + 1));
    if (!result)
        return (NULL);

    // —— Pass 2: extract each word —— 
    i = 0;
    idx = 0;
    while (expanded_value[i] && idx < word_count)
    {
        while (expanded_value[i] && ft_isspace((unsigned char)expanded_value[i]))
            i++;
        start = i;
        while (expanded_value[i] && !ft_isspace((unsigned char)expanded_value[i]))
            i++;
        len = i - start;

        result[idx] = ft_substr(expanded_value, start, len);
        if (!result[idx])
        {
            while (idx-- > 0)
                free(result[idx]);
            free(result);
            return (NULL);
        }
        idx++;
    }
    result[word_count] = NULL;
    return (result);
}

static void process_variable_token(t_token *token, t_list **current, t_minishell *shell)
{
    char *result;
    char **expanded_value;
    char **split_value;
    t_list *insert_pos;
    t_list *next_save;
    int i;
    int space;
    int space1;

    result = NULL;
    expanded_value = expand_variable(token->token, shell);
    if (!expanded_value || !expanded_value[0])
    {
        handle_empty_expansion(token);
        free_arg(expanded_value);
        return;
    }

    i = 0;
    while (expanded_value[i])
    {
        char *tmp;

        if (!result)
            result = ft_strdup(expanded_value[i]);
        else
        {
            tmp = result;
            result = ft_strjoin(tmp, expanded_value[i]);
            free(tmp);
        }
        i++;
    }

    free_arg(expanded_value);
    split_value = split_expanded_value(result);
    if (split_value)
    {
        if (!split_value[0])
        {
            free_arg(split_value);
            split_value = create_single_result(ft_strdup(""));
        }
        else if (!split_value[1])
        {
            free_arg(split_value);
            split_value = create_single_result(ft_strdup(result));
        }
    }
    free(result);

    i = 0;
    insert_pos = *current;
    next_save = (*current)->next;
    space1 = token->space;

    while (split_value && split_value[i])
    {
        if (ft_strcmp(split_value[i], "") == 0)
        {
            update_token_with_expansion(token, split_value[i], 0);
        }
        else if (i == 0 && split_value[i + 1])
        {
            update_token_with_expansion(token, split_value[i], 1);
        }
        else if (i == 0 && !split_value[i + 1])
        {
            update_token_with_expansion(token, split_value[i], token->space);
        }
        else
        {
            t_list *new_node;

            space = split_value[i + 1] ? 1 : space1;
            new_node = create_token(ft_strdup(split_value[i]), TOKEN_WD, space);
            if (!new_node)
                break;

            new_node->next = insert_pos->next;
            insert_pos->next = new_node;
            insert_pos = new_node;
        }
        i++;
    }

    free_arg(split_value);
    *current = insert_pos;
}

void expand_tokens(t_minishell *shell)
{
    t_list *current;

    current = shell->l_token;
    while (current)
    {
        t_token *token;

        token = (t_token *)current->content;
        if (token->type == TOKEN_VARIABLE)
        {
            process_variable_token(token, &current, shell);
        }
        else if (token->type == TOKEN_DQUOTE)
        {
            process_double_quote_token(token, shell);
        }
        current = current->next;
    }
}
