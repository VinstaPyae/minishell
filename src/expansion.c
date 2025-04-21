#include "minishell.h"

char *get_env_value(t_env *env, char *key)
{
    if (!key) // Add null check
        return ft_strdup("");
    while (env)
    {
        if (ft_strcmp(env->key, key) == 0)
        {
            if (env->value) // Check if env->value is not NULL
                return (ft_strdup(env->value));
            else
                return (ft_strdup("")); // Return an empty string if value is NULL
        }
        env = env->next;
    }
    return (ft_strdup("")); // Return an empty string if key is not found
}

char **create_single_result(char *str)
{
    char **result = malloc(sizeof(char*) * 2);
    if (!result) {
        free(str);
        return (NULL);
    }
    result[0] = str;
    result[1] = NULL;
    return (result);
}

// Handle the special case of "$"
char **expand_dollar_sign(void)
{
    char *str = ft_strdup("$");
    if (!str)
        return (NULL);
    return (create_single_result(str));
}

// Handle the special case of "$?"
char **expand_exit_status(t_minishell *shell)
{
    char *exit_str = ft_itoa(shell->exit_status);
    if (!exit_str)
        return (NULL);
    return (create_single_result(exit_str));
}

// ...existing code...

// Helper function to handle empty or null variable names
static char **handle_empty_var_name(void)
{
    return create_single_result(ft_strdup(""));
}

// Helper function to retrieve and validate the environment variable value
static char **get_valid_env_value(char **var_name, int c, t_minishell *shell)
{
    char **value;
    int i;

    i = -1;
    value = malloc(sizeof(char*) * (c + 1));
    if (!value)
        return (create_single_result(ft_strdup("")));
    while ( ++i < c)
    {
        value[i] = get_env_value(shell->envp, var_name[i]);
    }
    value[i] = NULL;
    if (!value || value[0] == NULL)
    {
        free(value);
        return (create_single_result(ft_strdup("")));
    }
    return value;
}

// Main function to expand environment variables
char **expand_env_variable(char **var_name, t_minishell *shell)
{
    char **value;
    // char **result;
    int c;

    if (!var_name && !var_name[0])
        return handle_empty_var_name();
    c = 0;
    while (var_name[c])
        c++;
    value = get_valid_env_value(var_name, c, shell);
    if (!value || !value[0])
        return create_single_result(ft_strdup(""));

    // result = split_env_value(value);
    // if (!result || !result[0])
    // {
    //     free(value); // Free value before returning
    //     if (result)
    //         free(result); // Free result if allocated
    //     return create_single_result(ft_strdup(""));
    // }

    // handle_trailing_space(result, value);
    // free(value); // Free value after use
    return value;
}

