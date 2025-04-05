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
static char *get_valid_env_value(char *var_name, t_minishell *shell)
{
    char *value = get_env_value(shell->envp, var_name);
    if (!value || value[0] == '\0')
    {
        free(value);
        return ft_strdup("");
    }
    return value;
}

// Helper function to split the environment variable value
static char **split_env_value(char *value)
{
    char **result = ft_split(value, ' ');
    if (!result || !result[0])
    {
        if (result)
            free(result);
        return create_single_result(ft_strdup(""));
    }
    return result;
}

// Helper function to handle trailing spaces in the last token
static void handle_trailing_space(char **result, char *value)
{
    int i = ft_strlen(value);
    int j = 0;

    if (i == 0) // Ensure value is not empty
        return;

    while (result[j])
        j++;

    if (ft_isspace(value[i - 1])) // Check the last character of value
    {
        char *tmp = ft_strjoin(result[j - 1], " ");
        if (!tmp)
            return; // Handle memory allocation failure
        free(result[j - 1]); // Free old string
        result[j - 1] = tmp; // Assign new string
    }
}

// Main function to expand environment variables
char **expand_env_variable(char *var_name, t_minishell *shell)
{
    char *value;
    char **result;

    if (!var_name)
        return handle_empty_var_name();

    value = get_valid_env_value(var_name, shell);
    if (!value)
        return create_single_result(ft_strdup(""));

    result = split_env_value(value);
    if (!result || !result[0])
    {
        free(value); // Free value before returning
        if (result)
            free(result); // Free result if allocated
        return create_single_result(ft_strdup(""));
    }

    handle_trailing_space(result, value);
    free(value); // Free value after use
    return result;
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
    char **result;
    
    if (!var)
        return (NULL);
    if (var[1] == '\0')
        return (expand_dollar_sign());
    if (var[1] == '?' && var[2] == '\0')
        return (expand_exit_status(shell));
    result = expand_env_variable(&var[1], shell);
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
static char *extract_variable_name(const char *input, int *i)
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
static char *append_expanded_variable(char *result, char *var_name, t_minishell *shell)
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
static char *append_normal_character(char *result, char c)
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

static void update_token_with_expansion(t_token *token, char **expanded_value)
{
    free(token->token);
    token->token = ft_strdup(expanded_value[0]);
    token->type = TOKEN_WD;
}

static void update_token_space_flag(t_token *token)
{
    if ((ft_strlen(token->token) > 0 && ft_isspace(token->token[ft_strlen(token->token) - 1])) || token->space == 1) {
        token->space = 1;
    } else {
        token->space = 0;
    }
}

static void insert_expanded_tokens(t_list **current, char **expanded_value, t_token *token)
{
    t_list *next_save = (*current)->next;
    int i = 1;

    while (expanded_value[i]) {
        char *new_token_str = ft_strdup(expanded_value[i]);
        int flag = (expanded_value[i + 1] || token->space == 1) ? 1 : 0;

        t_list *new_token_node = create_token(new_token_str, TOKEN_WD, flag);
        if (!new_token_node) {
            print_error(__func__, __FILE__, __LINE__, "Failed to create token for expanded value");
            free(new_token_str);
            break;
        }

        token->space = 1;
        new_token_node->next = (*current)->next;
        (*current)->next = new_token_node;
        *current = new_token_node;
        i++;
    }

    (*current)->next = next_save;
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

static void process_variable_token(t_token *token, t_list **current, t_minishell *shell)
{
    char **expanded_value = expand_variable(token->token, shell);

    if (expanded_value && expanded_value[0]) {
        update_token_with_expansion(token, expanded_value);
        if (expanded_value[1]) {
            insert_expanded_tokens(current, expanded_value, token);
        } else {
            update_token_space_flag(token);
        }
    } else {
        handle_empty_expansion(token);
    }

    free_expanded_value(expanded_value);
}

void expand_tokens(t_minishell *shell)
{
    t_list *current = shell->l_token;
    t_list *before_c = shell->l_token;

    while (current) {
        t_token *token = (t_token *)current->content;

        if (token->type == TOKEN_VARIABLE) {
            process_variable_token(token, &current, shell);
        } else if (token->type == TOKEN_DQUOTE) {
            process_double_quote_token(token, shell);
        }

        if (current != before_c && before_c->next)
            before_c = before_c->next;

        current = current->next;
    }
}
