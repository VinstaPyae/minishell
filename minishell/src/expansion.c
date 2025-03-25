#include "minishell.h"

char *get_env_value(t_env *env, char *key)
{
    while (env)
    {
        if (ft_strcmp(env->key, key) == 0)
            return (ft_strdup(env->value)); // Return the value if key matches
        env = env->next;
    }
    return (ft_strdup("")); // Return NULL if key is not found
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

// Handle environment variable expansion
char **expand_env_variable(char *var_name, t_minishell *shell)
{
    char *value;
    char **result;
    
    value = get_env_value(shell->envp, var_name);
    
    // Variable not found
    if (!value)
        return (free(value),create_single_result(ft_strdup("")));
    
    // Split the value if it contains spaces
    int i = ft_strlen(value);
    printf("value space (%c)\n", value[i-1]);
    result = ft_split(value, ' ');
    
    // Check if split resulted in empty array
    if (!result || !result[0]) {
        if (result)
            free(result);
        return (free(value),create_single_result(ft_strdup("")));
    }
    int j = 0;
    while (result[j])
    	j++;
    if (ft_isspace(value[i-1]))
    {
	result [j - 1] = ft_strjoin(result[j - 1], " "); //$$$$$$$$this could lead to segfault or conditional jump
    }
    printf("result space: (%s)\n", result[j - 1]);
    return (free(value),result);
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
    
    // Handle special cases
    if (var[1] == '\0')
        return (expand_dollar_sign());
    
    if (var[1] == '?' && var[2] == '\0')
        return (expand_exit_status(shell));
    
    // Handle normal environment variable
    result = expand_env_variable(&var[1], shell);
    
    // Debug output
    // debug_print_expansion(result);
    
    return (result);
}

char *expand_quote_variable(char *var, t_minishell *shell)
{
	char *value;

	if (!var)
		return (NULL);
	if (var[1] == '\0')
		return (ft_strdup("$"));

	if (var[1] == '?' && var[2] == '\0') // Handle $?
		return (ft_itoa(shell->exit_status));

	value = get_env_value(shell->envp, &var[1]);
	if (value)
		return (ft_strdup(value));
	
	return (ft_strdup(""));
}

char *expand_double_quotes(char *input, t_minishell *shell)
{
    char *result = ft_strdup(""); // Start with an empty string
    char *temp;
    int i = 0;

    while (input[i])
    {
        if (input[i] == '$') // Found variable expansion
        {
            int j = i + 1;
            
            // Handle special case: "$?"
            if (input[j] == '?')
                j++;

            // Extract variable name
            while (ft_isalnum(input[j]) || input[j] == '_')
                j++;

            char *var_name = ft_substr(input, i, j - i); // Extract "$VAR"
            char *expanded = expand_quote_variable(var_name, shell); // Expand it
            free(var_name);

            // Append the expanded variable to the result
            temp = ft_strjoin(result, expanded);
            free(result);
            free(expanded);
            result = temp;

            i = j; // Move past the variable
        }
        else
        {
            // Append normal character
            char char_str[2] = {input[i], '\0'}; // Convert char to string
            temp = ft_strjoin(result, char_str);
            free(result);
            result = temp;

            i++;
        }
    }
    return result;
}


/*
void expand_tokens(t_minishell *shell)
{
	int	i;
    t_list *current = shell->l_token;
    t_list *before_c = shell->l_token;
    
    i = 0;
    while (current)
    {
        t_token *token = (t_token *)current->content;
        // Handle variables inside double quotes or standalone
        if (token->type == TOKEN_VARIABLE)
        {
		//printf("before token: %s\n", ((t_token *)before_c->content)->token);
		char **expanded_value = expand_variable(token->token, shell);
		if (expanded_value[i + 1] != )
		free(token->token);
		token->token = expanded_value;
		if ((!expanded_value[0]) && ((t_token *)before_c->content)->space > 0)
			token->space = 0;
		// Convert TOKEN_VARIABLE → TOKEN_WD after expansion
		if (token->type == TOKEN_VARIABLE)
			token->type = TOKEN_WD;
            // If expansion results in an empty string, remove token
        	// if (!expanded_value[0])
		// {
		// t_list *to_remove = current;
		// current = current->next;
		// printf("list remove!\n");
		// remove_node(&shell->l_token, to_remove, c_token_destroy);
		// continue;
		// }
        }
	if (token->type == TOKEN_DQUOTE)
	{
		char *d_qvalue = expand_double_quotes(token->token, shell);
		free(token->token);
		token->token = d_qvalue;
	}
	if (current != before_c)
		before_c = before_c->next;
        current = current->next;
    }
}*/

void expand_tokens(t_minishell *shell)
{
    t_list *current = shell->l_token;
    t_list *before_c = shell->l_token;
    
    while (current)
    {
        t_token *token = (t_token *)current->content;
        
        if (token->type == TOKEN_VARIABLE)
        {
            char **expanded_value = expand_variable(token->token, shell);
            
            if (expanded_value && expanded_value[0]) {
                // Modify current token with first expanded value
                free(token->token);
                token->token = ft_strdup(expanded_value[0]);
                token->type = TOKEN_WD;

                // Check if there are multiple expanded values
                if (expanded_value[1]) {
                    t_list *next_save = current->next;
                    int i = 1;
                    while (expanded_value[i]) {
                        char *new_token_str = ft_strdup(expanded_value[i]);
                        
                        int flag;
                        if (expanded_value[i + 1] || token->space == 1)
                            flag = 1;
                        else
                            flag = 0;
                        t_list *new_token_node = create_token(new_token_str, TOKEN_WD, flag);
                        
                        if (!new_token_node) {
                            print_error(__func__, __FILE__, __LINE__, "Failed to create token for expanded value");
                            free(new_token_str);
                            break;
                        }
                        token->space = 1;
                        new_token_node->next = current->next;
                        current->next = new_token_node;
                        current = new_token_node;
                        
                        i++;
                    }
                    
                    current->next = next_save;
                }
		        else if (ft_isspace(token->token[ft_strlen(token->token) - 1]) || token->space == 1)
			        token->space = 1;
		        else
			        token->space = 0;
            } else {
                // Empty expansion
                free(token->token);
                token->token = ft_strdup("");
                token->type = TOKEN_WD;
            }
            
            // Free expanded_value array
            for (int j = 0; expanded_value && expanded_value[j]; j++)
                free(expanded_value[j]);
            free(expanded_value);
        }
        else if (token->type == TOKEN_DQUOTE)
        {
            char *d_qvalue = expand_double_quotes(token->token, shell);
            free(token->token);
            token->token = d_qvalue;
        }
        
        if (current != before_c && before_c->next)
            before_c = before_c->next;
            
        current = current->next;
    }
}