// Debug function to print expanded values
void debug_print_expansion(char **result)
{
    int i = 0;
    while (result[i]) {
        printf("expand: (%s)\n", result[i++]);
    }
}
char **expand_variable(char *var, t_minishell *shell)
{
    int i;
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
char	*expand_quote_variable(char *var, t_minishell *shell)
{
	char	*value;

	if (!var)
		return (NULL);
	if (var[1] == '\0')
		return (ft_strdup("$"));
	if (var[1] == '?' && var[2] == '\0')
		return (ft_itoa(shell->exit_status));
	value = get_env_value(shell->envp, &var[1]);
	if (value)
	{
		char	*dup = ft_strdup(value);
		free(value);
		return (dup);
	}
	return (ft_strdup(""));
}

// Helper function to extract variable name from input
char *extract_variable_name(const char *input, int *i)
{
    int j = *i + 1;

    // Handle special case: "$?"
    if (input[j] == '?')
        j++;

    // Extract variable name (alphanumeric or '_')
    while (ft_isalnum(input[j]) || input[j] == '_')
        j++;

    char *var_name = ft_substr(input, *i, j - *i); // Extract "$VAR"
    *i = j; // Update index to the end of the variable
    return var_name;
}

// Helper function to append expanded variable to the result
char *append_expanded_variable(char *result, char *var_name, t_minishell *shell)
{
    char *expanded = expand_quote_variable(var_name, shell); // Expand variable
    free(var_name); // Free variable name after use

    // Append expanded variable to the result
    char *temp = ft_strjoin(result, expanded);
    free(result); // Free old result
    free(expanded); // Free expanded variable
    return temp;
}

// Helper function to append a normal character to the result
char *append_normal_character(char *result, char c)
{
    char char_str[2] = {c, '\0'}; // Convert character to string
    char *temp = ft_strjoin(result, char_str); // Append character to result
    free(result); // Free old result
    return temp;
}

// Refactored expand_double_quotes function
char *expand_double_quotes(char *input, t_minishell *shell)
{
    char *result = ft_strdup(""); // Start with an empty string
    int i = 0;

    while (input[i])
    {
        if (input[i] == '$') // Found variable expansion
        {
            char *var_name = extract_variable_name(input, &i); // Extract variable name
            result = append_expanded_variable(result, var_name, shell); // Append expanded variable
        }
        else
        {
            result = append_normal_character(result, input[i]); // Append normal character
            i++;
        }
    }
    return result; // Return the final expanded result
}

static void update_token_with_expansion(t_token *token, char *expanded_value, int space)
{
    free(token->token);
    token->token = ft_strdup(expanded_value);
    token->type = TOKEN_WD;
    token->space = space;
}

static void handle_empty_expansion(t_token *token)
{
    free(token->token);
    token->token = ft_strdup("");
    token->type = TOKEN_WD;
}

static void free_expanded_value(char **expanded_value)
{
    for (int j = 0; expanded_value && expanded_value[j]; j++) {
        free(expanded_value[j]);
    }
    free(expanded_value);
}

static void process_double_quote_token(t_token *token, t_minishell *shell)
{
    char *d_qvalue = expand_double_quotes(token->token, shell);
    free(token->token);
    token->token = d_qvalue;
}

char **split_expanded_value(char *expanded_value)
{
    char **result;
    int  i = 0;
    int  word_count = 0;
    int  start, len, idx;

    // —— Pass 1: count words —— 
    while (expanded_value[i])
    {
        // skip any spaces
        while (expanded_value[i] && ft_isspace((unsigned char)expanded_value[i]))
            i++;
        // if we hit a non-space, that's a word
        if (expanded_value[i] && !ft_isspace((unsigned char)expanded_value[i]))
        {
            word_count++;
            // skip the rest of that word
            while (expanded_value[i] && !ft_isspace((unsigned char)expanded_value[i]))
                i++;
        }
    }

    // allocate pointer array (+1 for NULL)
    result = malloc(sizeof(char *) * (word_count + 1));
    if (!result)
        return (NULL);

    // —— Pass 2: extract each word —— 
    i = 0;
    idx = 0;
    while (expanded_value[i] && idx < word_count)
    {
        // skip spaces
        while (expanded_value[i] && ft_isspace((unsigned char)expanded_value[i]))
            i++;
        // mark word start
        start = i;
        // skip word
        while (expanded_value[i] && !ft_isspace((unsigned char)expanded_value[i]))
            i++;
        // length = end – start
        len = i - start;

        // grab substring
        result[idx] = ft_substr(expanded_value, start, len);
        if (!result[idx])
        {
            // on failure, free everything so far
            while (idx-- > 0)
                free(result[idx]);
            free(result);
            return (NULL);
        }
        idx++;
    }

    // terminate the array
    result[word_count] = NULL;
    return (result);
}


static void process_variable_token(t_token *token, t_list **current, t_minishell *shell)
{
    char *result;
    char **expanded_value;
    char **split_value;
    t_list  *insert_pos;
    t_list  *next_save;
    int i = 0;
    int space = 0;
    int space1 = 0;

    result = NULL;
    expanded_value = expand_variable(token->token, shell);
    if (!expanded_value || !expanded_value[0])
        return(handle_empty_expansion(token), free_arg(expanded_value));
    while (expanded_value && expanded_value[i])
    {
        printf("expanded value----------: (%s)\n", expanded_value[i]);
        if (result == NULL)
            result = ft_strdup(expanded_value[i]);
        else
        {
            char *tmp = result;
            result = ft_strjoin(tmp, expanded_value[i]);
            free(tmp);
        }
        i++;
    }
    printf("result----------: (%s)\n", result);
    i = 0;
    split_value = split_expanded_value(result);
    if (!split_value[1])
    {
        free_arg(split_value);
        split_value = create_single_result(ft_strdup(result));
    }
    free_arg(expanded_value);
    free(result);
    result = NULL;
    i = 0;
    insert_pos = *current;
    next_save = (*current)->next;
    space1 = token->space;
    printf("space----------: (%d)\n", space);
    while (split_value && split_value[i])
    {
        printf("split value----------: (%s)\n", split_value[i]);
        if (i == 0 && split_value[i + 1])
            update_token_with_expansion(token, split_value[i], 1);
        else if (i == 0 && !split_value[i + 1])
            update_token_with_expansion(token, split_value[i], token->space);
        else
        {
            printf("insert split value----------: (%s)\n", split_value[i]);
            if (split_value[i + 1])
                space = 1;
            else
                space = space1;
            t_list *new_node = create_token(ft_strdup(split_value[i]), TOKEN_WD, space);
            if (!new_node)
                break;
            // Insert new_node after insert_pos
            new_node->next = insert_pos->next;
            insert_pos->next = new_node;
            // Update insert_pos for next iteration
            insert_pos = new_node;
        }
        i++;
    }
    *current = insert_pos; // Update current to point to the last inserted node
    free_arg(split_value);
}

void expand_tokens(t_minishell *shell)
{
    t_list *current = shell->l_token;
    // t_list *before_c = shell->l_token;

    while (current) {
        t_token *token = (t_token *)current->content;

        if (token->type == TOKEN_VARIABLE) {
            process_variable_token(token, &current, shell);
        } else if (token->type == TOKEN_DQUOTE) {
            process_double_quote_token(token, shell);
        }

        // if (current != before_c && before_c->next)
        //     before_c = before_c->next;

        current = current->next;
    }
}
